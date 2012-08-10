#ifndef RENDERABLESTATICENTITY_H
#define RENDERABLESTATICENTITY_H

#include "StaticEntity.h"
#include "Renderable.h"

class RenderableStaticEntity : public StaticEntity, public Renderable
{
private:
    sf::Sprite sprite;
public:
    RenderableStaticEntity(const Vector2 & center, const Vector2 & size, const sf::Image &image,
            bool flip_horizontal, bool flip_vertical, bool flip_diagonally) :
        StaticEntity(center, size)
    {
        bounding_prismoid.setZ(0, 1);
        std::vector<Vector2> here;
        Util::makeRectangle(&here, center, size);
        for (int i = 0; i < (int)here.size(); i++)
            bounding_prismoid.addEdge(here[i], here[i]);

        sprite.SetImage(image);
        sprite.SetCenter(image.GetWidth() / 2, image.GetHeight() / 2);
        if (flip_diagonally) {
            sprite.SetRotation(-90);
            flip_horizontal = !flip_horizontal;
        }
        sprite.FlipX(flip_horizontal);
        sprite.FlipY(flip_vertical);
    }
    void render(Vector2 virtual_center, sf::RenderTarget * render_target);
};

#endif
