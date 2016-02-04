#include "wall.h"

#include "tile.h"

#include <cstring>

const int WALL_THICKNESS = 2;

wall::wall(int length) {
    makeEmpty(length);
}

wall::wall(const wall &w) {
    data = nullptr;
    *this = w;
}

wall::~wall() {
    if (data != nullptr)
        delete[] data;
}

wall &wall::operator = (const wall &w) {
    if (data != nullptr)
        delete[]data;
    _length = w._length;
    data = new bool[_length];
    if (w.data != nullptr)
        memcpy(data, w.data, _length);
    return *this;
}

void wall::resizeWall(int newLength) {
    if (data == nullptr) {
        makeEmpty(newLength);
    } else if (newLength < _length) {
        memset(data + newLength, 0, _length-newLength);
        _length = newLength;
    } else if (newLength > _length) {
        bool *newData = new bool[newLength];
        memset(newData, 0, newLength);

        memcpy(newData, data, _length);

        delete[]data;

        data = newData;
        _length = newLength;
    }
}

void wall::moveWall(int offset) {
    if (data == nullptr) return;

    if (offset < 0) {
        memmove(data, data-offset, _length+offset);
        memset(data+_length+offset, 0, -offset);
    } else if (offset > 0) {
        memmove(data+offset, data, _length-offset);
        memset(data, 0, offset);
    }
}

void wall::makeEmpty(int length) {
    _length = length;
    if (length == 0) {
        data = nullptr;
    } else {
        data = new bool[length];
        memset(data, 0, length);
    }
}

bool &wall::operator [](int index) const {
    return data[index];
}
