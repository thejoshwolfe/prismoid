#ifndef TILEDTMX_H_
#define TILEDTMX_H_

#include <SFML/Graphics/Rect.hpp>
#include <string>

struct Tile
{
    const unsigned value;
    explicit Tile(unsigned value) : value(value) {}

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
    Tile tile(int x, int y) { return Tile(layer[y * map_height + x]); }
    std::string tilesetImageFilename() { return tileset_image_filename; }
    sf::IntRect tilesetImageOffset(int global_id)
    {
        int local_id = global_id - 1;
        int row = local_id / tileset_width;
        int column = local_id % tileset_width;
        return sf::IntRect(column * tile_size, row * tile_size, (column + 1) * tile_size, (row + 1) * tile_size);
    }

private:
    const std::string filename;
    int map_width, map_height;
    std::string tileset_image_filename;
    int tileset_width;
    int tile_size;
    int* layer; // only 1 layer for now
    TiledTmx(std::string filename) : filename(filename) {}
    void internalLoad();
    template<typename T> void assertEquals(T value, T expected);
    void assertFailBogusString(std::string bogusValue);

    void setTile(int x, int y, int id) { layer[y * map_width + x] = id; }
};

#endif
