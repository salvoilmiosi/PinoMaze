template<typename T>
grid<T>::grid(const grid<T> &g) {
    delete[]data;

    w = g.w;
    h = g.h;
    data = new T[g.datasize()];
    memcpy(data, g.data, g.datasize() * sizeof *data);
}

template<typename T>
grid<T>::grid(grid<T> &&g) {
    data = g.data;
    w = g.w;
    h = g.h;
    g.data = nullptr;
}

template<typename T>
void grid<T>::copyGrid(const grid<T> &g) {
    if (data == nullptr) {
        data = new T[g.datasize()];
        w = g.w;
        h = g.h;
    }
    if (w == g.w && h == g.h) {
        memcpy(data, g.data, g.datasize() * sizeof *data);
    } else {
        int x = 0;
        int y = 0;
        for (int i = 0; i<g.datasize(); ++i) {
            x = i % g.w;
            y = i / g.w;
            T *item = getItem(x, y);
            if (item) *item = *g.getItem(x, y);
        }
    }
}

template<typename T>
void grid<T>::resizeGrid(int newW, int newH) {
    if (data == nullptr) {
        data = new T[w * h];
        w = newW;
        h = newH;
    } else {
        grid<T> newGrid(newW, newH);
        newGrid.copyGrid(*this);

        *this = newGrid;
    }
}

template<typename T>
void grid<T>::moveGrid(int ox, int oy, const T &newObj) {
    if (data == nullptr) return;

    int offset = oy * w + ox;
    if (offset < 0) {
        for (int i = -offset; i < datasize()-offset; ++i) {
            if (i < datasize()) {
                data[i+offset] = data[i];
            } else {
                data[i+offset] = newObj;
            }
        }
    } else if (offset > 0) {
        for (int i = datasize()-offset-1; i >= -offset; --i) {
            if (i >= 0) {
                data[i+offset] = data[i];
            } else {
                data[i+offset] = newObj;
            }
        }
    }
}

template<typename T>
void grid<T>::clear() {
	delete[]data;
	data = new T[datasize()];
}