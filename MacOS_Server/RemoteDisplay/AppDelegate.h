//
//  AppDelegate.h
//  RemoteDisplay
//
//  Created by Laurence Bank on 4/29/20.
//  Copyright © 2020 Laurence Bank. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CoreBluetooth/CoreBluetooth.h"
#import <Foundation/Foundation.h>
#import <IOKit/IOTypes.h>
#import <IOKit/serial/IOSerialKeys.h>
#import <IOKit/serial/ioss.h>
#import <termios.h>
#import <sys/param.h>
#import <sys/filio.h>
#import <sys/ioctl.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, CBPeripheralManagerDelegate>

@property (nonatomic, strong) CBPeripheralManager *peripheralManager;
@property (nonatomic, strong) CBMutableService *service;

@end

