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

//#if defined( TARGET_OS_MAC ) && !defined (TARGET_OS_IPHONE)
static int serialFileDescriptor = -1;
static dispatch_source_t readPollSource;
static char bsdPath[MAXPATHLEN];

//#endif

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    self.peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:nil];
    if ([self findModems])
        [self openPort];
    
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

//#if !defined (TARGET_OS_IPHONE)

// UART code

static bool getModemPath(io_iterator_t serialPortIterator, char *thePath, CFIndex maxPathSize)
{
    io_object_t        modemService;
    Boolean            modemFound = false;
    
    // Initialize the returned path
    *thePath = '\0';
    
    // Iterate across all modems found. In this example, we bail after finding the first modem.
    
    while ((modemService = IOIteratorNext(serialPortIterator)) && !modemFound) {
        CFTypeRef    bsdPathAsCFString;
        
        // Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
        // used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
        // incoming calls, e.g. a fax listener.
        
        bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
                                                            CFSTR(kIOCalloutDeviceKey),
                                                            kCFAllocatorDefault,
                                                            0);
        if (bsdPathAsCFString) {
            Boolean result;
            
            // Convert the path from a CFString to a C (NUL-terminated) string for use
            // with the POSIX open() call.
            
            result = CFStringGetCString(bsdPathAsCFString,
                                        thePath,
                                        maxPathSize,
                                        kCFStringEncodingUTF8);
            CFRelease(bsdPathAsCFString);
            
            if (strncmp(thePath, "/dev/cu.usbmodem", 16) == 0 || strncmp(thePath, "/dev/cu.usbserial", 17) == 0) {
                NSLog(@"Found a serial port!");
                modemFound = true;
            }
            
            if (result) {
                printf("Modem found with BSD path: %s", thePath);
            }
        }
        
        printf("\n");
        
        // Release the io_service_t now that we are done with it.
        
        (void) IOObjectRelease(modemService);
    }
    
    return modemFound;
} /* getModemPath() */

-(bool) findModems
{
    kern_return_t            kernResult;
    CFMutableDictionaryRef    classesToMatch;
    io_iterator_t    serialPortIterator;

    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    
    if (classesToMatch == NULL) {
        NSLog(@"IOServiceMatching returned a NULL dictionary.");
    }
    else {
        // Look for devices that claim to be modems.
        CFDictionarySetValue(classesToMatch,
                             CFSTR(kIOSerialBSDTypeKey),
                             CFSTR(kIOSerialBSDAllTypes));
    }
    // Get an iterator across all matching devices.
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, &serialPortIterator);
    if (KERN_SUCCESS != kernResult) {
        NSLog(@"IOServiceGetMatchingServices returned %d\n", kernResult);
        goto exit;
    }
    if (!(getModemPath(serialPortIterator, bsdPath, sizeof(bsdPath))))
        kernResult = KERN_FAILURE;
    
    IOObjectRelease(serialPortIterator);    // Release the iterator.
exit:
    return (kernResult == KERN_SUCCESS);

} /* findModems() */

-(void)openPort
{
    int result;
   speed_t baudRate;
   baudRate = 115200;
//   NSString *serialPortFile = @"/dev/cu.wchusbserial1410";
//   const char *bsdPath = [serialPortFile cStringUsingEncoding:NSUTF8StringEncoding];
    serialFileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_EXLOCK); // | O_NONBLOCK );
    if (serialFileDescriptor < 1)
    {
        NSLog(@"Error opening serial port");
        return;
    }
    NSLog(@"Opened serial port successfully");
    
    // Now that the device is open, clear the O_NONBLOCK flag so subsequent I/O will block.
    // See fcntl(2) ("man 2 fcntl") for details.
    fcntl(serialFileDescriptor, 0);
    struct termios options;
    
    tcgetattr(serialFileDescriptor, &options);
    
    cfmakeraw(&options);
    options.c_cc[VMIN] = 1; // Wait for at least 1 character before returning
    options.c_cc[VTIME] = 1; // Wait 100 milliseconds between bytes before returning from read
    // Set 8 data bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8; // 8 data bits
    options.c_cflag |= HUPCL; // Turn on hangup on close
    options.c_cflag |= CLOCAL; // Set local mode on
    options.c_cflag |= CREAD; // Enable receiver
    options.c_lflag &= ~(ICANON /*| ECHO*/ | ISIG); // Turn off canonical mode and signals
    
    // Set baud rate
    result = cfsetspeed(&options, baudRate);
    
    result = tcsetattr(serialFileDescriptor, TCSANOW, &options);
    if (result != 0) {
            // Try to set baud rate via ioctl if normal port settings fail
            int new_baud = (int)baudRate;
            result = ioctl(serialFileDescriptor, IOSSIOSPEED, &new_baud, 1);
        }
    // Start a read dispatch source in the background
    readPollSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, serialFileDescriptor, 0, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
    dispatch_source_set_event_handler(readPollSource, ^{
                
        // Data is available
        static unsigned char buf[1024];
        static int iOldData = 0;
        long lengthRead = read(serialFileDescriptor, &buf[iOldData], sizeof(buf)-iOldData);
        iOldData += (int)lengthRead;
        // Check for a data sync error. If we miss the beginning of the packet
        // we have to scan forward until we find valid data
        if (buf[0] < 2 || buf[0] > 130 || (buf[1] != 0x00 && buf[1] != 0x40)) { // sync error
            int bInvalid = 1;
            int iLen, i = 1;
            while (i < iOldData-2 && bInvalid) { // search forward to sync
                if (buf[i] >= 2 && buf[i] <= 130 && (buf[i+1] == 0x00 || buf[i+1] == 0x40)) { // test a candidate
                    iLen = buf[i];
                    if (i+iLen+2 < iOldData && buf[i+iLen+1] >= 2 && buf[i+iLen+1] <= 130 && (buf[i+iLen+2] == 0x00 || buf[i+iLen+2])) { // looks good
                        memcpy(buf, &buf[i], iOldData-i); // slide the good data down
                        iOldData -= i;
                        bInvalid = 0; // mark it as OK to proceed
                    }
                }
                i++;
            } // while
            if (bInvalid) // we can't resync, leave
                return;
        }
        while (iOldData > 0 && iOldData >= buf[0]+1 && buf[0] >= 2 && buf[0] <= 130 && (buf[1] == 0x00 || buf[1] == 0x40)) // enough data + valid to send at least 1 packet?
        {
            int iPacketLength = buf[0];
            NSData *readData = [NSData dataWithBytes:&buf[1] length:iPacketLength];
            [ViewController processBytes:readData];
            iOldData -= (iPacketLength+1);
            memcpy(buf, &buf[iPacketLength+1], iOldData);
        }
    });
    dispatch_source_set_cancel_handler(readPollSource, ^{
        // Set port back the way it was before we used it
//        tcsetattr(serialFileDescriptor, TCSADRAIN, &originalPortAttributes);
        NSLog(@"Closing port.\n");
        close(serialFileDescriptor);
    });
    dispatch_resume(readPollSource);
//    self.readPollSource = readPollSource;

} /* openPort() */
//#endif

@end
