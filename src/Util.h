#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <limits>

#include <gmpxx.h>
typedef mpz_class bigint;
typedef mpq_class bigfraction;
typedef sf::Vector2<bigint> Vector2;
typedef sf::Vector2<bigfraction> Vector2q;
typedef sf::Vector3<bigfraction> Vector3;
typedef sf::Rect<bigint> Rectangle;

typedef unsigned char byte;
typedef int32_t int32;
typedef int64_t int64;

struct Edge
{
    Vector3 points[2];
    Edge() {}
    Edge(const Vector3 &point1, const Vector3 &point2)
    {
        points[0] = point1;
        points[1] = point2;
    }
private:
    Edge(const Edge &) {} // non-copyable
};

namespace Util {

inline int toTileIndexFloored(bigint world_position, int tile_size)
{
    bigint value = world_position / tile_size;
    return value.get_si();
}
inline int toTileIndexCeilinged(bigint world_position, int tile_size)
{
    return toTileIndexFloored(world_position + tile_size - 1, tile_size);
}
inline sf::Vector2f toRenderPoint(Vector2 virtual_center, Vector2 point)
{
    Vector2 relative_point = point - virtual_center;
    return sf::Vector2f(relative_point.x.get_d(), relative_point.y.get_d());
}

inline float euclideanMod(float numerator, float denominator)
{
    float result = std::fmod(numerator, denominator);
    if (result < 0)
        result += denominator;
    return result;
}

inline int euclideanMod(int numerator, int denominator)
{
    return (numerator % denominator + denominator) % denominator;
}

inline bigint magnitude(Vector2 vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}

inline Vector2q normalized(Vector2 vector)
{
    bigint _magnitude = magnitude(vector);
    if (_magnitude == 0)
        return Vector2q();
    return Vector2q(vector.x / _magnitude,
                    vector.y / _magnitude);
}
template<typename To, typename From>
sf::Vector2<To> convertVector(sf::Vector2<From> vector)
{
    return sf::Vector2<To>(vector.x, vector.y);
}

template<typename T>
T dot(sf::Vector2<T> a, sf::Vector2<T> b)
{
    return a.x * b.x + a.y * b.y;
}
template<typename T>
T dot(sf::Vector3<T> a, sf::Vector3<T> b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
sf::Vector3<T> cross(const sf::Vector3<T> &a, const sf::Vector3<T> &b)
{
    return sf::Vector3<T>(a.y * b.z - a.z * b.y,
                          a.z * b.x - a.x * b.z,
                          a.x * b.y - a.y * b.x);
}

inline Vector2 scaleVector(bigfraction factor, const Vector2 &vector)
{
    Vector2 result = vector;
    result.x *= factor;
    result.y *= factor;
    return result;
}

inline bool getEdgeIntersectionWithQuadrilateral(const Edge &edge, const Edge &plane_edge1, const Edge &plane_edge2, Vector3 *output_intersection_point)
{
    // http://en.wikipedia.org/wiki/Line-plane_intersection
    Vector3 edge_point = edge.points[0];
    Vector3 edge_vector = edge.points[1] - edge_point;
    Vector3 plane_point = plane_edge1.points[0];
    Vector3 plane_vector = cross(plane_edge1.points[1] - plane_point, plane_edge2.points[0] - plane_point);
    bigfraction numerator = dot(plane_point - edge_point, plane_vector);
    Vector3 intersection_point;
    if (numerator == 0) {
        // intersects immediately
        intersection_point = edge_point;
    } else {
        bigfraction denominator = dot(edge_vector, plane_vector);
        if (denominator == 0) {
            // parallel and not intersecting
            return false;
        }
        bigfraction percent_to_intersection = numerator / denominator;
        // check the bounds of the edge
        if (!(0 <= percent_to_intersection && percent_to_intersection <= 1))
            return false;
        intersection_point = edge_point + percent_to_intersection * edge_vector;
    }

    // check the bounds of the face
    Vector3 plane_points[] = {
            plane_edge1.points[0],
            plane_edge1.points[1],
            plane_edge2.points[1],
            plane_edge2.points[0],
    };
    const int plane_points_count = sizeof(plane_points) / sizeof(Vector3);
    for (int i = 0; i < plane_points_count; i++) {
        Vector3 edge1 = plane_points[(i + 3) % plane_points_count] - plane_points[i];
        Vector3 edge2 = plane_points[(i + 1) % plane_points_count] - plane_points[i];
        Vector3 inward_direction = cross(edge2, cross(edge1, edge2));
        bool is_inside = dot(inward_direction, intersection_point - plane_points[i]) >= 0;
        if (!is_inside)
            return false; // miss
    }

    // hit
    *output_intersection_point = intersection_point;
    return true;
}

template<typename T>
sf::Vector2<T> perp(sf::Vector2<T> vector)
{
    return sf::Vector2<T>(vector.y, -vector.x);
}

template<typename K, typename V>
struct KeyAndValue
{
    K key;
    V value;
    KeyAndValue() {}
    KeyAndValue(K key, V value) : key(key), value(value) {}
    bool operator <(const KeyAndValue<K, V>& other) const
    {
        // surprise! we want priority queues to give us the smallest one
        return key > other.key;
    }
};

template<typename K, typename V>
void push(std::priority_queue<KeyAndValue<K, V> > *queue, K key, V value)
{
    queue->push(KeyAndValue<K, V>(key, value));
}
template<typename T>
void clear(std::priority_queue<T> * queue)
{
    if (!queue->empty())
        *queue = std::priority_queue<T>();
}
template<typename K, typename V>
void insert(std::multimap<K, V> *map, K key, V value)
{
    map->insert(std::pair<K, V>(key, value));
}

template<typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}
template<typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}

inline void assert(bool condition, std::string message)
{
    if (!condition)
        throw message;
}

// source for trim functions: http://stackoverflow.com/a/217605/367916
static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}
static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}
static inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

}

#endif
