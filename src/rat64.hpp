#ifndef RAT64_HPP
#define RAT64_HPP

#include "util.hpp"

template<typename T>
static inline T gcd(T a, T b) {
    // a and b must be positive.
    // Euclid's Algorithm.
    while (true) {
        T remainder = a % b;
        if (remainder == 0)
            return b;
        a = b;
        b = remainder;
    }
}

// construct with rat64::normalized(numerator, denominator)
// rather than rat64{numerator, denominator}
// unless you're sure it's already normalized,
// such as rat64{x, 1}.
struct rat64 {
    int64_t numerator;
    int64_t denominator;

    static rat64 normalized(int64_t numerator, int64_t denominator) {
        // infinity, -infinity, NaN
        if (denominator == 0) {
            numerator = sign(numerator);
            return {numerator, denominator};
        }
        // 0
        if (numerator == 0) {
            denominator = 1;
            return {numerator, denominator};
        }
        // denominator must be positive
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        // reduce the fraction
        int64_t divisor = gcd(abs(numerator), denominator);
        if (divisor != 1) {
            numerator /= divisor;
            denominator /= divisor;
        }
        return {numerator, denominator};
    }

    static constexpr rat64 nan() {
        return {0,0};
    }
};

static constexpr bool operator==(const rat64 & a, const rat64 & b) {
    return a.numerator == b.numerator && a.denominator == b.denominator;
}
static constexpr bool operator!=(const rat64 & a, const rat64 & b) {
    return !(a == b);
}

// these comparison operators work intuitively for infinities, but not for nan.
static constexpr bool operator<(const rat64 & a, const rat64 & b) {
    // TODO: watch out for overflow
    return a.numerator * b.denominator < b.numerator * a.denominator;
}
static constexpr bool operator> (const rat64 & a, const rat64 & b) { return   b < a;  }
static constexpr bool operator<=(const rat64 & a, const rat64 & b) { return !(b < a); }
static constexpr bool operator>=(const rat64 & a, const rat64 & b) { return !(a < b); }

// TODO: watch out for overflow
static inline rat64 operator+(const rat64 & a, const rat64 & b) {
    return rat64::normalized(a.numerator * b.denominator + b.numerator * a.denominator, a.denominator * b.denominator);
}
static inline rat64 operator-(const rat64 & a, const rat64 & b) {
    return rat64::normalized(a.numerator * b.denominator - b.numerator * a.denominator, a.denominator * b.denominator);
}
static inline void operator+=(rat64 & a, const rat64 & b) { a = a + b; }
static inline void operator-=(rat64 & a, const rat64 & b) { a = a - b; }

DEFINE_GDB_PY_SCRIPT("debug-scripts/rat64.py")

#endif
