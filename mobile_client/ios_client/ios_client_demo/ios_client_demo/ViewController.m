//
//  ViewController.m
//  ios_client_demo
//
//  Created by txj on 16/4/11.
//  Copyright © 2016年 gpush. All rights reserved.
//

#import "ViewController.h"
#import <GpushSDK/ClientWrapper.h>
@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    ClientWrapper *c=[ClientWrapper shared];
  
    
    __weak typeof(self) weakSelf = self;
    [c initClientWithHander:^(NSString *msg) {
        NSLog(@"%@",msg);
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            //Update UI in UI thread here
            weakSelf.lbStatus.text=msg;
            
        });
        
    }];
    int i= [c login:@"192.168.200.16" srvport:13000 cid:@"testios" version:@"1.0" token:@"aaa"];
    NSLog(@"%d",i);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
