#ifndef __HEAP_H__
#define __HEAP_H__

template<typename T, typename F = less<T> >
class heap {
private:
	T *items;
	const unsigned int maxSize;
	unsigned int curIndex;

    F comp;

public:
	heap(F comp = less<T>(), unsigned int maxSize = 500);

	virtual ~heap();

    T removeFirst();

    void insert(const T& item);
	void update(const T& item);

    bool contains(const T& item);

	void clear();
	bool empty();

private:
    void sortUp(int index);
    void sortDown(int index);
    void swapItems(int index0, int index1);
};

#include "heap.inl"

#endif // __HEAP_H__
