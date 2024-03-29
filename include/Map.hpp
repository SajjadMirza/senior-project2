#ifndef MAP_HPP
#define MAP_HPP

#include <common.hpp>

// define BOOST_DISABLE_ASSERTS in production
#include <boost/multi_array.hpp>
#include <Entity.hpp>
#include <memory>

#define MAP_DIM 2



enum CellType {
    HALLWAY,
    WALL,
    EMPTY,
    GOAL,
    PUZZLE_FLOOR,
    START,
    HOLE,
    END,
    NODRAW,
    NOCEIL,
 };

struct MapCell {
    std::string name;
    CellType type;
    std::shared_ptr<Entity> component;
    std::shared_ptr<Entity> component2;
    MapCell();
    ~MapCell();
};

struct Neighbors {
    Entity *up;
    Entity *down;
    Entity *left;
    Entity *right;
    Neighbors() : up(NULL), down(NULL), left(NULL), right(NULL) {}
    ~Neighbors() {}
};


class Map {
private:
    boost::multi_array<MapCell, MAP_DIM> grid;
    uint columns;
    uint rows;
    std::vector<Eigen::Vector3f> major_light_positions;
    std::vector<Eigen::Vector3f> minor_light_positions;
    std::vector<Eigen::Vector3f> tiny_light_positions;
    Eigen::Vector3f player_start;

    void initWalls();

public:
    Map(uint columns, uint rows);
    ~Map();

    int loadMapFromFile(std::string filename);
    int loadMapFromImage(const char *filename);

    CellType getTypeForCell(uint col, uint row) const;
    Entity *getMapComponentForCell(uint col, uint row) const;
    Neighbors getNearbyWalls(uint col, uint row) const;
    
    uint getColumns() const;
    uint getRows() const;

    MapCell& get(uint col, uint row);
    const MapCell& cget(uint col, uint row) const;
    const std::vector<Eigen::Vector3f>& getMajorLightPositions() const;
    const std::vector<Eigen::Vector3f>& getMinorLightPositions() const;
    const std::vector<Eigen::Vector3f>& getTinyLightPositions() const;
    const Eigen::Vector3f& getPlayerStart() const;
};

#undef MAP_DIM // 

#endif
