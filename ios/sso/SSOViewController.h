// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import <UIKit/UIKit.h>
#import "SSOWebViewController.h"
#import "SSOParser.h"
#import "SSOAPI.h"

@class SSOParser;
@class SSOAPI;

@interface SSOViewController : UIViewController <UIAlertViewDelegate,UIWebViewDelegate,APICompleteDelegate>
{
    UIButton *testBTN, *logOutBTN;
    IBOutlet UILabel *label;
}

@property (retain, nonatomic) IBOutlet UIButton *testBTN, *logOutBTN;

@property (retain, nonatomic) SSOAPI *ws;
@property (retain, nonatomic) NSDictionary *apiJSON;
@property (retain, nonatomic) NSURLConnection *connection;
@property (retain, nonatomic) NSMutableData *receivedData;

// Button events
- (IBAction)testClick:(id)sender;
- (IBAction)logOutClick:(id)sender;

@end
