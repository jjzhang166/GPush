//
//  ClientWrapper.m
//  ios_client_demo
//
//  Created by txj on 16/4/11.
//  Copyright © 2016年 gpush. All rights reserved.
//

#import "ClientWrapper.h"
#import "Iosclient.h"
@implementation ClientWrapper
{
    gim::IosClient* c;
    handerMessage msghander;
}

+ (instancetype)shared
{
    static ClientWrapper *_shared = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _shared = [[self alloc] init];
    });
    return _shared;
}
int calbakFunc(std::string msg)
{
    NSLog(@"loginCallback");
    ClientWrapper *cw=[ClientWrapper shared];
    cw.hmsg([NSString stringWithCString:msg.c_str() encoding:NSUTF8StringEncoding]);
    return 1;
}
-(void)hms
{
}
-(void)initClientWithHander:(handerMessage)h
{
    self.hmsg=h;
    c=new gim::IosClient();
//    c->setPublicKeyFile([[[NSBundle mainBundle] pathForResource:@"rsa_public_key" ofType:@"pem"] UTF8String]);
    c->setCallBackFun(calbakFunc);
    c->init();
    
}
-(int)login:(NSString *)anSrvip srvport:(int)anSrvport cid:(NSString *)anCid  version:(NSString *)anVersion  token:(NSString *)anToken
{
    return c->login([anSrvip UTF8String], anSrvport, [anCid UTF8String], [anVersion UTF8String], [anToken UTF8String]);
}

@end
