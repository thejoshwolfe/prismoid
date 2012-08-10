#ifndef DEBUGGING_HELPERS_H_
#define DEBUGGING_HELPERS_H_

#include "Physics.h"

#include <sstream>

// Let's get lots of overloads for s(something) so that we can use them for debugging

std::string s(const std::string & string);
std::string s(const std::stringstream & ss);
std::string s(const Vector2 & v);
std::string s(const Collision & collision);

template<typename T>
std::string s(T value)
{
    // this catches primitive types and stuff
    std::stringstream ss;
    ss << value;
    return s(ss);
}

template<typename T>
std::string s(std::tr1::shared_ptr<T> shared_pointer)
{
    return s(*shared_pointer);
}

template<typename T, int N>
std::string s(T(& array)[N])
{
    std::stringstream ss;
    ss << "[" << std::endl;
    for (int i = 0; i < N; i++)
        ss << "    " << s(array[i]) << std::endl;
    ss << "]";
    return s(ss);
}

template<typename T>
std::string s(std::vector<T> vector)
{
    std::stringstream ss;
    ss << "[" << std::endl;
    for (int i = 0; i < (int)vector.size(); i++)
        ss << "    " << s(vector[i]) << std::endl;
    ss << "]";
    return s(ss);
}

template<typename K, typename V>
std::string s(std::multimap<K, V> map)
{
    std::stringstream ss;
    ss << "{" << std::endl;
    for (typename std::multimap<K, V>::iterator iterator = map.begin(); iterator != map.end(); iterator++)
        ss << "    " << s(iterator->first) << ": " << s(iterator->second) << std::endl;
    ss << "}";
    return s(ss);
}

template<typename T>
std::string s(std::set<T> set)
{
    std::stringstream ss;
    ss << "{" << std::endl;
    for (typename std::set<T>::iterator iterator = set.begin(); iterator != set.end(); iterator++)
        ss << "    " << s(*iterator) << std::endl;
    ss << "}";
    return s(ss);
}

template<typename T1, typename T2>
std::string s(std::pair<T1, T2> pair)
{
    std::stringstream ss;
    ss << "<" << pair.first << "," << pair.second << ">";
    return s(ss);
}

#endif /* DEBUGGING_HELPERS_H_ */
