// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import <UIKit/UIKit.h>


@class SSOViewController;

@interface SSOAppDelegate : UIResponder <UIApplicationDelegate>{}

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) SSOViewController *viewController;

@property (strong, nonatomic) UINavigationController *navigationController;

@end
