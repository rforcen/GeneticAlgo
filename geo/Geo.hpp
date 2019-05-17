//
//  Geo.hpp
//  GeneticAlgos
//
//  Created by asd on 16/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef Geo_hpp
#define Geo_hpp

#include <stdlib.h>
#include <vector>

using std::vector;

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t color;


class point {
public:
    point(){}
    point(int x, int y) : x(x), y(y) {}
    
    bool operator == (const point &other) {
        return x==other.x && y==other.y;
    }
    int x,y;
};

class Color {
public:
    Color() {}
    Color(byte r, byte g, byte b) {
        asBytes.a=0xff; asBytes.r=r; asBytes.g=g; asBytes.b=b;
    }
    Color(color c) {
        asColor=c | 0xff000000;
    }
    
    inline word _abs(byte a, byte b) const { return (a>b) ? (a-b) : (b-a); }
    
    inline word operator-(const Color& other) const {
        return (_abs(asBytes.r, other.asBytes.r) +
                _abs(asBytes.g, other.asBytes.g) +
                _abs(asBytes.b, other.asBytes.b));
    }
    union {
        struct { byte r,g,b, a; } asBytes;
        color asColor = 0xffffffff; // white default init
    };
};


#endif /* Geo_hpp */
