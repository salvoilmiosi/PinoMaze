#ifndef __GRID_3D_H__
#define __GRID_3D_H__

#include "grid.h"

struct vector3d {
int x;
int y;
int z;
};

template <typename T>
class grid3d {
protected:
	T *data;

    int w;
    int h;
    int _depth;

public:
    grid3d (int w = 0, int h = 0, int depth = 0): w(w), h(h), _depth(depth) {
        if (w==0 || h==0 || depth==0) {
            data = nullptr;
        } else {
            data = new T[w * h * depth];
        }
    }

	virtual ~grid3d() {
	    delete[]data;
	}

	T *getItem(int x, int y, int z = 0) {
        if (x < 0 || x >= w) return nullptr;
        if (y < 0 || y >= h) return nullptr;
        if (z < 0 || z >= _depth) return nullptr;

        return data + (y * w + x)*_depth + z;
    }

	T *getItem(vector3d coords) {
		return getItem(coords.x, coords.y, coords.z);
	}

    vector3d getCoords(const T *item);

public:
	int width() {
		return w;
	}

	int height() {
		return h;
	}

	int depth() {
        return _depth;
	}

	int datasize() {
        return w * h *_depth;
	}

	void resetGrid(int newW, int newH, int newDepth);
};

#include "grid3d.inl"

#endif // __GRID_3D_H__
