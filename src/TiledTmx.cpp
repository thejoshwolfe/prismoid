// see: https://github.com/bjorn/tiled/wiki/TMX-Map-Format
#include "TiledTmx.h"

#include "Util.h"
#include "pugixml.hpp"
#include "base64.h"
#include "zlib_wrapper.h"

#include <iostream>
#include <cstdlib>

const unsigned Tile::FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned Tile::FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned Tile::FLIPPED_DIAGONALLY_FLAG = 0x20000000;

TiledTmx * TiledTmx::load(std::string filename)
{
    TiledTmx * result = new TiledTmx(filename);
    result->internalLoad();
    return result;
}

void TiledTmx::internalLoad()
{
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(filename.c_str());
    if (!result) {
        std::cerr << "ERROR: " << filename << ":+" << result.offset << ": " << result.description() << std::endl;
        std::exit(1);
    }
    pugi::xml_node root = document.document_element();
    assertEquals<std::string> (root.name(), "map");
    assertEquals<std::string> (root.attribute("version").value(), "1.0");
    assertEquals<std::string> (root.attribute("orientation").value(), "orthogonal");
    map_width = std::atoi(root.attribute("width").value());
    map_height = std::atoi(root.attribute("height").value());
    tile_size = std::atoi(root.attribute("tilewidth").value());
    assertEquals(std::atoi(root.attribute("tileheight").value()), tile_size);
    for (pugi::xml_node node = root.first_child(); !node.empty(); node = node.next_sibling()) {
        std::string name = node.name();

        if (name == "tileset") {
            assertEquals(std::atoi(node.attribute("firstgid").value()), 1);
            assertEquals(std::atoi(node.attribute("tilewidth").value()), tile_size);
            assertEquals(std::atoi(node.attribute("tileheight").value()), tile_size);

            pugi::xml_node image_node = node.child("image");
            int image_width = std::atoi(image_node.attribute("width").value());
            // if these assertions fail, they'll look really weird
            assertEquals(image_width != 0, true);
            assertEquals(image_width % tile_size, 0);
            tileset_width = image_width / tile_size;
            std::string relative_source = image_node.attribute("source").value();
            int index = Util::max<int>(filename.rfind('/'), filename.rfind('\\'));
            tileset_image_filename = filename.substr(0, index + 1) + relative_source;

        } else if (name == "objectgroup") {
            // TODO

        } else if (name == "layer") {
            assertEquals(std::atoi(node.attribute("width").value()), map_width);
            assertEquals(std::atoi(node.attribute("height").value()), map_height);
            layer = new int[map_width * map_height];

            pugi::xml_node data_node = node.child("data");
            assertEquals<std::string> (data_node.attribute("encoding").value(), "base64");
            assertEquals<std::string> (data_node.attribute("compression").value(), "zlib");

            std::string data = data_node.first_child().value();
            data = base64_decode(Util::trim(data));
            data = decompress_string(data);

            assertEquals((int)data.size(), map_width * map_height * 4);
            int tile_index = 0;
            for (int y = 0; y < map_height; y++) {
                for (int x = 0; x < map_width; x++) {
                    unsigned tile_value = 0;
                    tile_value |= data[tile_index + 0] << 0;
                    tile_value |= data[tile_index + 1] << 8;
                    tile_value |= data[tile_index + 2] << 16;
                    tile_value |= data[tile_index + 3] << 24;
                    tile_index += 4;

                    setTile(x, y, tile_value);
                }
            }
        } else
            assertFailBogusString(name);
    }
}

template<typename T>
void TiledTmx::assertEquals(T value, T expected)
{
    if (value == expected)
        return;
    std::cerr << "ERROR: " << filename << ": expected \"" << expected << "\" but got \"" << value << "\"" << std::endl;
    std::exit(1);
}
void TiledTmx::assertFailBogusString(std::string bogusValue)
{
    std::cerr << "ERROR: " << filename << ": bogus string \"" << bogusValue << "\"" << std::endl;
    std::exit(1);
}
