//
//  AppDelegate.m
//  RemoteDisplay_Multi
//
//  Created by Laurence Bank on 12/20/20.
//

#import "AppDelegate.h"
#import "ViewController.h"

@interface AppDelegate ()

@end

static CBMutableCharacteristic *characteristic1, *characteristic2;
static dispatch_source_t readPollSource;
static CBMutableService *includedService;

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    self.peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:nil];
    return YES;
}


#pragma mark - UISceneSession lifecycle


- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}

- (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral
{
//    NSLog(@"peripheralManagerDidUpdateState: %d", (int)peripheral.state);
       NSString *stateString = nil;
       switch((int)peripheral.state)
       {
           case CBManagerStateResetting: stateString = @"The connection with the system service was momentarily lost, update imminent."; break;
           case CBManagerStateUnsupported: stateString = @"The platform doesn't support Bluetooth Low Energy."; break;
           case CBManagerStateUnauthorized: stateString = @"The app is not authorized to use Bluetooth Low Energy."; break;
           case CBManagerStatePoweredOff: stateString = @"Bluetooth is currently powered off."; break;
           case CBManagerStatePoweredOn: stateString = @"Bluetooth is currently powered on and available to use."; break;
           default: stateString = @"State unknown, update imminent."; break;
       }
    NSLog(@"%@", stateString);
    
        if (CBManagerStatePoweredOn == peripheral.state) {
        
//        NSData *zombie = [@"zombie" dataUsingEncoding:NSUTF8StringEncoding];
//        CBMutableDescriptor *descriptor1 = [[CBMutableDescriptor alloc] initWithType:[CBUUID UUIDWithString:CBUUIDClientCharacteristicConfigurationString] value:@"0"];
            // Our main data characteristic
        characteristic1 = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:@"0000fea1-1234-1000-8000-00805f9b34fb"] properties:CBCharacteristicPropertyWriteWithoutResponse  | CBCharacteristicPropertyWrite | CBCharacteristicPropertyRead | CBCharacteristicPropertyIndicate value:nil permissions:CBAttributePermissionsWriteable | CBAttributePermissionsReadable];
//        characteristic1.descriptors = @[descriptor1];

//        NSData *ghost = [@"ghost" dataUsingEncoding:NSUTF8StringEncoding];
            // Our name characteristic (sets the window name)
            // Needed to create this additional characteristic with an invalid UUID to make it work on ESP32
        characteristic2 = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:@"0000fea1-0000-1000-8000-00805f9b34fb"] properties:CBCharacteristicPropertyWriteWithoutResponse | CBCharacteristicPropertyWrite | CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsWriteable | CBAttributePermissionsReadable];

//        characteristic3 = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:@"0000fea3-0000-1000-8000-00805f9b34fb"] properties:CBCharacteristicPropertyWriteWithoutResponse | CBCharacteristicPropertyWrite | CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsWriteable | CBAttributePermissionsReadable];

//        includedService = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:@"0000FEA0-0000-1000-8000-00805F9B34FB"] primary:YES];
//        includedService.characteristics = @[includedCharacteristic];

//            [self.peripheralManager addService:includedService];
            
//            self.service = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:@"0xFEA0"] primary:YES];
        self.service = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:@"0000fea0-1234-1000-8000-00805f9b34fb"] primary:YES];
        self.service.characteristics = @[characteristic1, characteristic2];
//        [self.peripheralManager addService:self.service];
            
//            self.service2 = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:@"0000fea0-0000-1000-8000-00805f9b34fb"] primary:NO];
//                self.service2.characteristics = @[characteristic2];
//            [self.peripheralManager addService:self.service2];
            
//            self.service.includedServices = @[self.service2];
            
            [self.peripheralManager addService:self.service];
            
            [peripheral startAdvertising:@{
                                           CBAdvertisementDataLocalNameKey: @"RemoteDisplay",
                                           CBAdvertisementDataServiceUUIDsKey: @[[CBUUID UUIDWithString:@"0000fea0-1234-1000-8000-00805f9b34fb"]]
                          }];

        } else {
        [peripheral stopAdvertising];
        [peripheral removeAllServices];
    }
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral
didReceiveReadRequest:(CBATTRequest *)request;
{
    request.value = [self makeButtonPayload];
    [peripheral respondToRequest:request withResult:CBATTErrorSuccess];
//    NSLog(@"Got a read request");
}
- (NSData *)makeButtonPayload
{
    NSMutableData *payload = [NSMutableData dataWithLength:2];
    uint8_t *bytes = [payload mutableBytes];
    bytes[0] = [ViewController getButtons];
    bytes[1] = 0;
    return payload;
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral
didReceiveWriteRequests:(NSArray<CBATTRequest *> *)requests;
{
    for (CBATTRequest *request in requests) {
        NSData *data = [request value];
        if (request.characteristic.UUID == characteristic1.UUID || request.characteristic.UUID == characteristic2.UUID)
        {
            [ViewController processBytes:data];
        }
//        else
//        { // set the label name from this one
//            NSString *clientName = [[NSString alloc]  initWithBytes:[data bytes]
//            length:[data length] encoding: NSUTF8StringEncoding];
//            [ViewController showText:clientName];
//        }
        // This function can handle writes with no response and writes which require a response
        // if no response is given, the client's waiting will time out / error
        // We always give a successful response to our characteristic for clients
        // which require a response
//        if (request.characteristic.UUID != characteristic1.UUID)
            [self.peripheralManager respondToRequest:request  withResult:CBATTErrorSuccess];
    }
}
- (void)peripheralManager:(CBPeripheralManager *)peripheral
                  central:(CBCentral *)central
didSubscribeToCharacteristic:(CBCharacteristic *)characteristic;
{
    NSLog(@"didSubscribeToCharacteristic");
}
- (void)peripheralManager:(CBPeripheralManager *)peripheral
                  central:(CBCentral *)central
didUnsubscribeFromCharacteristic:(CBCharacteristic *)characteristic;
{
    NSLog(@"didUnsubscribeFromCharacteristic");
}
- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(NSError *)error
{
    NSLog(@"peripheralManagerDidStartAdvertising: %@", error);
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral didAddService:(CBService *)service error:(NSError *)error
{
    NSLog(@"peripheralManagerDidAddService: %@ %@", service, error);
}


@end
