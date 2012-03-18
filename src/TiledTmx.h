#ifndef TILEDTMX_H_
#define TILEDTMX_H_

#include <string>

struct Tile
{
    const unsigned value;
    Tile(unsigned value) : value(value) {}

    bool is_flipped_horizontally() { return value & FLIPPED_HORIZONTALLY_FLAG; }
    bool is_flipped_vertically() { return value & FLIPPED_VERTICALLY_FLAG; }
    bool is_flipped_diagonally() { return value & FLIPPED_DIAGONALLY_FLAG; }
    int global_id() { return value & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG); }

private:
    static const unsigned FLIPPED_HORIZONTALLY_FLAG;
    static const unsigned FLIPPED_VERTICALLY_FLAG;
    static const unsigned FLIPPED_DIAGONALLY_FLAG;
};

class TiledTmx
{
public:
    static TiledTmx * load(std::string filename);

    int width() { return map_width; }
    int height() { return map_height; }
    int tileSize() { return tile_size; }
    Tile getTile(int x, int y) { return Tile(layer[y * map_height + x]); }

private:
    const std::string filename;
    int map_width, map_height;
    int tile_size;
    int* layer; // only 1 layer for now
    TiledTmx(std::string filename) : filename(filename) {}
    void internalLoad();
    template<typename T> void assertEquals(T value, T expected);
    void assertFailBogusString(std::string bogusValue);

    void setTile(int x, int y, int id)
    {
        layer[y * map_width + x] = id;
    }
};

#endif
