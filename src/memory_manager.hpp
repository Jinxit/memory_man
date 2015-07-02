#pragma once

#include <functional>
#include <assert.h>

// interface for the memory managers
class memory_manager
{
protected:
	memory_manager(unsigned int memory_size) : memory_size(memory_size), free_size(memory_size)
	{
		// sanity check
		assert(memory_size > 0);
	};

	unsigned int free_size;

public:
	struct block
	{
		const unsigned int start;
		const unsigned int size;

		block() : start(0), size(0) { };
		block(unsigned int start, unsigned int size) : start(start), size(size) { };
	};

	const unsigned int memory_size;

	// allocate memory, the returned struct indicates where the block starts and its size
	virtual block alloc(unsigned int size) = 0;
	// free memory, it is assumed that the user knows the block has already been allocated
	// the block must be moved in to signify that the block is then invalid
	virtual void free(block&& b) = 0;
	// coalesce (merge) contiguous free blocks into larger blocks
	virtual void coalesce() = 0;

	float get_usage()
	{
		return float(memory_size - free_size) / memory_size;
	}
};