#ifndef __BIT_ARRAY_H__
#define __BIT_ARRAY_H__

#include <fstream>

using namespace std;

class bitArray {
private:
    bool *bits;
    int _length;

public:
    bitArray();
    bitArray(int length);

    virtual ~bitArray();

public:
    int length() {
        return _length;
    }

    int write(ofstream &ofs, int bytes = 0);
    int read(ifstream &ifs, int bytes = 0);

    bool &operator [](int i) {
        return bits[i];
    }
};

#endif // __BIT_ARRAY_H__
