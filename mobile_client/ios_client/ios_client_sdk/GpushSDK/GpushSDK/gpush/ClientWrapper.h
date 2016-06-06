//
//  ClientWrapper.h
//  ios_client_demo
//
//  Created by txj on 16/4/11.
//  Copyright © 2016年 gpush. All rights reserved.
//
//zk上查看连接数  get /Dev/CStatus/11035

#import <Foundation/Foundation.h>
typedef void (^handerMessage)(NSString * msg);
@interface ClientWrapper : NSObject
+ (instancetype)shared;

@property (strong,nonatomic) handerMessage hmsg;
@property (strong,nonatomic) BOOL is_connect;

-(void)initClientWithHander:(handerMessage)h;
-(int)login:(NSString *)anSrvip srvport:(int)anSrvport cid:(NSString *)anCid  version:(NSString *)anVersion  token:(NSString *)anToken;
-(int)loginout:(NSString *)anCid;
@end
