#include "maze_items.h"

#include "maze.h"

#include <cstring>

mazeItem makeItem(itemType type) {
    mazeItem item;
    memset(&item, 0, sizeof item);
    item.type = type;
    return item;
}

int itemIndex(const mazeItem &item, int w) {
    return (item.item.y * w + item.item.x);
}

bool arrowPointsTo(const mazeItem &item, int ox, int oy) {
    if (item.type != ITEM_ARROW) return false;

    if (item.arrow.direction == ARROW_UP && (ox==0 && oy<0)) {
        return true;
    } else if (item.arrow.direction == ARROW_DOWN && (ox==0 && oy>0)) {
        return true;
    } else if (item.arrow.direction == ARROW_LEFT && (ox<0 && oy==0)) {
        return true;
    } else if (item.arrow.direction == ARROW_RIGHT && (ox>0 && oy==0)) {
        return true;
    }
    return false;
}
