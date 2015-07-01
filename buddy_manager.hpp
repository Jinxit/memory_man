#pragma once

#include "memory_manager.hpp"
#include <set>
#include <map>
#include <iostream>
#include <assert.h>

class buddy_manager : public memory_manager
{
protected:
	using compare_function = std::function<bool(const block&, const block&)>;

	static bool compare(const block& a, const block& b)
	{
		return a.start < b.start;
	}

	std::map<unsigned int, std::multiset<block, compare_function>> free_blocks;

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

			for (unsigned int size = min_block_size; size <= memory_size; size *= 2)
			{
				free_blocks[size] = std::multiset<block, compare_function>(compare);
			}
			free_blocks[memory_size].emplace_hint(free_blocks[memory_size].begin(), 0, memory_size);
		};

	virtual block alloc(unsigned int size)
	{
		if (size > free_size)
			return block();

		free_size -= size;

		for (auto& kvp : free_blocks)
		{
			if (kvp.first >= size)
			{
				if (kvp.second.size() == 0)
				{
					unsigned int lowest_available = kvp.first * 2;
					while (free_blocks[lowest_available].size() == 0)
					{
						lowest_available *= 2;
					}

					while (lowest_available > kvp.first)
					{
						auto it = free_blocks[lowest_available].begin();
						free_blocks[lowest_available / 2].emplace(it->start, it->size / 2);
						free_blocks[lowest_available / 2].emplace(it->start + it->size / 2, it->size / 2);
						free_blocks[lowest_available].erase(it);
						lowest_available /= 2;
					}
				}

				auto it = kvp.second.begin();
				block result(it->start, size);
				kvp.second.erase(it);
				return result;
			}
		}

		// no slot found
		return block();
	}

	virtual void free(block&& b)
	{
		free_size += b.size;
		for (auto& kvp : free_blocks)
		{
			if (kvp.first >= b.size)
			{
				kvp.second.insert(std::forward<block>(b));
				return;
			}
		}
	}

	virtual void coalesce()
	{
		for (auto it_a = free_blocks.begin(); it_a != free_blocks.end(); ++it_a)
		{
			while (it_a->second.size() >= 2)
			{
				auto it_b = it_a;
				++it_b;
				it_b->second.emplace_hint(it_b->second.begin(), it_a->second.begin()->start, it_b->first);
				it_a->second.erase(it_a->second.begin());
				it_a->second.erase(it_a->second.begin());
			}
		}
	}
};