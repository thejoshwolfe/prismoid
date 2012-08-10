#ifndef UTIL_H
#define UTIL_H

#include <SFML/System.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <queue>
#include <map>
#include <set>
#include <cmath>
#include <limits>
#include <tr1/memory>

typedef sf::Vector2f Vector2;
typedef sf::Vector3f Vector3;
typedef sf::FloatRect Rectangle;

// gimme the compare operators!
namespace std {
    template<>
    struct less<Vector2>
    {
        bool operator()(const Vector2& left, const Vector2& right) const
        {
            if (left.x != right.x) return left.x < right.x;
            if (left.y != right.y) return left.y < right.y;
            return false;
        }
    };
    template<>
    struct less<sf::IntRect>
    {
        bool operator()(const sf::IntRect& left, const sf::IntRect& right) const
        {
            if (left.Left   != right.Left)   return left.Left   < right.Left;
            if (left.Top    != right.Top)    return left.Top    < right.Top;
            if (left.Right  != right.Right)  return left.Right  < right.Right;
            if (left.Bottom != right.Bottom) return left.Bottom < right.Bottom;
            return false;
        }
    };
}

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

inline int toTileIndexCeilinged(float world_position, int tile_size)
{
    return std::ceil(world_position / tile_size);
}
inline sf::Vector2f toRenderPoint(Vector2 virtual_center, Vector2 point)
{
    Vector2 relative_point = point - virtual_center;
    // snap to integers to avoid gaps between sprites
    return sf::Vector2f(std::floor(relative_point.x + 0.5),
                        std::floor(relative_point.y + 0.5));
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

inline float magnitude(Vector2 vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}

inline Vector2 normalized(Vector2 vector)
{
    float _magnitude = magnitude(vector);
    if (_magnitude == 0)
        return Vector2();
    return vector / _magnitude;
}

inline void makeRectangle(std::vector<Vector2>* polygon, const Vector2 &center, const Vector2 &size)
{
    // clockwise
    polygon->push_back(Vector2(center.x - size.x / 2, center.y - size.y / 2));
    polygon->push_back(Vector2(center.x + size.x / 2, center.y - size.y / 2));
    polygon->push_back(Vector2(center.x + size.x / 2, center.y + size.y / 2));
    polygon->push_back(Vector2(center.x - size.x / 2, center.y + size.y / 2));
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

inline Vector2 scaleVector(float factor, const Vector2 &vector)
{
    Vector2 result = vector;
    result.x *= factor;
    result.y *= factor;
    return result;
}

/** rotates counterclockwise */
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