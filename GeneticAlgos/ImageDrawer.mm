//
//  ImageDrawer.m
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import "ImageDrawer.h"
#import "image/ImageHelper.h"

#import "timer.h"
#import "ViewController.h"

#import "GeneticGenerator.hpp"

@implementation ImageDrawer{
    long tCPU;
}

-(void)awakeFromNib {} // never generate metal resources here, always in drawRect!!

-(void)refresh {
    [self setNeedsDisplay:YES];
}

-(void)logit {
    ViewController* vc = (ViewController*) [[[[NSApplication sharedApplication] mainWindow]windowController]contentViewController];
    
    [vc setStatus: [NSString stringWithFormat:@"generation:%d, score:%.3f, genes:%ld, time=%ld ms", ggPoly->generation, ggPoly->getScore(), ggPoly->parent.genes.size(), tCPU]];
    //    [vc setStatus: [NSString stringWithFormat:@"generation:%d, score:%.3f, genes:%ld, time=%ld ms", ggCircle->generation, ggCircle->getScore(), ggCircle->parent.genes.size(), tCPU]];
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
    
    if(ggPoly) {
        self->tCPU=Timer().chronoMilli([self](){ // CPU multithread version
            ggPoly->generate();
            //            ggCircle->generate();
        });
        [self logit];
        [[ImageHelper convertBitmapRGBA8ToUIImage:(byte*)ggPoly->bmp withWidth:ggPoly->w withHeight:ggPoly->h] drawInRect:rect];
        //        [[ImageHelper convertBitmapRGBA8ToUIImage:(byte*)ggCircle->bmp withWidth:ggCircle->w withHeight:ggCircle->h] drawInRect:rect];
    }
}

@end
