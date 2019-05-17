//
//  Circle.hpp
//  GeneticAlgos
//
//  Created by asd on 16/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef Circle_hpp
#define Circle_hpp

#include "Geo.hpp"
#include "Random.hpp"

class Circle : Random {
public:
    Circle(){}
    Circle(int w, int h) :
        pos(point(randint(0,w), randint(0,h))),
        diameter(randint(w/40, w/10)),
        color(Color(randint(0, 255), randint(0, 255), randint(0, 255))),
        rad2((diameter/2) * (diameter*2)){}
    
    inline bool contains(int x, int y) {
        x-=pos.x; y-=pos.y;
        return x*x + y*y < rad2;
    }
    
    void draw(color*bmp, int w, int h) {
        for (int y=0, ibmp=0; y<h; y++)
            for (int x=0; x<w; x++, ibmp++) {
                if(contains(x, y))
                    bmp[ibmp] = color.asColor;
            }
    }
    
    
    void mutate(int w, int h) {
        switch (randint(0, 3)) {
            case 0: diameter = randint(w/40, w/10); break;
            case 1: pos   = point(randint(0, w), randint(0, h)); break;
            case 2: color = Color(randint(0, 255), randint(0, 255), randint(0, 255));
        }
    }
    
    int diameter=0, rad2=0;
    point pos;
    Color color;
};

#endif /* Circle_hpp */
