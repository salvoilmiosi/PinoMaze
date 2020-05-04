#ifndef __WALL_H__
#define __WALL_H__

constexpr int WALL_THICKNESS = 2;

#include <vector>

using uint8_t = unsigned char;

struct wall_section {
    uint8_t value;

    wall_section(uint8_t value = 0) : value(value) {}

    bool solid() const {
        return value > 0;
    }
};

class wall : public std::vector<wall_section> {
public:
    wall(int length = 0, int value = 0) : std::vector<wall_section>(length, value) {};

    void move(int offset) {
        if (offset > 0) {
            resize(size() - offset);
            insert(begin(), offset, wall_section());
        } else if (offset < 0) {
            erase(begin(), begin() - offset);
            resize(size() - offset);
        }
    }
};
#endif // __WALL_H__
