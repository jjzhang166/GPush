//
//  ViewController.m
//  ios_client_demo
//
//  Created by txj on 16/4/11.
//  Copyright © 2016年 gpush. All rights reserved.
//

#import "ViewController.h"
#import "ClientWrapper.h"
@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    ClientWrapper *c=[ClientWrapper shared];
  
    [c initClientWithHander:^(NSString *msg) {
        NSLog(@"%@",msg);
    }];
    int i= [c login:@"192.168.200.153" srvport:13000 cid:@"test0001" version:@"1.0" token:@"aaa"];
    NSLog(@"%d",i);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
