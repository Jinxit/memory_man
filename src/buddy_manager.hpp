#pragma once

#include <set>
#include <map>
#include <functional>
#include <assert.h>
#include "memory_manager.hpp"

// memory manager that uses the buddy system
// https://en.wikipedia.org/wiki/Buddy_memory_allocation
// + fast coalescence
// + low external fragmentation
// - possibly high internal fragmentation
class buddy_manager : public memory_manager
{
protected:
	using compare_function = std::function<bool(const block&, const block&)>;
	std::map<unsigned int, std::multiset<block, compare_function>> free_blocks;

	static bool compare(const block& a, const block& b)
	{
		return a.start < b.start;
	}

public:
	buddy_manager(unsigned int memory_size, unsigned int min_block_size)
		: memory_manager(memory_size)
		{
			// check that memory_size is divisible by min_block_size
			assert(memory_size % min_block_size == 0);
			// check that memory_size == min_block_size * 2^n for some n
			assert(((memory_size / min_block_size) != 0) 
				&& (((memory_size / min_block_size) 
					& (~(memory_size / min_block_size) + 1))
				== (memory_size / min_block_size)));

			// create empty lists for all sizes
			for (unsigned int size = min_block_size; size <= memory_size; size *= 2)
			{
				free_blocks[size] = std::multiset<block, compare_function>(compare);
			}
			// and place a single full sized block on the largest block size list
			free_blocks[memory_size].emplace_hint(free_blocks[memory_size].begin(), 0, memory_size);
		};

	virtual block alloc(unsigned int size)
	{
		// the allocation must not exceed the maximum memory
		if (size > free_size)
			return block();

		free_size -= size;

		for (auto& kvp : free_blocks)
		{
			// find the smallest block size that still fits
			if (kvp.first >= size)
			{
				// if there are no blocks available at that size...
				if (kvp.second.size() == 0)
				{
					// find the smallest block size list which contains at least one free block
					// since we only end up here if the original block size list is empty,
					//  we can start at the next one thereafter
					unsigned int lowest_available = kvp.first * 2;
					while (free_blocks[lowest_available].size() == 0)
					{
						lowest_available *= 2;
					}

					do
					{
						// split the large block into two equally sized smaller blocks
						auto it = free_blocks[lowest_available].begin();
						free_blocks[lowest_available / 2].emplace(it->start, it->size / 2);
						free_blocks[lowest_available / 2].emplace(it->start + it->size / 2, it->size / 2);
						free_blocks[lowest_available].erase(it);

						// go down a size
						lowest_available /= 2;

						// until we reach the desired size
					} while (lowest_available > kvp.first);
				}

				// if/once we have an available block at the right size,
				//  delete it from the free list to allocate it
				auto it = kvp.second.begin();
				block result(it->start, size);
				kvp.second.erase(it);
				return result;
			}
		}

		// no slot available
		return block();
	}

	virtual void free(block&& b)
	{
		free_size += b.size;
		for (auto& kvp : free_blocks)
		{
			// find the smallest block size that still fits
			if (kvp.first >= b.size)
			{
				// and insert the block into the free list
				kvp.second.insert(std::forward<block>(b));
				return;
			}
		}
	}

	virtual void coalesce()
	{
		for (auto it_a = free_blocks.begin(); it_a != free_blocks.end(); ++it_a)
		{
			// if there are at least two blocks available of the same size...
			while (it_a->second.size() >= 2)
			{
				// merge them!
				auto it_b = it_a;
				++it_b;
				it_b->second.emplace_hint(it_b->second.begin(), it_a->second.begin()->start, it_b->first);
				it_a->second.erase(it_a->second.begin());
				it_a->second.erase(it_a->second.begin());
			}
		}
	}
};