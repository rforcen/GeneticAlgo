//
//  ViewController.h
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ImageDrawer.h"

@interface ViewController : NSViewController
@property (weak) IBOutlet NSImageView *imageReff;
@property (weak) IBOutlet NSTextField *lblStatus;
@property (weak) IBOutlet ImageDrawer *imageMetal;

- (void)setStatus: (NSString*)stat;

@end

