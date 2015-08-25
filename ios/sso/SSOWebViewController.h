// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import <UIKit/UIKit.h>

@class SSOParser;

@interface SSOWebViewController : UIViewController

@property (retain, nonatomic) IBOutlet UIWebView *webView;
@property (retain, nonatomic) NSString *str;

-(id) initWithString:(NSString *) str;
@end
