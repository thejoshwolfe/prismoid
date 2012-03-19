#ifndef PRISMOID_H
#define PRISMOID_H

#include "Util.h"

// NOTE: this class does not prevent the "prismoid"-definition violation of
// non-parallel corresponding edges in bases, therefore forming skew quadrilateral
// faces representing hyperbolic paraboloids.
// Perhaps some assertions should be implanted to ensure clients don't do this.

class Prismoid
{
private:
    bigfraction z[2];
    std::vector<Vector2> bases[2];
public:
    void setZ(bigfraction z1, bigfraction z2)
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
    void addEdge(const Vector2 &point1, const Vector2 &point2)
    {
        bases[0].push_back(point1);
        bases[1].push_back(point2);
    }
    void getEdge(int i, Edge * edge) const
    {
        *edge = Edge(Vector3(bases[0][i].x, bases[0][i].y, z[0]),
                     Vector3(bases[1][i].x, bases[1][i].y, z[1]));
    }
    void getFace(int i, Edge * edge1, Edge * edge2) const
    {
        getEdge(i, edge1);
        getEdge((i + 1) % size(), edge2);
    }
    Vector2 getNormal(int i)
    {
        // assumes clockwise, or something
        return Util::perp(bases[0][(i + 1) % size()] - bases[0][i]);
    }
};

#endif // PRISMOID_H
