#include "bit_array.h"

#include <cstring>

bitArray::bitArray() {
    bits = nullptr;
    _length = 0;
}

bitArray::bitArray(int length) {
    _length = length;
    if (_length % 8) {
        _length += 8 - _length % 8;
    }

    bits = new bool[_length];
    memset(bits, 0, _length);
}

bitArray::~bitArray() {
    delete[]bits;
}

int bitArray::write(ofstream &ofs, int bytes) {
    if (bytes == 0)
        bytes = _length / 8;

    bool *curBit = bits;

    static const unsigned char FILL_NUMS[] = {0x0, 0xff};
	for (int i = 0; i < bytes; ++i) {
		unsigned char curByte = 0;

        for (unsigned char mask = 0x01; mask != 0; mask <<= 1) {
			curByte |= mask & FILL_NUMS[*curBit];
            ++curBit;
        }
		ofs.put(curByte);
    }

    return bytes;
}

int bitArray::read(ifstream &ifs, int bytes) {
    if (bits == nullptr)
        bits = new bool[bytes * 8];

    bool *curBit = bits;

    if (bytes == 0)
        bytes = _length / 8;

	for (int i = 0; i < bytes; ++i) {
		unsigned char curByte = ifs.get();
        for (unsigned char mask = 0x01; mask != 0; mask <<= 1) {
            *curBit = (curByte & mask) != 0;
            ++curBit;
        }
    }
	return bytes;
}
