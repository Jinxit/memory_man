#pragma once

namespace memory_man
{
		struct block
		{
			const unsigned int start;
			const unsigned int size;

			block() : start(0), size(0) { };
			block(unsigned int start, unsigned int size) : start(start), size(size) { };
		};
}