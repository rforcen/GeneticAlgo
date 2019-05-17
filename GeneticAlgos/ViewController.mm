//
//  ViewController.m
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import "ViewController.h"
#import "GeneticGenerator.hpp"
#import "image/ImageHelper.h"

@implementation ViewController {
    BOOL running;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    srand((uint)time(0));
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0ul), ^{
        for (self->running=YES; self->running;) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self->_imageMetal refresh];
            });
        }
    });
}

- (void)stop{
    running=NO;
}

- (void)setStatus: (NSString*)stat {
    _lblStatus.stringValue=stat;
}
@end
