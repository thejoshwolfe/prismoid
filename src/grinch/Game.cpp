#include "Game.h"

Game::Game(std::string filename)
    : physics(this)
{
    // moving_entities.push_back(new PlayerEntity(Vector2(100, -50), Vector2(30, 84), Vector2(0, 0)));
    player = new PlayerEntity(Vector2(32 + 16, 16), Vector2(32, 32), Vector2(0, -1));
    physics.moving_entities.push_back(player);

    TiledTmx * map = TiledTmx::load(filename);
    Util::assert(tileset_image.LoadFromFile(map->tilesetImageFilename()), "image load");
    tile_size = map->tileSize();
    layer_width = map->width();
    layer_height = map->height();
    physics_layer = new RenderableStaticEntity*[layer_width * layer_height];
    for (int y = 0; y < layer_height; y++) {
        for (int x = 0; x < layer_width; x++) {
            Tile tile = map->tile(x, y);
            RenderableStaticEntity * entity = NULL;
            if (tile.global_id() != 0) {
                // we have to create subimages manually. see https://github.com/thejoshwolfe/grinch/issues/2
                sf::IntRect sub_rect = map->tilesetImageOffset(tile.global_id());
                sf::Image* image;
                if (tileset_images.count(sub_rect)) {
                    image = tileset_images[sub_rect];
                } else {
                    image = new sf::Image(sub_rect.GetWidth(), sub_rect.GetHeight());
                    image->Copy(tileset_image, 0, 0, sub_rect);
                    tileset_images[sub_rect] = image;
                }
                entity = new RenderableStaticEntity(Vector2(x * tile_size + tile_size / 2, y * tile_size + tile_size / 2), Vector2(tile_size, tile_size), *image, tile.is_flipped_horizontally(),
                        tile.is_flipped_vertically(), tile.is_flipped_diagonally());
            }
            physics_layer[y * layer_width + x] = entity;
        }
    }
    delete map;
}

void Game::doFrame(const sf::Input * input)
{
    // run controllers
    this->input = input;
    player->doController(this);

    physics.doFrame();
}

void Game::render(sf::RenderTarget *render_target)
{
    render_target->GetDefaultView().SetCenter(0, 0);
    Vector2 virtual_center = player->center;

    float width_apothem = render_target->GetWidth() / 2;
    float height_apothem = render_target->GetHeight() / 2;
    Rectangle bounding_rectangle( //
            virtual_center.x - width_apothem, virtual_center.y - height_apothem, //
            virtual_center.x + width_apothem, virtual_center.y + height_apothem);
    std::vector<StaticEntity *> static_entities;
    getStaticEntities(static_entities, bounding_rectangle);
    for (int i = 0; i < (int)static_entities.size(); i++)
        dynamic_cast<Renderable*>(static_entities[i])->render(virtual_center, render_target);
    for (int i = 0; i < (int)physics.moving_entities.size(); i++)
        dynamic_cast<Renderable*>(physics.moving_entities[i])->render(virtual_center, render_target);
}

void Game::getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle)
{
    // when considering the exact edge between two tiles, grab both tiles.
    // for example, if the bounding rectangle is [0,0,0,0],
    // return all 4 tiles that touch that point: [-1,-1]->[0,0]
    int min_x = Util::toTileIndexCeilinged(bounding_rectangle.Left, tile_size) - 1;
    int min_y = Util::toTileIndexCeilinged(bounding_rectangle.Top, tile_size) - 1;
    int max_x = Util::toTileIndexCeilinged(bounding_rectangle.Right, tile_size);
    int max_y = Util::toTileIndexCeilinged(bounding_rectangle.Bottom, tile_size);

    min_x = Util::max(min_x, 0);
    min_y = Util::max(min_y, 0);
    max_x = Util::min(max_x, layer_width - 1);
    max_y = Util::min(max_y, layer_height - 1);

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            StaticEntity * entity = physics_layer[y * layer_width + x];
            if (entity != NULL)
                static_entities.push_back(entity);
        }
    }
}
