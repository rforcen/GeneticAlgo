//
//  Random.h
//  GeneticAlgos
//
//  Created by asd on 16/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef Random_h
#define Random_h

#include <stdlib.h>

class Random {
public:
    int randint(int from, int to) {
        return (rand_r(&seed) % (to-from)) + from;
    }
    float rnd() {
        return (float)rand_r(&seed) / RAND_MAX;
    }
private:
    static unsigned seed; // same for all instances
};
#endif /* Random_h */
