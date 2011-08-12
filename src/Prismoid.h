#ifndef PRISMOID_H
#define PRISMOID_H

#include <SFML/Graphics.hpp>

struct Edge
{
    sf::Vector3f points[2];
    Edge() {}
    Edge(const sf::Vector3f &point1, const sf::Vector3f &point2)
    {
        points[0] = point1;
        points[1] = point2;
    }
private:
    Edge(const Edge &) {}; // non-copyable
};

// NOTE: this class does not prevent the "prismoid"-definition violation of
// non-parallel corresponding edges in bases, therefore forming skew quadrilateral
// faces representing hyperbolic paraboloids.
// Perhaps some assertions should be implanted to ensure clients don't do this.

class Prismoid
{
private:
    float z[2];
    std::vector<sf::Vector2f> bases[2];
public:
    void setZ(float z1, float z2)
    {
        z[0] = z1;
        z[1] = z2;
    }
    int size() const { return (int)bases[0].size(); }
    void clearEdges()
    {
        bases[0].clear();
        bases[1].clear();
    }
    void addEdge(const sf::Vector2f &point1, const sf::Vector2f &point2)
    {
        bases[0].push_back(point1);
        bases[1].push_back(point2);
    }
    void getEdge(int i, Edge * edge) const
    {
        *edge = Edge(sf::Vector3f(bases[0][i].x, bases[0][i].y, z[0]),
                     sf::Vector3f(bases[1][i].x, bases[1][i].y, z[1]));
    }
    void getFace(int i, Edge * edge1, Edge * edge2) const
    {
        getEdge(i, edge1);
        getEdge((i + 1) % size(), edge2);
    }
    sf::Vector2f getNormal(int i);

};

#endif // PRISMOID_H
