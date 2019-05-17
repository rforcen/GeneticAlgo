//
//  GeneticGenerator.hpp
//  GeneticAlgos
//
//  Created by asd on 16/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef GeneticGenerator_hpp
#define GeneticGenerator_hpp

#include <random>
#include <float.h>

#include "Circle.hpp"
#include "polygon.hpp"
#include "Thread.h"


namespace GeneConsts {
    static const int
    MUTATIONS_PER_GENERATION = 100,
    GENETAIONS_PER_IMAGE = 50,
    GENERAIONS_PER_SAVE = 100;
    
    static float
    MUTATION_CHANCE = 0.02, // 0.02
    ADD_GENE_CHANCE = 0.3,
    REM_GENE_CHANCE = 0.2,
    INITIAL_GENES = 200;
};



template <class T> class Gene {
public:
    Gene(){}
    Gene(int w, int h) {
        geo=T(w,h);
    }
    Gene(const Gene &other) {
        geo = other.geo;
    }
    Gene &operator=(const Gene &other) {
        if (&other == this) return *this; // check for self-assignment
        geo=other.geo;        
        return *this;
    }
    bool operator!=(const Gene& other) {
        return geo != other.geo;
    }
    void draw(color*bmp, int w, int h) {
        geo.draw(bmp, w, h);
    }
    void mutate(int w, int h) { geo.mutate(w,h); }
    
    T geo;
};


template <class T> class Organism : Random {
public:
    Organism(){}
    Organism(int ng, int w, int h){
        for (int i=0; i<ng; i++)
            genes.push_back(Gene<T>(w,h));
    }
    Organism(const Organism &other) {
        score=other.score;   genes=other.genes;
    }
    ~Organism() { genes.clear(); }
    
    Organism &operator=(const Organism &other) {
        if (&other == this) return *this; // check for self-assignment
        score=other.score;    genes=other.genes;
        
        return *this;
    }
    
    Organism &mutate(int w, int h) {
        if(genes.size() < GeneConsts::INITIAL_GENES) {
            for (auto &g : genes)
                if(GeneConsts::MUTATION_CHANCE > rnd())
                    g.mutate(w, h);
        } else {
            for (auto &g : sample(genes.size() * GeneConsts::MUTATION_CHANCE))
                g.mutate(w, h);
        }
        if(GeneConsts::ADD_GENE_CHANCE > rnd()) {
            genes.push_back(Gene<T>(w, h));
            if (genes.size() > 0 && GeneConsts::REM_GENE_CHANCE > rnd())
                genes.erase(genes.begin() + randint(0, (int)genes.size()));
        }
        return *this;
    }
    
    vector<Gene<T>>sample(int k) { // get k random genes
        vector<Gene<T>>out;
        std::sample(genes.begin(), genes.end(), std::back_inserter(out),
                    k, std::mt19937{std::random_device{}()});
        return out;
    }
    
    float scoreIt(color*target, int w, int h) { // must draw in bmp before
        int size=w*h;
        
        color*bmp=new color[size];
        draw(bmp, w, h);
        
        score=0;
        for (int i=0; i<size; i++)
             score += Color(target[i]) - Color(bmp[i]);
        
        delete[]bmp;
        return score=scaleScore(score, size);
    }
    
    float scaleScore(float score, int size) {  return  (score/255. * 100)/size;     }
    
    void draw(color*bmp, int w, int h) {
        memset(bmp, 0xff, (w*h)*sizeof(color)); // clear image w/0xff
        for (auto g : genes) g.draw(bmp, w, h); // draw all genes
    }
    
    Organism<T> genMutation(color*target, int w, int h) { // create a mutation & score it
        Organism mutOrg(*this);
        mutOrg.mutate(w, h).scoreIt(target, w, h);
        return mutOrg;
    }
    
    vector<Gene<T>>genes;
    float score = FLT_MAX;
};


template <class T> class GeneticGenerator {
public:
    GeneticGenerator(){}
    GeneticGenerator(color*target, int w, int h) : target(target), w(w), h(h), size(w*h), sizeBytes(size*sizeof(color)) {
        bmp=new color[size];
        parent = Organism<T>(GeneConsts::INITIAL_GENES, w,h);
        parent.scoreIt(target, w, h);
        generate();
    }
    ~GeneticGenerator(){
        delete[]bmp;
    }
    
    void generate() {
        generation++;
        prevScore = parent.score;
        
        Thread(nOrgs-1).run([this](int i) {
            organisms[i] = parent.genMutation(target, w, h);
        });
//        for (int i=0; i<nOrgs-1; i++)
//               organisms[i] = parent.genMutation(target, bmp, w, h);
        organisms[nOrgs-1]=parent; // current organism
        
        parent = getBest(); // from organisms[]
        parent.draw(bmp, w, h);
    }
    
    float getScore() { return parent.score; }
    
    Organism<T> &getBest() { // get best organism (min score) -> natural selection
        int ixMin=(int)(std::min_element(organisms, organisms+nOrgs,
                                         [](Organism<T>&o1, Organism<T>&o2)->bool { return o1.score < o2.score; }) - organisms);
        return organisms[ixMin];
    }
    
    Organism<T>parent;
    // generated mutations
    const int nOrgs=GeneConsts::MUTATIONS_PER_GENERATION;
    Organism<T> organisms[GeneConsts::MUTATIONS_PER_GENERATION];
    
    int w,h, size, sizeBytes;
    float prevScore=0;
    color *target, *bmp;
    int generation=0;
};

extern GeneticGenerator<Circle>*ggCircle;
extern GeneticGenerator<polygon>*ggPoly;
#endif /* GeneticGenerator_hpp */
