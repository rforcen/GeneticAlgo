//
//  GeneticImage.hpp
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#ifndef GeneticImage_hpp
#define GeneticImage_hpp

#include <stdlib.h>
#include <vector>
#include <random>
#include <iterator>
#include <algorithm>
#include "Thread.h"
#include "timer.h"

#include <Cocoa/Cocoa.h>
#include "metal/MetalDevice.h"
#include "image/ImageBuffer.h"
#include "image/ImageHelper.h"

const int
MUTATIONS_PER_GENERATION = 100,
GENETAIONS_PER_IMAGE = 50,
GENERAIONS_PER_SAVE = 100;

const float
MUTATION_CHANCE = 0.02,
ADD_GENE_CHANCE = 0.3,
REM_GENE_CHANCE = 0.2,
INITIAL_GENES = 200;

typedef uint8_t byte;
typedef uint16_t word;

using std::vector;

class _Point {
    public:
    _Point(){}
    _Point(int x, int y) : x(x), y(y) {}
    
    int x,y;
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
    
    inline word operator-(const Color& other) const {
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
    Gene(){} // empty gene is at least diameter==0 
    
    Gene(int w, int h) : diameter(randint(w/30, w/15)), pos(_Point(randint(0, w), randint(0, h))), color(Color(randint(0, 255), randint(0, 255), randint(0, 255))), rad2(diameter*diameter) {
    }
    
    void mutate(int w, int h) {
        switch (randint(0, 3)) {
            case 0: diameter = randint(w/40, w/10); break;
            case 1: pos = _Point(randint(0, w), randint(0, h)); break;
            case 2: color = Color(randint(0, 255), randint(0, 255), randint(0, 255));
        }
    }
    
    inline bool inCircle(int x, int y) {
        x-=pos.x; y-=pos.y;
        return x*x + y*y < rad2;
    }
    inline bool inImage(int x, int y, int w, int h) const { return (x>=0 && y>=0 && x<w && y<h);  }
    
    uint32_t* draw(uint32_t *bmp, int w, int h) {
        for (int y=0, ibmp=0; y<h; y++)
        for (int x=0; x<w; x++, ibmp++) {
            if(inCircle(x, y))
            bmp[ibmp] = color.asColor;
        }
        return bmp;
    }
    
    int randint(int from, int to) {
        static unsigned seed;
        return (rand_r(&seed) % (to-from)) + from;
    }
    
    int diameter=0, rad2=0;
    _Point pos;
    Color color;
};

class Organism {
    public:
    Organism(){}
    Organism(int w, int h, int num) {
        for (int i=0; i<num; i++) genes.push_back(Gene(w,h));
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
        if(genes.size() < INITIAL_GENES) {
            for (auto g : genes)
            if(MUTATION_CHANCE > rnd()) g.mutate(w, h);
        } else {
            for (auto g : sample(genes.size() * MUTATION_CHANCE)) g.mutate(w, h);
        }
        if(ADD_GENE_CHANCE > rnd()) {
            genes.push_back(Gene(w, h));
            if (genes.size() > 0 && REM_GENE_CHANCE > rnd()) {
                auto ir=randInt((int)genes.size());
                genes.erase(genes.begin() + ir);
            }
        }
        return *this;
    }
    vector<Gene>sample(int k) { // get k random genes
        vector<Gene>out;
        std::sample(genes.begin(), genes.end(), std::back_inserter(out),
                    k, std::mt19937{std::random_device{}()});
        return out;
    }
    float rnd() {
        static unsigned seed;
        return (float)rand_r(&seed) / RAND_MAX;
    }
    float randInt(int max) {
        static unsigned seed;
        return rand_r(&seed) % max;
    }
    
    uint32_t* draw(uint32_t*bmp, int w, int h) {
        memset(bmp, 0xff, (w*h)*sizeof(uint32_t)); // clear image w/0xff
        
        for (auto g : genes) g.draw(bmp, w, h); // draw all genes
        return bmp;
    }
    
    float adjustScore(float score, int size) {  return  (score/255. * 100)/size;     }
    
    float scoreIt(uint32_t*target, int w, int h) {
        int size=w*h;
        score=0;
        for (int y=0, iimg=0; y<h; y++) {
            for (int x=0; x<w; x++, iimg++) {
                Color topColor; // top color in x,y
                for (auto g : genes) {
                    if (g.inCircle(x, y))
                    topColor = g.color;
                }
                score += Color(target[iimg]) - topColor;
            }
        }
        return score=adjustScore(score, size);
    }
    
