#pragma once

#include <functional>

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

	using compare_function = std::function<bool(const block&, const block&)>;

	static bool compare_increasing_size(const block& a, const block& b)
	{
		return a.size < b.size;
	}

	static bool compare_memory_location(const block& a, const block& b)
	{
		return a.start < b.start;
	}
};