//
//  Voronoi.metal
//  Voronoi
//
//  Created by asd on 21/04/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t color;

class _Point {
public:
    _Point(){}
    _Point(int x, int y) : x(x), y(y) {}
    int x=0, y=0;
};

class Color {
public:
    Color() {}
    Color(byte r, byte g, byte b) {
        asBytes.a=0xff; asBytes.r=r; asBytes.g=g; asBytes.b=b;
    }
    Color(uint32_t c) {
        asColor=c | 0xff000000;
    }
    
    inline word _abs(uint8_t a, uint8_t b) const { return (a>b) ? (a-b) : (b-a); }
    
    inline word operator-(thread const Color& other) const {
        return (_abs(asBytes.r, other.asBytes.r) +
                _abs(asBytes.g, other.asBytes.g) +
                _abs(asBytes.b, other.asBytes.b));
    }
    union {
        struct { uint8_t r,g,b, a; } asBytes;
        uint32_t asColor = 0xffffffff; // white default init
    };
};

class Gene {
public:
    inline bool inCircle(int x, int y) const {
        x-=pos.x; y-=pos.y;
        return x*x + y*y < rad2;
    }
    
    int diameter, rad2;
    _Point pos;
    Color color;
};

// generate cellSCores per x,y, iOrg
kernel void scoreOrganisms(const device color*target[[buffer(0)]],
                        const device Gene*genes[[buffer(1)]],
                        const device int&nGenePerOrg[[buffer(2)]], // n genes in organisms -> sqare arrangement
                        device atomic_uint*scores[[buffer(3)]], // as it's updated by several threads
                        const device int&w[[buffer(4)]], //
                        
                        uint3  pos [[thread_position_in_grid]] )
{
    int x=pos.x, y=pos.y, iOrg=pos.z; // indexes (x,y, iOrg)
    
    Color topColor; // calc topColor of the organism -> set of nGene genes
    for (int nGene=nGenePerOrg, iGene=iOrg*nGene, i=0; i<nGene; i++, iGene++) {
        Gene g=genes[iGene];
        if (g.diameter==0) break; // start of fill gene -> done
        if (g.inCircle(x,y)) topColor=g.color;
    }

    int imgIndex=y*w+x; // target image index
    auto sc = Color(target[imgIndex]) - topColor; // generate point(x) diff
    atomic_fetch_add_explicit(&scores[iOrg], sc, memory_order_relaxed); // scores[iOrg] += sc;
}

kernel void scoreParent(const device color*target[[buffer(0)]],
                           const device Gene*genes[[buffer(1)]],
                           const device int&nGene[[buffer(2)]],
                           device atomic_uint&score[[buffer(3)]], // as it's updated by several threads
                           const device int&w[[buffer(4)]], //
                           
                           uint2  pos [[thread_position_in_grid]] )
{
    int x=pos.x, y=pos.y; // indexes (x,y, iOrg)
    
    Color topColor; // calc topColor of the organism -> set of nGene genes
    for (int i=0; i<nGene; i++) {
        Gene g=genes[i];
        if (g.diameter==0) break; // start of fill gene -> done
        if (g.inCircle(x,y)) topColor=g.color;
    }
    
    auto sc = Color(target[x + y*w]) - topColor; // generate point(x) diff
    atomic_fetch_add_explicit(&score, sc, memory_order_relaxed); // scores[iOrg] += sc;
}


// draw organism in imgBuffer
kernel void drawOrganism(      device color*imgBuffer[[buffer(0)]],
                         const device Gene*genes[[buffer(1)]],
                         const device int&nGene [[buffer(2)]],
                         const device int&w     [[buffer(3)]],

                         uint2  pos [[thread_position_in_grid]] ) {
    
    int x=pos.x, y=pos.y; // indexes (x,y)
    
    Color topColor; // calc topColor of the organism -> set of nGene genes
    for (int i=0; i<nGene; i++) {
        Gene g=genes[i];
        if (g.inCircle(x,y)) topColor=g.color;
    }
    imgBuffer[x + y*w]=topColor.asColor;
}


void kernel
scoreImage(device const color*bmp[[buffer(0)]],
           device const color*reference[[buffer(1)]],
           device const int &w[[buffer(2)]],
           device atomic_uint &score[[buffer(3)]],
           
           uint2  pos [[thread_position_in_grid]]
           )
{
    uint ix = pos.x + pos.y * w;
    uint sc = Color(bmp[ix]) - Color(reference[ix]);
    
    atomic_fetch_add_explicit(&score, sc, memory_order_relaxed);
}
