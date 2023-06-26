#include <cassert>
#include "map.h"

static const char map[] = "2233244334423322"\
                          "2              0"\
                          "3  5       4   0"\
                          "3  5       4   0"\
                          "2  5       4   0"\
                          "4  5           0"\
                          "4  5           0"\
                          "1  5       2   0"\
                          "1  5       2   0"\
                          "4  5       2   0"\
                          "4          2   0"\
                          "2          2   0"\
                          "3              0"\
                          "3   2222       0"\
                          "2              0"\
                          "2233244334423322";

Map::Map() : w(16), h(16) {
    assert(sizeof(map) == w * h + 1);
}

int Map::get(const size_t i, const size_t j) const {
    assert(i < w&& j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] - '0';
}

bool Map::is_empty(const size_t i, const size_t j) const {
    assert(i < w&& j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] == ' ';
}