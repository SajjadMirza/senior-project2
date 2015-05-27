#ifndef MAP_HPP
#define MAP_HPP

#include <common.hpp>

// define BOOST_DISABLE_ASSERTS in production
#include <boost/multi_array.hpp>

#define MAP_DIM 2



enum CellType {
    HALLWAY,
    WALL,
    EMPTY,
};

struct MapCell {
    CellType type;
    MapCell();
    ~MapCell();
};


class Map {
private:
    boost::multi_array<MapCell, MAP_DIM> grid;
    uint columns;
    uint rows;

    void initWalls();

public:
    Map(uint columns, uint rows);
    ~Map();

    int loadMapFromFile(std::string filename);
};

#undef MAP_DIM // 

#endif
