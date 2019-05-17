//
//  AppDelegate.m
//  GeneticAlgos
//
//  Created by asd on 06/05/2019.
//  Copyright © 2019 voicesync. All rights reserved.
//

#import "AppDelegate.h"
#import "GeneticGenerator.hpp"
#import "image/ImageHelper.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void) initGneticGenerators {
    auto img = [NSImage imageNamed:@"reference.png"];
    auto target = (color*)[ImageHelper convertImageToBitmapRGBA8:img];
    NSSize sz=[img size];
    
    ggCircle = new GeneticGenerator<Circle>(target, sz.width, sz.height);
    ggPoly   = new GeneticGenerator<polygon>(target,sz.width, sz.height);
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [self initGneticGenerators];
}
- (void)applicationWillTerminate:(NSNotification *)aNotification {}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSNotification *)aNotification { return TRUE; }

@end
