#ifndef PROJECT_BUFFER_FRAME_H
#define PROJECT_BUFFER_FRAME_H

class BufferFrame {
	const uint8_t DIRTY_MASK = 0x1; // 001
	const uint8_t EXCLUSIVE_MASK = 0x2; // 010
	const uint8_t UNFIXED_MASK = 0x4; // 100

private:
	void *data;
	uint64_t pageId;
	uint8_t state; // combine dirty and exclusive flag - see chapter 2, slide 17
	boost::shared_mutex latch; // TODO: make sure false conflicts are avoided - modulo(sizeof(BufferFrame), cache_line_size_byte = 64) = 0!

	bool isFlagSet(uint8_t mask);

	void setFlag(uint8_t mask);

	void unsetFlag(uint8_t mask);

public:
	BufferFrame(uint64_t pageId, void *page);

	void *getData();

	bool isDirty();

	bool isExclusive();

	bool isUnfixed();

	uint64_t getPageId();

	uint64_t getSegmentId();
};

#endif //PROJECT_BUFFER_FRAME_H