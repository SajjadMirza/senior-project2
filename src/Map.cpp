#include <Map.hpp>

#include <fstream>
#include <cassert>

#include <resources.hpp>
#include <log.hpp>


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
    vec3 light_pos;
    vec3 up_pos(0, 0, 0.40);
    vec3 left_pos(-.60, 0, 1);
    vec3 right_pos(.60, 0, 1);
    vec3 down_pos(0, 0, 1.60);
    for (int col = 0; col < columns; col++) {
        for (int row = 0; row < rows; row++) {
            MapCell &current = grid[col][row];
            light_pos(0) = col;
            light_pos(1) = 0.02;
            light_pos(2) = row;
            if (current.type != EMPTY) {
                continue;
            }
            
            if (col != 0) {
                MapCell &left = grid[col-1][row];
                if (left.type != EMPTY && left.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                    tiny_light_positions.push_back(light_pos + left_pos);
                }
            }
            if (row != 0) {
                MapCell &up = grid[col][row-1];
                if (up.type != EMPTY && up.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                    tiny_light_positions.push_back(light_pos + up_pos);
                    if (row == 49 || row == 50) {
                        LOG("found upward floor at (" << col << ", " << row << ") "
                            << up.type);
                    }
                }
            }
            if (col != columns - 1) {
                MapCell &right = grid[col+1][row];
                if (right.type != EMPTY && right.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                    tiny_light_positions.push_back(light_pos + right_pos);
                }
            }
            if (row != rows - 1) {
                MapCell &down = grid[col][row+1];
                if (down.type != EMPTY && down.type != WALL) {
                    current.type = WALL;
                    current.name = "WALL";
                    tiny_light_positions.push_back(light_pos + down_pos);
                }
            }
        }
    }

    LOG("tiny lights: " << tiny_light_positions.size());
}

static const uint32_t RED_SHIFT    = 16;
static const uint32_t GREEN_SHIFT  = 8;
static const uint32_t BLUE_SHIFT   = 0;

static const uint32_t COLOR_ROOM_FLOOR   = 0x0000FF;
static const uint32_t COLOR_BIG_LIGHT    = 0xFFFF00;
static const uint32_t COLOR_SMALL_LIGHT  = 0x00FFFF;
static const uint32_t COLOR_BOSS_FLOOR   = 0xFF0000;
static const uint32_t COLOR_START_POS    = 0x00FF00;
static const uint32_t COLOR_CORRIDOR     = 0x808080;
static const uint32_t COLOR_EMPTY        = 0xFFFFFF;
static const uint32_t COLOR_NODRAW_FLOOR = 0x000000;

int Map::loadMapFromImage(const char *filename)
{
    FIBITMAP *img = resource::GenericLoader(filename, 0);
    uint bpp = FreeImage_GetBPP(img);
//    BYTE *data = FreeImage_GetBits(img);

    uint width = FreeImage_GetWidth(img);
    uint height = FreeImage_GetHeight(img);
    uint pitch = FreeImage_GetPitch(img);
    uint line = FreeImage_GetLine(img); // Line means "width measured in bytes"
    uint bytespp = line / width;

    FREE_IMAGE_TYPE imgtype = FreeImage_GetImageType(img);

    if (bpp < 24) {
        ERROR("Map file bit depth < 24: " << bpp);
        exit(-1);
    }

    if (imgtype != FIT_BITMAP) {
        ERROR("Map file is not bitmap!");
        exit(-1);
    }

    vec3 base_light_pos(0, 1.0, 0);
    
    
    
    for (uint j = 0; j < height; j++) {
        BYTE *bits = FreeImage_GetScanLine(img, j);
        for (uint i = 0; i < width; i++) {
            uint32_t pixel = 0;
            pixel |= (bits[FI_RGBA_RED] << RED_SHIFT);
            pixel |= (bits[FI_RGBA_GREEN] << GREEN_SHIFT);
            pixel |= (bits[FI_RGBA_BLUE] << BLUE_SHIFT);
            bits += bytespp;

            uint x = i;
            uint y = (height - j) - 1;

            switch (pixel) {
            case COLOR_EMPTY: // Nothing to do
                break;
            case COLOR_ROOM_FLOOR:
                grid[x][y].type = HALLWAY;
                grid[x][y].name = "HALLWAY";
                break;
            case COLOR_BIG_LIGHT:
                grid[x][y].type = HALLWAY;
                grid[x][y].name = "HALLWAY";
                major_light_positions.push_back(base_light_pos + vec3(x, 0, y+1));
                break;
            case COLOR_SMALL_LIGHT:
                grid[x][y].type = HALLWAY;
                grid[x][y].name = "HALLWAY";
                minor_light_positions.push_back(base_light_pos + vec3(x, 0, y+1));
                break;
            case COLOR_BOSS_FLOOR:
                grid[x][y].type = NOCEIL;
                grid[x][y].name = "NOCEIL";
                break;
            case COLOR_START_POS:
                grid[x][y].type = HALLWAY;
                grid[x][y].name = "HALLWAY";
                player_start = vec3(x, 0.0f, y);
                break;
            case COLOR_CORRIDOR:
                grid[x][y].type = HALLWAY;
                grid[x][y].name = "HALLWAY";
                break;
            case COLOR_NODRAW_FLOOR:
                grid[x][y].type = NODRAW;
                grid[x][y].name = "NODRAW";
                break;
            default:
                ERROR("Pixel value not from accepted set (" << x << ", " <<  y << "): " 
                      << std::hex << pixel);
                assert(0 && __FILE__ && __LINE__);
            }
        }
    }

    initWalls();
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

const std::vector<Eigen::Vector3f>& Map::getMajorLightPositions() const
{
    return major_light_positions;
}

const std::vector<Eigen::Vector3f>& Map::getMinorLightPositions() const
{
    return minor_light_positions;
}

const std::vector<Eigen::Vector3f>& Map::getTinyLightPositions() const
{
    return tiny_light_positions;
}

const Eigen::Vector3f& Map::getPlayerStart() const
{
    return player_start;
}
