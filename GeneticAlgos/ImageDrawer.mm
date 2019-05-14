//
//  ImageDrawer.m
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import "ImageDrawer.h"
#import "image/ImageBuffer.h"

#import "timer.h"
#import "ViewController.h"

#import "GeneticImage.hpp"

@implementation ImageDrawer{
    long tCPU, tGPU;
}

-(void)awakeFromNib {} // never generate metal resources here, always in drawRect!!

-(void)refresh {
    [self setNeedsDisplay:YES];
}

-(void)logit {
    //    NSLog(@"gen:%d, score:%.2f, genes:%ld (cpu=%ld, gpu=%ld)", geneticImg->generation, geneticImg->getScore(), geneticImg->parent.genes.size(), tCPU, tGPU);
    
    ViewController* vc = (ViewController*) [[[[NSApplication sharedApplication] mainWindow]windowController]contentViewController];
    [vc setStatus: [NSString stringWithFormat:@"generation:%d, score:%.3f, genes:%ld (cpu=%ld, gpu=%ld)ms", geneticImg->generation, geneticImg->getScore(), geneticImg->parent.genes.size(), tCPU, tGPU]];
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
    
    if(geneticImg) {
        self->tGPU=Timer().chronoMilli([self](){ // GPU version
            geneticImg->generateMetal();
        });
        
        self->tCPU=Timer().chronoMilli([self](){ // CPU multithread version
            //                      geneticImg->generate();
        });
        [self logit];
        [[geneticImg->imgBuffer getimage] drawInRect:rect];
    }
}

@end
