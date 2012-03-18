// see: https://github.com/bjorn/tiled/wiki/TMX-Map-Format
#include "TiledTmx.h"

#include "Util.h"
#include "pugixml.hpp"
#include "base64.h"
#include "zlib_wrapper.h"

#include <iostream>
#include <cstdlib>

const unsigned TiledTmx::FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned TiledTmx::FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned TiledTmx::FLIPPED_DIAGONALLY_FLAG = 0x20000000;

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
    for (pugi::xml_node node = root.first_child(); !node.empty(); node = node.next_sibling()) {
        std::string name = node.name();
        if (name == "tileset") {
            // TODO
        } else if (name == "objectgroup") {
            // TODO
        } else if (name == "layer") {
            {
                int layer_width = std::atoi(node.attribute("width").value());
                int layer_height = std::atoi(node.attribute("height").value());
                assertEquals(layer_width, map_width);
                assertEquals(layer_height, map_height);
            }
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
                    unsigned global_tile_id = 0;
                    global_tile_id |= data[tile_index + 0] << 0;
                    global_tile_id |= data[tile_index + 1] << 8;
                    global_tile_id |= data[tile_index + 2] << 16;
                    global_tile_id |= data[tile_index + 3] << 24;
                    tile_index += 4;

                    // flips are not supported
                    assertEquals<unsigned int>(global_tile_id & FLIPPED_HORIZONTALLY_FLAG, 0);
                    assertEquals<unsigned int>(global_tile_id & FLIPPED_VERTICALLY_FLAG, 0);
                    assertEquals<unsigned int>(global_tile_id & FLIPPED_DIAGONALLY_FLAG, 0);
                    global_tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

                    setTile(x, y, global_tile_id);
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
