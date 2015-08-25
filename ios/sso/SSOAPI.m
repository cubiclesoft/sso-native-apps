// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import "SSOAPI.h"
#import "SSOParser.h"

NSURLConnection *connection;
NSMutableData *receivedData;

@implementation SSOAPI

@synthesize delegate;
@synthesize request = _request;
@synthesize connection = _connection;
@synthesize receivedData = _receivedData;
@synthesize receivedString;
@synthesize loginAbsUrl;
@synthesize apiJSON;

-(id)initRequestPost:(NSString *)strParams
{
    self = [super init];
    if (self)
    {
        [connection cancel];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
        [request setURL:[NSURL URLWithString:@"https://barebonescms.com/sso/native_app/api/"]];
        NSString *postData = [[NSString alloc] initWithString:(NSString *)strParams];
        [request setHTTPMethod:@"POST"];
        [request setValue:@"application/x-www-form-urlencoded; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPBody:[postData dataUsingEncoding:NSUTF8StringEncoding]];

        NSLog(@"Data will be received from URL: %@", request.URL);
        connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
        if (connection) {
            NSMutableData *data = [[NSMutableData alloc] init];
            receivedData = data;
        } else {
            NSLog(@"Connection failed.");
        }
     }
     return self;
}

-(id)initRequestGet:(NSString *)getUrl
{
    self = [super init];
    if (self)
    {
        [connection cancel];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
        [request setURL:[NSURL URLWithString:getUrl]];

        NSLog(@"Data will be received from URL: %@", request.URL);
        connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
        if (connection) {
            NSMutableData *data = [[NSMutableData alloc] init];
            receivedData = data;
        } else {
            NSLog(@"Connection failed.");
        }
    }
    return self;
}

#pragma mark -
#pragma mark - NSURLConnectionDelegate protocol for asynchronous request

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    [receivedData setLength:0];

    NSHTTPURLResponse * httpResponse;

    httpResponse = (NSHTTPURLResponse *) response;
    assert( [httpResponse isKindOfClass:[NSHTTPURLResponse class]] );

    if ((httpResponse.statusCode / 100) != 2) {
        NSLog(@"httpResponse is %ld",(long)httpResponse.statusCode);
    } else {
        NSLog(@"Response OK.");
    }
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    [receivedData appendData:data];
    NSLog(@"%@", receivedData);
}

- (NSCachedURLResponse *)connection:(NSURLConnection *)connection
                  willCacheResponse:(NSCachedURLResponse*)cachedResponse
{
    return nil;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    NSLog(@"%@",error);
}

-(NSURLRequest *)connection:(NSURLConnection *)connection
            willSendRequest:(NSURLRequest *)inRequest
           redirectResponse:(NSURLResponse *)inRedirectResponse
{
    if (inRedirectResponse)
    {
        NSMutableURLRequest *r = [inRequest mutableCopy]; // original request
        [r setURL: [inRequest URL]];
        return r;
    }
    else
    {
        return inRequest;
    }
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    // Convert received data into string for testing or other functional purposes.
    receivedString = [[NSString alloc] initWithData:receivedData
                                           encoding:NSASCIIStringEncoding];
    NSLog( @"From connectionDidFinishLoading: %@", receivedString );

    // Parse our current URL for login and redirection.
    loginAbsUrl = [[SSOParser alloc] urlRequestToString:[connection currentRequest]];

    // This is our JSON data for parsing in the view.
    NSError *jsonError = nil;
    apiJSON = [NSJSONSerialization JSONObjectWithData:receivedData options:0 error:&jsonError];

    // Tell our viewcontroller that we're done and it can get its data.
    [delegate finished];
}

@end