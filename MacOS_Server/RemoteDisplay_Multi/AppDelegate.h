//
//  AppDelegate.h
//  RemoteDisplay_Multi
//
//  Created by Laurence Bank on 12/20/20.
//

#import <UIKit/UIKit.h>
#import "CoreBluetooth/CoreBluetooth.h"

// Disable UART code on iOS
//#if defined( TARGET_OS_MAC ) && !defined (TARGET_OS_IPHONE)
#import <IOKit/IOTypes.h>
#import <IOKit/serial/IOSerialKeys.h>
#import <IOKit/serial/ioss.h>
#import <IOKit/usb/IOUSBLib.h>
#import <IOKit/hid/IOHIDKeys.h>
#import <termios.h>
#import <sys/param.h>
#import <sys/filio.h>
#import <sys/ioctl.h>
//#endif

@interface AppDelegate : UIResponder <UIApplicationDelegate, CBPeripheralManagerDelegate>

@property (nonatomic, strong) CBPeripheralManager *peripheralManager;
@property (nonatomic, strong) CBMutableService *service;
@property (nonatomic, strong) CBMutableService *service2;
@end

