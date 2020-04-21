#ifndef __HEAP_H__
#define __HEAP_H__

#include <functional>

template<typename T, typename F = std::less<T> >
class heap {
private:
	const size_t maxSize;
	
    F comp;

	T *items;
	size_t curIndex;

public:
	heap(F comp = std::less<T>(), size_t maxSize = 500);

	~heap();

    T removeFirst();

    void insert(const T& item);
	void update(const T& item);

    bool contains(const T& item);

	void clear();
	bool empty();

private:
    void sortUp(size_t index);
    void sortDown(size_t index);
    void swapItems(size_t index0, size_t index1);
};

#include "heap.inl"

#endif // __HEAP_H__
