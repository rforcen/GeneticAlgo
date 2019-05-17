//
//  PolygonDrawer.metal
//  GeneticAlgos
//
//  Created by asd on 15/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

constant int maxPoints=200;
typedef uint32_t color; // ABGR, i.e. red=0xff0000ff
typedef uint8_t byte;
typedef uint16_t word;

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

class point {
public:
    point(){}
    point(int x, int y) : x(x), y(y){}
    int x=0,y=0;
};


class PolygonDrawer { // http://alienryderflex.com/polygon_fill/
    
public:
    static bool inSide(device const point *points, int nPoints, int x, int y, int w, int h) {
        
        //  calc. number of intersections
        int nIntersections=0, j=nPoints-1;
        for (int i=0; i<nPoints; i++) {
            if ( (points[i].y < y && points[j].y >= y) ||
                (points[j].y < y && points[i].y >= y) ) {
                auto nx=(points[i].x + (float)(y-points[i].y) / (points[j].y-points[i].y)
                         * (points[j].x - points[i].x));
                if (nx>x && nx>=0 && nx<w) nIntersections++;
            }
            j=i;
        }
        
        return nIntersections & 1; // odd number of intersections -> IN
    }
    
    static inline void putpixel(device color*bmp, int x, int y, color col, int w, int h) {
        if(x>=0 && x<w && y>=0 && y<h) // in clip?
            bmp[x + y * w] = col;
    }
    
    
    static void draw(device const point *points, int nPoints, device color*bmp, int w, int h, color col) {
        
        int nodeX[maxPoints];
        int top=points[0].y, bottom=points[0].y, left=points[0].x, right=points[0].x;
        
        // polygon margins (top, bottom, left, right)
        for (int i=0; i<nPoints; i++) {
            if(points[i].y < top)      top      =points[i].y;
            if(points[i].y > bottom)   bottom   =points[i].y;
            if(points[i].x > right)    right    =points[i].x;
            if(points[i].x < left)     left     =points[i].x;
        }
        
        //  Loop through the rows of the image.
        for (int pixelY=top; pixelY<bottom; pixelY++) {
            
            //  Build a list of nodes.
            int nodes=0, j=nPoints-1;
            for (int i=0; i<nPoints; i++) {
                if ( (points[i].y < pixelY && points[j].y >= pixelY) ||
                    (points[j].y < pixelY && points[i].y >= pixelY) ) {
                    nodeX[nodes++]=(points[i].x + (float)(pixelY-points[i].y) / (points[j].y-points[i].y)
                                    * (points[j].x - points[i].x)); }
                j=i;
            }
            
            //  Sort the nodes, via a simple “Bubble” sort.
            for (int i=0; i<nodes-1;) {
                if (nodeX[i]>nodeX[i+1]) {
                    int swap=nodeX[i];
                    nodeX[i]=nodeX[i+1];
                    nodeX[i+1]=swap;
                    if (i) i--;
                } else {
                    i++;
                }
            }
            
            //  Fill the pixels between node pairs.
            for (int i=0; i<nodes; i+=2) {
                if   (nodeX[i  ]>=right) break;
                if   (nodeX[i+1]> left ) {
                    if (nodeX[i  ]< left ) nodeX[i  ]=left ;
                    if (nodeX[i+1]> right) nodeX[i+1]=right;
                    for (int pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++)
                        putpixel(bmp, pixelX, pixelY, col, w, h);
                }
            }
        }
    }
};


void kernel
drawPoly(device const  point*points[[buffer(0)]],
         device const int& nPoints[[buffer(1)]],
         device color *bmp[[buffer(2)]],
         device const int &w[[buffer(3)]],
         device const int &h[[buffer(4)]],
         device const color &color[[buffer(5)]],
         
         uint2  pos [[thread_position_in_grid]] // w x h
         ) {
    
    if(PolygonDrawer::inSide(points, nPoints, pos.x, pos.y, w, h))
        bmp[ pos.x + pos.y * w] = color;
}

void kernel
drawPolySet(device const point*points[[buffer(0)]],
            device const int&nPoints[[buffer(1)]],
            device color *bmp[[buffer(2)]],
            device const int &w[[buffer(3)]],
            device const int &h[[buffer(4)]],
            device const color*colors[[buffer(5)]],
            
            uint2  pos [[thread_position_in_grid]] // nSets x 1
            ) {
    
    int setn=pos.x;
    
    PolygonDrawer::draw(points + nPoints*setn, nPoints, bmp, w, h, colors[setn]);
}

