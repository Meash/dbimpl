project(database)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -lto")

set(SOURCE_FILES buffer/BufferFrame.cpp buffer/BufferFrame.hpp buffer/BufferManager.cpp buffer/BufferManager.hpp testing/BufferManagerTest.cpp buffer/IPageIO.h util/Math.cpp util/Math.h)
add_executable(database ${SOURCE_FILES} util/LruCache.h testing/LruCacheTest.cpp buffer/IReplacementStrategy.h)