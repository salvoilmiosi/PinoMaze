#ifndef __BIT_ARRAY_H__
#define __BIT_ARRAY_H__

#include <fstream>



class bitArray {
private:
    bool *bits;
    int _length;

public:
    bitArray();
    bitArray(int length);

    ~bitArray();

public:
    int length() {
        return _length;
    }

    int write(std::ofstream &ofs, int bytes = 0);
    int read(std::ifstream &ifs, int bytes = 0);

    bool &operator [](int i) {
        return bits[i];
    }
};

#endif // __BIT_ARRAY_H__
