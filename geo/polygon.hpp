//
//  Polygon.hpp
//  GeneticAlgos
//
//  Created by asd on 16/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef Polygon_hpp
#define Polygon_hpp

#include "Geo.hpp"

class polygon : Random {
public:
    polygon() {}
    polygon(int w, int h) : color(Color(randint(0, 255), randint(0, 255), randint(0, 255))) {
        generate(w, h);
    }
    polygon(const polygon &other) { // implement deep copy for vector
        points = other.points;
        color = other.color;
    }
    polygon &operator=(const polygon &other) {
        if (&other == this) return *this; // check for self-assignment
        points = other.points;
        color = other.color;
        return *this;
    }
    
    bool operator!=(const polygon &other) {
        bool eq=points.size() == other.points.size();
        for (int i=0; eq && i<points.size(); i++)
            eq = points[i] == other.points[i];
        return eq;
    }
    
    void mutate(int w, int h) {     generate(w, h);     }
    
    void generate(int w, int h) {
        points.clear();
        int np=randint(3, 10);
        for (int i=0; i<np; i++)
            points.push_back(point(randint(0, w), randint(0, h)));
    }
    
    bool contains(int x, int y, int w, int h) {
        
        int nPoints=(int)points.size();
        
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
    
    void draw(color*bmp, int x, int y, int w, int h) {
        if(contains(x, y, w, h))
            bmp[x+y*w]=color.asColor;
    }
    
    
    void draw(color*bmp, int w, int h) {
        
        int nPoints=(int)points.size();
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
                        putpixel(bmp, pixelX, pixelY, w, h);
                }
            }
        }
    }
    
    inline void putpixel(color*bmp, int x, int y, int w, int h) {
        if(x>=0 && x<w && y>=0 && y<h) // in clip?
            bmp[x + y * w] = color.asColor;
    }
    
    
    vector<point>points;
    Color color;
};
#endif /* Polygon_hpp */
