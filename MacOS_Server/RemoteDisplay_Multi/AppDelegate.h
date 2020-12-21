//
//  AppDelegate.h
//  RemoteDisplay_Multi
//
//  Created by Laurence Bank on 12/20/20.
//

#import <UIKit/UIKit.h>
#import "CoreBluetooth/CoreBluetooth.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate, CBPeripheralManagerDelegate>

@property (nonatomic, strong) CBPeripheralManager *peripheralManager;
@property (nonatomic, strong) CBMutableService *service;
@property (nonatomic, strong) CBMutableService *service2;
@end

