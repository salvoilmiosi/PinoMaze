#ifndef __WALL_H__
#define __WALL_H__

extern const int WALL_THICKNESS;

class wall {
private:
    bool *data;
    int _length;

public:
    wall(int length = 0);
    wall(const wall &w);

    virtual ~wall();

    wall & operator = (const wall &w);

public:
    int length() const {
        return _length;
    }

    void resizeWall(int newLength);

    void moveWall(int offset);

    bool isEmpty() const {
        return data == nullptr || _length == 0;
    }

    bool &operator [](int index) const;

private:
    void makeEmpty(int length);
};
#endif // __WALL_H__