    Organism genMutation(uint32_t*target, int w, int h) { // create a mutation & score it
        Organism mutOrg(*this);
        mutOrg.mutate(w, h).scoreIt(target, w, h);
        return mutOrg;
    }
    Organism genMutation(int w, int h) { // create a mutation & score it
        Organism mutOrg(*this);
        mutOrg.mutate(w, h);
        return mutOrg;
    }
    
    vector<Gene>genes;
    float score;
};

class GeneticImage {
    
    public:
    GeneticImage(NSImage*itarget) {
        NSSize sz=[itarget size];
        
        w=(int)sz.width; h=(int)sz.height; // reff image geo
        size=w*h; sizeBytes=size*sizeof(uint32_t);
        
        generation=0;
        parent=Organism(w, h, INITIAL_GENES);
        prevScore=-1;
        
        
        md     = [MetalDevice init];
        
        imgBuffer = [ImageBuffer initWithWidth:w Height:h]; // display buffer
        
        target  =(uint32_t*)[md newData: sizeBytes];
        bmp     =(uint32_t*)[md newData: sizeBytes]; // local working copy must memcpy to imgBuffer todisplay

        auto _target = (uint32_t*)[ImageHelper convertImageToBitmapRGBA8:itarget]; // target=itarget->(uint32_t*)
        memcpy(target, _target, sizeBytes);
        free(_target);
        
        targetBuff = [md copyBuffer:target length:sizeBytes]; // permanent buffers
        bmpBuff    = [md copyBuffer:bmp    length:sizeBytes];

        scoreParentMetal();
    }
    
    ~GeneticImage() {
        [md deleteData:sizeBytes data:target];
        [md deleteData:sizeBytes data:bmp];
    }
    
    
    void generate() {
        generation++;
        prevScore = parent.score;
        
        Thread(nOrgs).run([this](int t, int from, int to) {
            for (int i=from; i<to; i++)
            organisms[i] = parent.genMutation(target, w, h);
        });
        organisms[0]=parent; // 0 is current organism
        
        parent = getBest(); // from organisms[]
        parent.draw(imgBuffer.imgBuff32, w, h);
    }
    
    void generateGenePool() { // generate gene pool new generation not scored
        generation++;
        prevScore = parent.score;
        
        // generate not scored mutations
        Thread(nOrgs).run([this](int t, int from, int to) {
            for (int i=from; i<to; i++)
            organisms[i] = parent.genMutation(w, h);
        });
        organisms[0]=parent; // 0 is current organism
        
        generateGenePoolSquared();
        // calc scores and assign to parent
    }
    
    void generateGenePoolSquared() { // generate squared gene pool from organisms nOrgs x maxGenePerOrg
        maxGenePerOrg = (int)(std::max_element(organisms, organisms+nOrgs,  [](Organism&o1, Organism&o2)->bool { return o1.genes.size() < o2.genes.size(); })->genes.size() ); // max # of genes in organisms
        
        genePool.clear();
        
        for (auto i=0; i<nOrgs; i++) {
            int n=0;
            Organism &org=organisms[i];
            
            for (auto g : org.genes) { genePool.push_back(g); n++; }
            for (int j=0; j<maxGenePerOrg-n; j++) genePool.push_back(Gene()); // fill to fit maxGenePerOrg w/ empty gene (diameter==0) checked in shader
        }
    }
    
    void setParent(int orgIndex, float score) {
        parent = organisms[orgIndex];
        parent.score=score;
    }
    
    void draw() {  parent.draw(bmp, w, h);    }
    
    float getScore() { return parent.score; }
    
    Organism &getBest() { // get best organism (min score) -> natural selection
        int ixMin=(int)(std::min_element(organisms, organisms+nOrgs,
                                         [](Organism&o1, Organism&o2)->bool { return o1.score < o2.score; }) - organisms);
        return organisms[ixMin];
    }
    
    float adjustScore(float s) { return parent.adjustScore(s, size); }
    
