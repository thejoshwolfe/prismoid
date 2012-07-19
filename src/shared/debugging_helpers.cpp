#include "debugging_helpers.h"

std::string s(const std::stringstream & ss)
{
    return ss.str();
}

std::string s(const Vector2 & v)
{
    std::stringstream ss;
    ss << "<" << v.x << "," << v.y << ">";
    return s(ss);
}

std::string s(const Collision & collision)
{
    std::stringstream ss;
    ss << "(";
    ss << (collision.vertex_entity == collision.moving_entity ? "M" : "S") << "." << collision.vertex_index;
    ss << "(" << s(collision.vertex_entity->bounding_prismoid.getCapVertex(collision.vertex_index)) << ")";
    ss << ", " << (collision.edge_entity == collision.moving_entity ? "M" : "S") << "." << collision.edge_index;
    ss << "(" << s(collision.edge_entity->bounding_prismoid.getCapVertex(collision.edge_index)) << ",";
    ss << s(collision.edge_entity->bounding_prismoid.getCapVertex((collision.edge_index + 1) % collision.edge_entity->bounding_prismoid.size())) << ")";
    ss << ", " << s(collision.getNormal());
    ss << ")";
    return s(ss);
}
