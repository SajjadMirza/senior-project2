#include <Map.hpp>

#include <fstream>
#include <cassert>

const char hallway_marker = '#';
const char empty_marker = ' ';
const char sentinel = '.';
const char goal = 'G';
const char puzzle_floor = 'g';
const char start = 'S';
const char hole = 'H';
const char end = 'E';

MapCell::MapCell() : type(EMPTY) {
}

MapCell::~MapCell() {
}

Map::Map(uint c, uint r) : grid(boost::extents[c][r]),
                           columns(c),
                           rows(r) {

}

Map::~Map() {
    
}

void Map::initWalls() {
    for (int col = 0; col < columns; col++) {
        for (int row = 0; row < rows; row++) {
            MapCell &current = grid[col][row];
            if (current.type != EMPTY) {
                continue;
            }
            
            if (col != 0) {
                MapCell &left = grid[col-1][row];
                if (left.type != EMPTY && left.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                }
            }
            if (row != 0) {
                MapCell &up = grid[col][row-1];
                if (up.type != EMPTY && up.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                }
            }
            if (col != columns - 1) {
                MapCell &right = grid[col+1][row];
                if (right.type != EMPTY && right.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                }
            }
            if (row != rows - 1) {
                MapCell &down = grid[col][row+1];
                if (down.type != EMPTY && down.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                }
            }
        }
    }
}

int Map::loadMapFromFile(std::string filename) {
    std::ifstream mapfile;
    mapfile.open(filename.c_str());
    
    if (!mapfile) {
        return -1;
    }
    
    bool done = false;
    int row = 0;
    while (mapfile && !done) {
        std::string line;
        std::getline(mapfile, line);
        std::cout << row << std::endl;
        
        for (int col = 0; col < columns && !done; col++) {
//            std::cout << col << " ";
            switch (line.at(col)) {
            case hallway_marker:
                grid[col][row].type = HALLWAY;
                grid[col][row].name = "HALLWAY";
                break;
            case goal:
                grid[col][row].type = HALLWAY;
                grid[col][row].name = "HALLWAY";
                break;
            case puzzle_floor:
                grid[col][row].type = HALLWAY;
                grid[col][row].name = "HALLWAY";
                break;
             case start:
                grid[col][row].type = HALLWAY;
                grid[col][row].name = "HALLWAY";
                break;
            case hole:
                grid[col][row].type = HALLWAY;
                grid[col][row].name = "HALLWAY";
                break;
            case end:
                grid[col][row].type = END;
                grid[col][row].name = "END";
                break;
            case empty_marker:
                break;
            case sentinel:
                done = true;
                break;
            default:
                break;
//               assert(0 && __FILE__ && __LINE__);
            }
        }
        row++;
    }

    initWalls();
    
    return 0;
}

CellType Map::getTypeForCell(uint col, uint row) const {
    return grid[col][row].type;
}

Entity *Map::getMapComponentForCell(uint col, uint row) const {
    return grid[col][row].component.get();
}

Neighbors Map::getNearbyWalls(uint col, uint row) const {
    Neighbors walls;
    if (col < 0 || col >= columns || row < 0 || row >= rows) {
        return walls;
    }
    
    if (col > 0) {
        const MapCell &cell = grid[col-1][row];
        if (cell.type == WALL) {
            walls.left = cell.component.get();
        }
    }

    if (row > 0) {
        const MapCell &cell = grid[col][row-1];
        if (cell.type == WALL) {
            walls.up = cell.component.get();
        }

    }

    if (col < columns - 1) {
        const MapCell &cell = grid[col+1][row];
        if (cell.type == WALL) {
            walls.right = cell.component.get();
        }
    }

    if (row < rows - 1) {
        const MapCell &cell = grid[col][row+1];
        if (cell.type == WALL) {
            walls.down = cell.component.get();
        }

    }

    return walls;
}


uint Map::getColumns() const {
    return columns;
}
uint Map::getRows() const {
    return rows;
}

MapCell& Map::get(uint col, uint row) {
    return grid[col][row];
}

const MapCell& Map::cget(uint col, uint row) const {
    return grid[col][row];
}
