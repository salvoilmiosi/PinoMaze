template<typename T, typename F> heap<T, F>::heap(F comp, size_t maxSize) : maxSize(maxSize), comp(comp) {
	items = new T[maxSize];
	curIndex = 0;
}

template<typename T, typename F> heap<T, F>::~heap() {
	delete[]items;
}

template<typename T, typename F> T heap<T, F>::removeFirst() {
	T firstItem = items[0];
	--curIndex;
	items[0] = items[curIndex];
    sortDown(0);
    return firstItem;
}

template<typename T, typename F> void heap<T, F>::insert(const T& item) {
	if (curIndex >= maxSize) return;

	items[curIndex] = item;
    sortUp(curIndex);
	++curIndex;
}

template<typename T, typename F> void heap<T, F>::update(const T& item) {
	// Should do sortDown
}

template<typename T, typename F> void heap<T, F>::sortUp(size_t index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (comp(items[parentIndex], items[index])) {
            swapItems(index, parentIndex);
            index = parentIndex;
        } else {
            break;
        }
    }
}

template<typename T, typename F> void heap<T, F>::sortDown(size_t index) {
    while (true) {
        size_t childIndex0 = (index * 2) + 1;
        size_t childIndex1 = (index * 2) + 2;

        size_t swapIndex = index;
        if (childIndex0 < curIndex && comp(items[index], items[childIndex0])) {
            swapIndex = childIndex0;
        }
        if (childIndex1 < curIndex && comp(items[index], items[childIndex1])) {
            swapIndex = childIndex1;
        }

        if (index != swapIndex) {
            swapItems(index, swapIndex);
            index = swapIndex;
        } else {
            return;
        }
    }
}

template<typename T, typename F> void heap<T, F>::clear() {
	curIndex = 0;
}

template<typename T, typename F> bool heap<T, F>::empty() {
	return curIndex == 0;
}

template<typename T, typename F> void heap<T, F>::swapItems(size_t index0, size_t index1) {
    std::swap(items[index0], items[index1]);
}

template<typename T, typename F> bool heap<T, F>::contains(const T& item) {
	for (T *i = items; i - items < (int) curIndex; ++i) {
		if (*i == item) {
			return true;
		}
	}
	return false;
}