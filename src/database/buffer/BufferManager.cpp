#include <stdlib.h>
#include <stdint.h>
#include "BufferManager.hpp"
#include "../util/TwoQList.h"
#include "../util/debug.h"

const unsigned PAGE_SIZE_BYTE = 4096;
const uint64_t SEGMENT_MASK = 0xFFFF000000000000;
const uint64_t PAGE_MASK = 0xFFFFFFFFFFFF;

/* Keeps up to size frames in main memory*/
BufferManager::BufferManager(uint64_t pagesInMemory) {
	this->maxFramesInMemory = pagesInMemory;
	this->pageIO = new PageIOUtil();
	this->replacementStrategy = new TwoQList();
	this->initCache(this->maxFramesInMemory);
}

/* Writes all dirty frames to disk and free all resources */
BufferManager::~BufferManager() {
	debug("tear down\n");
	/* write back all frames */
	BufferFrame *frame;
	while ((frame = replacementStrategy->pop()) != nullptr) {
		if (frame->isDirty()) {
			writeOut(frame);
		}
	}

	this->freeCache();
	delete this->pageIO;
	delete this->replacementStrategy;
}

/* Retrieve frames given a page Id and indication whether or not it is exclusive for the thread
 * Can fail, if no free frame is available and no used frame can be freed*/
BufferFrame &BufferManager::fixPage(uint64_t pageAndSegmentId, bool exclusive) {
	uint64_t pageId, segmentId;
	this->extractPageAndSegmentId(pageAndSegmentId, pageId, segmentId);

	debug(pageId, "Page Id extracted");
	debug(pageId, "trying to get global lock");
	this->global_lock();
	debug(pageId, "global lock aquired");

	BufferFrame *frame = this->getPageInMemoryOrNull(pageId);
	if (frame != nullptr) {
		//frame exists
		debug(pageId, "Frame exists in Memory");
		frame->increaseUsageCount();
		this->replacementStrategy->remove(frame);
		frame->setUsedBefore();
		this->global_unlock();
		debug(pageId, "global lock released");
		frame->lock(exclusive);
	} else {
		// frame does not exist
		debug(pageId, "Frame for pageId %llu does not exist", pageId);
		if (this->isSpaceAvailable()) { // don't have to replace anything
			debug(pageId, "space available -> create frame");
			frame = this->createFrame(pageId, segmentId);
			frame->setUnusedBefore();
			debug(pageId, "New frame created");
			this->global_unlock();
			debug(pageId, "global unlocked");
		} else {
			debug(pageId, "no space available -> replace");
			frame = this->replacementStrategy->pop();
			debug(pageId, "Popping frame with page id %llu",
				  frame != nullptr ? frame->getPageId() : (unsigned long long) -1);
			if (frame == nullptr) {
				this->global_unlock();
				throw "Frame is not swapped in, no space is available and no pages are poppable";
			}
			// write out if necessary
			if (frame->isDirty()) {
				debug(pageId, "locking frame for dirty write out");
				frame->lock(false);
				this->global_unlock();
				this->writeOut(frame);
				debug(pageId, "written out");
				this->global_lock();
				frame->unlock();
			}
			this->reinitialize(frame, pageId);
			debug(pageId, "frame reinitialized");
		}
		debug(pageId, "Lock frame to load from disk");
		frame->lock(true); // TODO: optimize - only lock if page exists
		debug(pageId, "frame locked");
		this->loadFromDiskIfExists(frame);
		debug(pageId, "loaded from disk");
		frame->unlock();
		debug(pageId, "unlocked frame");
		debug(pageId, "try to lock frame with id: %llu | exclusive: %s", frame->getPageId(),
			  exclusive ? "true" : "false");
		frame->lock(exclusive);
		debug(pageId, "Waiting count: %llu", frame->getWaitingCount());
	}
	this->global_unlock();
	return *frame;
}

/**
 * Return a frame to the buffer manager indicating whether it is dirty or not.
 * If dirty, the page manager must write it back to disk.
 * It does not have to write it back immediately,
 * but must not write it back before unfixPage is called.
 */
void BufferManager::unfixPage(BufferFrame &frame, bool isDirty) {
	this->global_lock();

	/*
	 * Only set the dirty flag.
	 * The frame will be swapped out when it is replaced in memory
	 * or when the BufferManager dies.
	 * Hence, the data pointer of the frame is not added to the free pages
	 * because the page data is still occupied although it can be replaced.
	 */
	frame.setDirty(isDirty);
	frame.decreaseUsageCount();
	if (!frame.isUsed()) {
		this->replacementStrategy->push(&frame);
	}
	frame.unlock();
	this->global_unlock();
}

void BufferManager::extractPageAndSegmentId(uint64_t pageAndSegmentId, uint64_t &pageId, uint64_t &segmentId) {
	pageId = pageAndSegmentId & PAGE_MASK;
	segmentId = pageAndSegmentId & SEGMENT_MASK;
}

void BufferManager::initCache(uint64_t pages) {
	this->cache = malloc((size_t) (pages * PAGE_SIZE_BYTE));
	for (int i = 0; i < pages; ++i) {
		this->freePages.push_back(this->cache + i * PAGE_SIZE_BYTE);
	}
}

void BufferManager::freeCache() {
	debug("Freeing cache\n");
	free(this->cache);
}

BufferFrame *BufferManager::getPageInMemoryOrNull(uint64_t pageId) {
	return this->pageFrameMap[pageId];
}

BufferFrame *BufferManager::createFrame(uint64_t pageId, uint64_t segmentId) {
	void *page = this->getFreePage();
	BufferFrame *frame = new BufferFrame(pageId, segmentId, page);
	this->pageFrameMap[pageId] = frame;
	return frame;
}

bool BufferManager::isSpaceAvailable() {
	return !this->freePages.empty();
}

void *BufferManager::getFreePage() {
	void *result = this->freePages.front();
	this->freePages.pop_front();
	return result;
}

void BufferManager::writeOut(BufferFrame *frame) {
	this->pageIO->writePage(frame->getPageId(), frame->getSegmentId(), frame->getData(), PAGE_SIZE_BYTE);
}

void BufferManager::reinitialize(BufferFrame *frame, uint64_t newPageId) {
	this->pageIO->writePage(frame->getPageId(), frame->getSegmentId(), frame->getData(), PAGE_SIZE_BYTE);
	this->pageFrameMap.erase(frame->getPageId());
	frame->resetFlags();
	frame->setUnusedBefore();
	this->pageFrameMap[newPageId] = frame;
}

void BufferManager::loadFromDiskIfExists(BufferFrame *frame) {
	this->pageIO->readPage(frame->getPageId(), frame->getSegmentId(), frame->getData(), PAGE_SIZE_BYTE);
}

void BufferManager::global_lock() {
	this->global_mutex.lock();
}

void BufferManager::global_unlock() {
	this->global_mutex.unlock();
}