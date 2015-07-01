#pragma once

#include "firstfit_manager.hpp"

class firstfit_location_manager : public firstfit_manager
{
protected:
	static bool compare(const block& a, const block& b)
	{
		return a.start < b.start;
	}

public:
	firstfit_location_manager(unsigned int memory_size)
		: firstfit_manager(memory_size, compare) { };

	virtual void coalesce()
	{
		for (auto it_a = free_blocks.begin(); it_a != free_blocks.end(); ++it_a)
		{
			for (auto it_b = it_a; it_b != free_blocks.end(); ++it_b)
			{
				if (it_a == it_b)
					continue;

				if (it_a->start + it_a->size == it_b->start)
				{
					free_blocks.emplace_hint(it_a, it_a->start, it_a->size + it_b->size);
					free_blocks.erase(it_a);
					free_blocks.erase(it_b);
					++it_a;
					break;
				}
			}
		}
	}
};