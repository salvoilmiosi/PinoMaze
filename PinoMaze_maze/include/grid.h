#ifndef __GRID_H_
#define __GRID_H_

template<typename T>
class grid {
protected:
	T *data;
	int w, h;

public:
	grid(int w, int h) : w(w), h(h) {
        data = new T[datasize()];
	}

	grid (const grid &g);

	grid (grid &&g);

	virtual ~grid() {
	    delete[] data;
	}

	T *getItem(int x, int y) const {
        if (x < 0 || x >= w) return nullptr;
        if (y < 0 || y >= h) return nullptr;

        return data + (y*w + x);
    }

    T *getItem (int index) const {
        if (index >= w*h || index < 0) return nullptr;
        return data + index;
    }

    int getIndex (T *item) const {
		return (int)(item - data);
    }

	int width() const {
		return w;
	}

	int height() const {
		return h;
	}

	int datasize() const {
	    return w * h;
	}

	void copyGrid(const grid<T> &g);

	void resizeGrid(int newW, int newH);

	void moveGrid(int ox, int oy, const T &newObj);

	void clear();
};

#include "grid.inl"

#endif // __GRID_H_

