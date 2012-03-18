#include "StaticEntity.h"

const Vector2 StaticEntity::zero(0, 0);

void StaticEntity::render(Vector2 virtual_center, sf::RenderTarget * render_target)
{
    sf::Vector2f render_center = Util::toRenderPoint(virtual_center, center);
    sprite.SetPosition(render_center);
    render_target->Draw(sprite);
}
