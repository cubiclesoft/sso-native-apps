// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import "SSOWebViewController.h"
#import "SSOParser.h"

@implementation SSOWebViewController
@synthesize webView;
@synthesize str;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(id) initWithString:(NSString *)s{

    self = [super init];

    if (self) {
        self.str = s;
    }

    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];

    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    NSURL *url = [NSURL URLWithString:self.str];
    NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
    [self.webView loadRequest:requestObj];
    NSLog(@"loading");
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    NSLog(@"viewDidStartLoad seems to be working");
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{

    NSURL *currentURL = [self.webView.request mainDocumentURL];
    NSString *stringurl = [currentURL absoluteString];

    NSLog(@"%@", stringurl);

    NSString *currentParseString = [NSString stringWithFormat:@"%@",stringurl];
    SSOParser *parser = [[SSOParser alloc] initWithURLString:currentParseString];
    NSString *finalAction = [parser valueForVariable:@"action"];
    NSLog(@"%@", finalAction);
    if ([finalAction isEqual: @"sso_done"]) {
        [self.navigationController popViewControllerAnimated:YES];

        UIAlertView *webmessage = [[UIAlertView alloc] initWithTitle:@"Logged in."
                                                             message:@"Redirecting back to API test."
                                                            delegate:self
                                                   cancelButtonTitle:@"OK"
                                                   otherButtonTitles:nil];
        [webmessage show];
    }
}

- (void)viewDidUnload
{
    [self setWebView:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


@end