    void generateMetal() {
        uint szScores=nOrgs * sizeof(uint), *scores_uint=(uint*)[md newData:szScores];
        
        generateGenePool(); // -> genePool of maxGenePerOrg
        
        [md compileFunc:@"scoreOrganisms"]; // generate cellScores(w,h,nOrg)
        
        int nGene=(int)genePool.size(), szGene=nGene*sizeof(Gene); // parent genes, nGene
        Gene *genes=(Gene*)[md newData:szGene];
        memcpy(genes, genePool.data(), szGene);
        
        id<MTLBuffer>genesBuff      = [md copyBuffer:genes          length:szGene];
        id<MTLBuffer>scores_uintBuff= [md copyBuffer:scores_uint    length:szScores];
        
        [md setBufferParam:targetBuff       index:0]; // shader parameters: target, genes, index, scores
        [md setBufferParam:genesBuff        index:1];
        [md setIntParam:&maxGenePerOrg      index:2];
        [md setBufferParam:scores_uintBuff  index:3]; // -> sum by organism (sum(w*h))
        [md setIntParam:&w                  index:4];
        
        [md runThreads3d:w height:h depth:nOrgs];    // w x h x nOrg grid -> cellSCores
        
        float *scores=new float[nOrgs]; // -> float scores from uit scoresTmp
        for (int i=0; i<nOrgs; i++) scores[i] = adjustScore(scores_uint[i]);
        
        int ixMin=(int)(std::min_element(scores, scores+nOrgs) - scores); // get min item index
        
        setParent(ixMin, scores[ixMin]);
        
        delete[]scores;
        
        [md deleteData:szScores data:scores_uint];
        [md deleteData:szGene   data:genes];
        
        drawMetal();
    }
    
    void scoreParentMetal() {
        [md compileFunc:@"scoreParent"]; // generate cellScores(w,h,nOrg)
        
        uint szScore=1*sizeof(uint), *score=(uint*)[md newData:szScore];
        id<MTLBuffer>scoreBuff = [md copyBuffer:score  length:szScore];
        
        int nGene=(int)parent.genes.size(), szGene=nGene*sizeof(Gene); // parent genes, nGene
        Gene *genes=(Gene*)[md newData:szGene];
        memcpy(genes, parent.genes.data(), szGene);
        id<MTLBuffer>genesBuff = [md copyBuffer:genes  length:szGene];
        
        [md setBufferParam:targetBuff   index:0]; // shader parameters: target, genes, index, score, w
        [md setBufferParam:genesBuff    index:1];
        [md setIntParam:&nGene          index:2];
        [md setBufferParam:scoreBuff    index:3]; // -> score
        [md setIntParam:&w              index:4];
        
        [md  runThreadsWidth:w height:h];    // w x h x nOrg grid -> cellSCores
        
        parent.score = adjustScore(*score);
        
        [md deleteData:szScore data:score];
        [md deleteData:szGene data:genes];
    }
    
    void drawMetal() { // draw parent organism
        [md compileFunc:@"drawOrganism"];
        
        int nGene=(int)parent.genes.size(), szGene=nGene*sizeof(Gene); // parent genes, nGene
        Gene *genes=(Gene*)[md newData:szGene];
        memcpy(genes, parent.genes.data(), szGene);
        id<MTLBuffer>genesBuff = [md copyBuffer:genes  length:szGene];
        
        [md setBufferParam:bmpBuff      index:0]; // shader parameters: bmp, genes, nGene, w
        [md setBufferParam:genesBuff    index:1];
        [md setIntParam:&nGene          index:2];
        [md setIntParam:&w              index:3];
        
        [md runThreadsWidth:w height:h];    // w x h -> bmpBuff
        
        [md deleteData:szGene data:genes];
        
        memcpy(imgBuffer.imgBuff32, bmp, sizeBytes); // copy to imgBuffer to display w/getImage
    }
    
    int w,h, size, sizeBytes;
    float prevScore;
    uint32_t *target, *bmp;
    int generation;
    Organism parent;
    
    // generate mutations
    const int nOrgs=MUTATIONS_PER_GENERATION;
    Organism organisms[MUTATIONS_PER_GENERATION];
    
    vector<Gene>genePool; // gene pool
    int maxGenePerOrg=0;
    
    MetalDevice*md;
    id<MTLBuffer>targetBuff, bmpBuff;

    
    ImageBuffer*imgBuffer;
};


extern GeneticImage*geneticImg; // global object
#endif /* GeneticImage_hpp */
