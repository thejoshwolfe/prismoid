#ifndef GAME_H
#define GAME_H

#include "Physics.h"
#include "PlayerEntity.h"
#include "TiledTmx.h"
#include "RenderableStaticEntity.h"

#include <tr1/memory>

class Game : public StaticEntityProvider
{
public:
    Game(std::string filename);

    void doFrame(const sf::Input * input);
    void render(sf::RenderTarget * render_target);

    const sf::Input * getInput() { return input; }

    void getStaticEntities(std::vector<StaticEntity *> &static_entities, const Rectangle &bounding_rectangle);

private:
    sf::Image tileset_image;
    std::map<sf::IntRect, sf::Image*> tileset_images;
    int tile_size;
    int layer_width;
    int layer_height;
    RenderableStaticEntity ** physics_layer;
    Physics physics;
    PlayerEntity * player;

    // temporary
    const sf::Input * input;
};

#endif // GAME_H
