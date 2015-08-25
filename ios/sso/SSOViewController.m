// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import "SSOViewController.h"
#import "SSOAPI.h"
#import "SSOParser.h"

@interface SSOViewController ()

@property (assign) BOOL loggedIn;

@end

@implementation SSOViewController

SSOAPI *ws;

@synthesize testBTN;
@synthesize logOutBTN;
@synthesize loggedIn = loggedIn_;
@synthesize apiJSON;
@synthesize connection;
@synthesize receivedData;


NSString *loginURL = @"https://barebonescms.com/sso/native_app/api/login.php";

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark -
#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

- (void)viewDidUnload
{
    [self setTestBTN:nil];
    [self setLogOutBTN:nil];
    [super viewDidUnload];
}

#pragma mark -
#pragma mark - BTN logic

-(IBAction)testClick:(id)sender
{
    [label setText:nil];

    NSString *params = @"action=test";
    ws = [[SSOAPI alloc] initRequestPost:params];
    ws.delegate = self;
}

-(IBAction)logOutClick:(id)sender
{
    if (loggedIn_)
    {
        [label setText:nil];

        NSString *params = @"action=logout";
        ws = [[SSOAPI alloc] initRequestPost:params];
        ws.delegate = self;

        [self setLoggedIn:FALSE];
    }
}

#pragma mark -
#pragma mark - callback logic from API

-(void)finished
{
    NSString *getUrl = ws.loginAbsUrl;
    NSDictionary *infoJSON = ws.apiJSON;
    // NSString *theHTML = ws.receivedString;

    if ([getUrl isEqual: loginURL])
    {
        SSOWebViewController *controller = [[SSOWebViewController alloc] initWithString:getUrl];

        [self.navigationController pushViewController:controller animated:YES];
    }

    NSNumber *apiSuccess = (NSNumber *)[infoJSON valueForKey:@"success"];
    NSString *apiError = [infoJSON valueForKey:@"error"];
    NSString *apiErrorCode = [infoJSON valueForKey:@"errorcode"];
    NSString *apiSuccessMsg = [infoJSON valueForKey:@"msg"];

    // Localize strings from localizable.strings using NSLocalizedString, shown below
    if (apiSuccess && [apiSuccess boolValue] == YES)
    {
        [label setText:apiSuccessMsg];

        [self setLoggedIn:TRUE];

        if ([apiSuccessMsg isEqualToString:@"Successfully logged out."]) {[self setLoggedIn:FALSE];};

        return;
    }
    else if (apiErrorCode && [apiErrorCode isEqual:@"no_login"] && !loggedIn_)
    {
        UIAlertView *testmessage = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"sso_api_no_login_title", nil)
                                                              message:NSLocalizedString(@"sso_api_no_login_message", nil)
                                                             delegate:self
                                                    cancelButtonTitle:@"Dismiss"
                                                    otherButtonTitles:@"Log In",nil];
        [testmessage show];

        return;
    }
    else if (apiErrorCode && [apiErrorCode isEqual:@"random_error"])
    {
        UIAlertView *testmessage = [[UIAlertView alloc] initWithTitle:@"Random Error"
                                                              message:apiError
                                                             delegate:self
                                                    cancelButtonTitle:@"Dismiss"
                                                    otherButtonTitles:nil];
        [testmessage show];

        return;
    }
}

#pragma mark -
#pragma mark - Alert view handling

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSString *title = [alertView buttonTitleAtIndex:buttonIndex];

    if([title isEqualToString:@"Dismiss"] || [title isEqualToString:@"OK"])
    {
        //NSLog(@"alert dismissed");
    }
    else if([title isEqualToString:@"Log In"])
    {
        ws = [[SSOAPI alloc] initRequestGet:loginURL];
        ws.delegate = self;
    }

    [alertView dismissWithClickedButtonIndex:buttonIndex animated:YES];
}

@end
