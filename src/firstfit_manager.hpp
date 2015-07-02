#pragma once

#include <set>
#include <functional>
#include "memory_manager.hpp"

// interface for the first fit memory managers
class firstfit_manager : public memory_manager
{
protected:
	using compare_function = std::function<bool(const block&, const block&)>;
	std::multiset<block, compare_function> free_blocks;

public:
	firstfit_manager(unsigned int memory_size, compare_function comp)
		: memory_manager(memory_size), free_blocks(comp)
		{
			free_blocks.emplace_hint(free_blocks.begin(), 0, memory_size);
		};

	virtual block alloc(unsigned int size)
	{
		// the allocation must not exceed the maximum memory
		if (size > free_size)
			return block();

		free_size -= size;

		for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it)
		{
			// find the first block that fits
			// the list is sorted by the implementation
			if (it->size > size)
			{
				// reserve memory at the start of the free block,
				//  let the rest remain as a free block
				block result(it->start, size);
				free_blocks.emplace_hint(it, it->start + size, it->size - size);
				free_blocks.erase(it);
				return result;
			}
		}
	}

	virtual void free(block&& b)
	{
		free_size += b.size;

		// simply add the block to the free list, sorting takes care of the position
		free_blocks.insert(std::forward<block>(b));
	}
};