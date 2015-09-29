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

class rat64 {
public:
    int64_t numerator;
    int64_t denominator;
    rat64() {}
    rat64(int64_t numerator, int64_t denominator) :
        numerator(numerator), denominator(denominator) {
        normalize();
    }

private:
    void normalize() {
        // infinity, -infinity, NaN
        if (denominator == 0) { numerator = sign(numerator); return; }
        // 0
        if (numerator == 0) { denominator = 1; return; }
        // denominator must be positive
        if (denominator < 0) { numerator = -numerator; denominator = -denominator; }
        // reduce the fraction
        int64_t divisor = gcd(abs(numerator), denominator);
        if (divisor != 1) {
            numerator /= divisor;
            denominator /= divisor;
        }
    }
};

static constexpr bool operator==(const rat64 & a, const rat64 & b) {
    return a.numerator == b.numerator && a.denominator == b.denominator;
}
static constexpr bool operator!=(const rat64 & a, const rat64 & b) {
    return !(a == b);
}

static constexpr bool operator<(const rat64 & a, const rat64 & b) {
    // TODO: watch out for overflow
    return a.numerator * b.denominator < b.numerator * a.denominator;
}

DEFINE_GDB_PY_SCRIPT("debug-scripts/rat64.py")

#endif
