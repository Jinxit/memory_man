#pragma once

#include "firstfit_manager.hpp"
#include "block.hpp"

namespace memory_man
{
	// first fit memory manager that sorts the free list by memory size
	// + low external fragmentation
	// + no internal fragmentation
	// - slow coalescence
	class first_fit_size : public first_fit
	{
	protected:
		static bool compare(const block& a, const block& b)
		{
			return a.size < b.size;
		}

	public:
		first_fit_size(unsigned int memory_size)
			: first_fit(memory_size, compare) { };

		virtual void coalesce()
		{
			// loop through all free blocks...
			for (auto it_a = free_blocks.begin(); it_a != free_blocks.end(); ++it_a)
			{
				// ...and again, starting from the first (!)
				for (auto it_b = free_blocks.begin(); it_b != free_blocks.end(); ++it_b)
				{
					// a block can't merge with itself
					if (it_a == it_b)
						continue;

					// if the first block's end matches the second block's start...
					if (it_a->start + it_a->size == it_b->start)
					{
						// merge them!
						free_blocks.emplace_hint(it_a, it_a->start, it_a->size + it_b->size);
						free_blocks.erase(it_a);
						free_blocks.erase(it_b);
					}
				}
			}
		}
	};
}