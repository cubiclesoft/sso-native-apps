// Single Sign-On integration for iOS
// (C) 2013 CubicleSoft.  All Rights Reserved.
//
// Created by JEFF TURMAN.

#import "SSOParser.h"

@implementation SSOParser

@synthesize variables;

#pragma mark -
#pragma mark - parse URL for params

// URL parser logic from http://dev.doukasd.com/2011/03/get-values-from-parameters-in-an-nsurl-string/
- (id) initWithURLString:(NSString *)url
{
    self = [super init];
    if (self != nil) {
        NSString *string = url;
        NSScanner *scanner = [NSScanner scannerWithString:string];
        [scanner setCharactersToBeSkipped:[NSCharacterSet characterSetWithCharactersInString:@"&?"]];
        NSString *tempString;
        NSMutableArray *vars = [NSMutableArray new];
        [scanner scanUpToString:@"?" intoString:nil];
        while ([scanner scanUpToString:@"&" intoString:&tempString]) {
            [vars addObject:[tempString copy]];
        }
        self.variables = vars;
    }
    return self;
}

- (NSString *) valueForVariable:(NSString *)varName
{
    for (NSString *var in self.variables) {
        if ([var length] > [varName length]+1 && [[var substringWithRange:NSMakeRange(0, [varName length]+1)] isEqualToString:[varName stringByAppendingString:@"="]]) {
            NSString *varValue = [var substringFromIndex:[varName length]+1];
            return varValue;
        }
    }
    return nil;
}

- (NSString *) urlRequestToString:(NSURLRequest *)urlRequest
{
    NSString *requestPath = [[urlRequest URL] absoluteString];
    return requestPath;
}

- (void) dealloc{
    self.variables = nil;
}


@end
