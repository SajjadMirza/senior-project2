#include <Map.hpp>

#include <fstream>
#include <cassert>

const char hallway_marker = '#';
const char empty_marker = ' ';
const char sentinel = '.';

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
            if (current.type == HALLWAY) {
                continue;
            }
            
            if (col != 0) {
                MapCell &left = grid[col-1][row];
                if (left.type == HALLWAY) {
                    current.type = WALL;
                }
            }
            if (row != 0) {
                MapCell &up = grid[col][row-1];
                if (up.type == HALLWAY) {
                    current.type = WALL;
                }
            }
            if (col != columns - 1) {
                MapCell &right = grid[col+1][row];
                if (right.type == HALLWAY) {
                    current.type = WALL;
                }
            }
            if (row != rows - 1) {
                MapCell &down = grid[col][row+1];
                if (down.type == HALLWAY) {
                    current.type = WALL;
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

uint Map::getColumns() const {
    return columns;
}
uint Map::getRows() const {
    return rows;
}
