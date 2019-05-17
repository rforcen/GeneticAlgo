//
//  PolygonDrawer.hpp
//  GeneticAlgos
//
//  Created by asd on 15/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef PolygonDrawer_hpp
#define PolygonDrawer_hpp

#include <stdlib.h>
#include "Thread.h"

typedef uint32_t color; // ABGR, i.e. red=0xff0000ff

class point {
public:
    point(){}
    point(int x, int y) : x(x), y(y){}
    int x=0,y=0;
};


class PolygonDrawer { // http://alienryderflex.com/polygon_fill/
public:
    PolygonDrawer(){}
    
    color topColor(const point *points, int nPoints, color*bmp, int x, int y, int w, int h, color col) {
        
        //  calc. number of intersections
        int nIntersections=0, j=nPoints-1;
        for (int i=0; i<nPoints; i++) {
            if ( (points[i].y < y && points[j].y >= y) ||
                (points[j].y < y && points[i].y >= y) ) {
                auto nx=(points[i].x + (float)(y-points[i].y) / (points[j].y-points[i].y)
                         * (points[j].x - points[i].x));
                if (nx>=x) nIntersections++;
            }
            j=i;
        }
        
        if (nIntersections & 1) return col; // odd number of intersections -> IN
        else return bmp[x+y*w]; // OUT
    }
    
    bool inSide(const point *points, int nPoints, int x, int y, int w, int h) {
        
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
    
    void draw(point *points, int nPoints, color*bmp, int x, int y, int w, int h, color col) {
        if(inSide(points, nPoints, x, y, w, h))
            bmp[x+y*w]=col;
    }

    
    void draw(point *points, int nPoints, color*bmp, int w, int h, color col) {
        
        int nodeX[nPoints];
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
    
    inline void putpixel(color*bmp, int x, int y, color col, int w, int h) {
        if(x>=0 && x<w && y>=0 && y<h) // in clip?
            bmp[x + y * w] = col;
    }
    
    void testMultiple(color*bmp, int w, int h, color col) {
        Thread(300).run([this, bmp, w, h, col](){
            testRand(bmp, w, h, 0xff000000 | rand());
        });
    }
    
    void testMultipleTopColor(color*bmp, int w, int h, color col) {
        for (int i=0; i<300; i++)
            testRandtopColor(bmp, w, h, 0xff000000 | rand());
    }
    
    void testRand(color*bmp, int w, int h, color col) {
        static unsigned seed;
        
        int np=(rand_r(&seed) % 20)+5;
        point pnts[np];
        for (int i=0; i<np; i++) pnts[i]=point(rand_r(&seed)%w, rand_r(&seed)%h);
        draw(pnts, np, bmp, w, h, col);
    }
    
    void testRandtopColor(color*bmp, int w, int h, color col) {
        int np=(rand() % 20)+5;
        point *pnts=new point[np];
        for (int i=0; i<np; i++) pnts[i]=point(rand()%w, rand()%h);
        
        Thread(h).run([this, pnts, np, w, h, col, bmp](int y){
             for (int x=0; x<w; x++)
                 draw(pnts, np, bmp, x, y, w, h, col);
        });
        
        delete[]pnts;
    }
    void test(color*bmp, int w, int h, color col) {
        point pnts[]={{3,1},{6,3},{3,7},{1,3},{5,4}};
        for (int i=0; i<5; i++) {pnts[i].x*=30; pnts[i].y*=40;}
        draw(pnts, sizeof(pnts)/sizeof(*pnts), bmp, w, h, col);
    }
    
    void testTopColor(color*bmp, int w, int h, color col) {
        point pnts[]={{3,1},{6,3},{3,7},{1,3},{5,4}};
        int np=5;
        for (int i=0; i<np; i++) {pnts[i].x*=20; pnts[i].y*=20;}
        
        draw(pnts, np, bmp, w, h, 0xffff0000);
        for (int y=0; y<h; y++)
            for (int x=0; x<w; x++)
                if (inSide(pnts, np, x, y, w, h)) bmp[x+y*w]=col;
        
    }
    
};

#endif /* PolygonDrawer_hpp */
