#pragma once

#include <set>
#include "memory_manager.hpp"

class firstfit_manager : public memory_manager
{
protected:
	using compare_function = std::function<bool(const block&, const block&)>;
	std::multiset<block, compare_function> free_blocks;

	virtual void coalesce() = 0;

public:
	firstfit_manager(unsigned int memory_size, compare_function comp)
		: memory_manager(memory_size), free_blocks(comp)
		{
			free_blocks.emplace_hint(free_blocks.begin(), 0, memory_size);
		};

	virtual block alloc(unsigned int size)
	{
		if (size > free_size)
			return block();

		free_size -= size;

		for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it)
		{
			if (it->size > size)
			{
				block result(it->start, size);
				free_blocks.emplace_hint(it, it->start + size, it->size - size);
				free_blocks.erase(it);
				return result;
			}
		}
		//TODO: compact memory here
	}

	virtual void free(block&& b)
	{
		free_size += b.size;
		free_blocks.insert(std::forward<block>(b));
		coalesce();
	}
};