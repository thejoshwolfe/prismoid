#ifndef TILEDTMX_H_
#define TILEDTMX_H_

#include <string>

class TiledTmx
{
public:
    static TiledTmx * load(std::string filename);

    int width()
    {
        return map_width;
    }
    int height()
    {
        return map_height;
    }
    int getTile(int x, int y)
    {
        return layer[y * map_height + x];
    }

private:
    static const unsigned FLIPPED_HORIZONTALLY_FLAG;
    static const unsigned FLIPPED_VERTICALLY_FLAG;
    static const unsigned FLIPPED_DIAGONALLY_FLAG;

    const std::string filename;
    int map_width, map_height;
    int* layer; // only 1 layer for now
    TiledTmx(std::string filename) :
        filename(filename)
    {
    }
    void internalLoad();
    template<typename T> void assertEquals(T value, T expected);
    void assertFailBogusString(std::string bogusValue);

    void setTile(int x, int y, int id)
    {
        layer[y * map_width + x] = id;
    }
};

#endif
