#ifndef PRISMOID_H
#define PRISMOID_H

#include "Util.h"

// For discussion purposes, let the quadrilateral [A,B,C,D] be one cap of this prismoid.
// (Note that the caps can be any n-gons, not just quadrilaterals).
// The other cap shall be [A1,B1,C1,D1].

// Note: this class does not prevent the "prismoid"-definition violation of
// non-parallel corresponding edges in bases, such as [A,B] not parallel to [A1,B1]
// thereby forming skew quadrilateral faces representing hyperbolic paraboloids.

class Prismoid
{
private:
    float z[2];
    std::vector<Vector2> bases[2];
public:
    void setZ(float z1, float z2)
    {
        z[0] = z1;
        z[1] = z2;
    }
    /** the n of the n-gon caps */
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
    /** such as [A,A1] */
    void getEdge(int i, Edge * edge) const
    {
        *edge = Edge(Vector3(bases[0][i].x, bases[0][i].y, z[0]),
                     Vector3(bases[1][i].x, bases[1][i].y, z[1]));
    }
    /** such as [A,A1],[B,B1] */
    void getFace(int i, Edge * edge1, Edge * edge2) const
    {
        getEdge(i, edge1);
        getEdge((i + 1) % size(), edge2);
    }
    /** such as normal to [A,B] */
    Vector2 getNormal(int i)
    {
        // assumes clockwise, or something
        return Util::perp(bases[0][(i + 1) % size()] - bases[0][i]);
    }
    /** such as B-A, D-A */
    void getAdjacentCapEdgeVectors(int i, Vector2 * adjacent_edge1, Vector2 * adjacent_edge2)
    {
        *adjacent_edge1 = bases[0][(i + 1) % size()] - bases[0][i];
        *adjacent_edge2 = bases[0][(i - 1) % size()] - bases[0][i];
    }
};

#endif // PRISMOID_H
