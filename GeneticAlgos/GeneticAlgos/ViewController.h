//
//  ViewController.h
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController
@property (weak) IBOutlet NSImageView *imageReff;
@property (weak) IBOutlet NSImageView *imageCalc;
@property (weak) IBOutlet NSTextField *lblStatus;

- (void)setStatus: (NSString*)stat;

@end

