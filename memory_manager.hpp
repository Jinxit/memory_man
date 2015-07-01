#pragma once

class memory_manager
{
protected:
	memory_manager(unsigned int memory_size) : memory_size(memory_size) { };
public:
	struct block
	{
		const unsigned int start;
		const unsigned int size;

		block(unsigned int start, unsigned int size) : start(start), size(size) { };
	};

	const unsigned int memory_size;

	virtual block alloc(unsigned int size) = 0;
	virtual void free(block b) = 0;
};