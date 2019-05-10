//
//  ViewController.m
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import "ViewController.h"
#import "GeneticImage.hpp"
#import "image/ImageHelper.h"

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    srand((uint)time(0));
    
    geneticImg = new GeneticImage([_imageReff image]);
}

- (void)setStatus: (NSString*)stat {
    _lblStatus.stringValue=stat;
}
@end
