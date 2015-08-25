// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import <Foundation/Foundation.h>

@protocol APICompleteDelegate

- (void)finished;

@end

@interface SSOAPI : NSObject
{
    id <APICompleteDelegate> delegate;

    NSString *loginAbsUrl;
    NSDictionary *apiJSON;
}

@property (retain, nonatomic) id <APICompleteDelegate> delegate;
@property (retain, nonatomic) NSURLConnection *connection;
@property (retain, nonatomic) NSString *receivedString;
@property (retain, nonatomic) NSMutableData *receivedData;
@property (retain, nonatomic) NSMutableURLRequest *request;
@property (retain, nonatomic) NSString *loginAbsUrl;
@property (retain, nonatomic) NSDictionary *apiJSON;

- (id)initRequestPost:(NSString *)strParams;
- (id)initRequestGet:(NSString *)url;

@end
