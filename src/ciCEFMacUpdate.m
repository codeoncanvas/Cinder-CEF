//
//  ciCEFMacUpdate.m
//  CEFCinderSample
//
//  Created by Dave Towey on 2/4/18.
//

#import "ciCEFMacUpdate.h"
#include "ciCEF.h"

@implementation ciCEFMacUpdate

- (ciCEFMacUpdate *)init:(id)_cef
{
    self = [super init];
    cef = _cef;
    float interval = 1;
    NSTimer *mAnimationTimer = [NSTimer     timerWithTimeInterval:interval
                                                       target:self
                                                     selector:@selector(timerFired:)
                                                     userInfo:nil
                                                      repeats:YES];

[[NSRunLoop currentRunLoop] addTimer:mAnimationTimer forMode:NSEventTrackingRunLoopMode];
    return self;
}

- (void)timerFired:(NSTimer *)t {
    
    ((coc::ciCEF*)&cef)->macHack();
    
}

@end
