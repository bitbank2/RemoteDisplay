//
//  AppDelegate.m
//  RemoteDisplay
//
//  Created by Laurence Bank on 4/29/20.
//  Copyright © 2020 Laurence Bank. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"

static CBMutableCharacteristic *characteristic1, *characteristic2, *characteristic3;
//void resetDisplay(void);
static int serialFileDescriptor = -1;
static dispatch_source_t readPollSource;
static CBMutableService *includedService;
@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    self.peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:nil];
    // Start a timer to keep track of connection status
    // Start a timer to animate GIF/PNG
//    [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(timer:) userInfo:nil repeats:YES]; // set up a
//    resetDisplay(); // assume we set up the display
//    [self openPort];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

-(void) timer:(id) sender
{
    if (serialFileDescriptor == -1)
        [self openPort]; // start reading from the serial port
    else {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"showOLED"
        object:self];
    }
//    if (self.peripheralManager.state != CBPeripheralStateConnected)
//    {
//        NSLog(@"Not connected");
//    } else {
//        NSLog(@"Connected");
//    }
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
        characteristic1 = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:@"0xFEA1"] properties:CBCharacteristicPropertyWriteWithoutResponse  | CBCharacteristicPropertyWrite | CBCharacteristicPropertyRead | CBCharacteristicPropertyIndicate value:nil permissions:CBAttributePermissionsWriteable | CBAttributePermissionsReadable];
//        characteristic1.descriptors = @[descriptor1];

//        NSData *ghost = [@"ghost" dataUsingEncoding:NSUTF8StringEncoding];
            // Our name characteristic (sets the window name)
//        characteristic2 = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:@"0000fea2-0000-1000-8000-00805f9b34fb"] properties:CBCharacteristicPropertyWriteWithoutResponse | CBCharacteristicPropertyWrite | CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsWriteable | CBAttributePermissionsReadable];

//        characteristic3 = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:@"0000fea3-0000-1000-8000-00805f9b34fb"] properties:CBCharacteristicPropertyWriteWithoutResponse | CBCharacteristicPropertyWrite | CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsWriteable | CBAttributePermissionsReadable];

//        includedService = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:@"0000FEA0-0000-1000-8000-00805F9B34FB"] primary:YES];
//        includedService.characteristics = @[includedCharacteristic];

//            [self.peripheralManager addService:includedService];
            
        self.service = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:@"0xFEA0"] primary:YES];
            self.service.characteristics = @[characteristic1]; //, characteristic2, characteristic3];
//            self.service.includedServices = @[includedService];

        [self.peripheralManager addService:self.service];
            
            [peripheral startAdvertising:@{
                                           CBAdvertisementDataLocalNameKey: @"RemoteDisplay",
                                           CBAdvertisementDataServiceUUIDsKey: @[[CBUUID UUIDWithString:@"0xFEA0"]]
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
        if (request.characteristic.UUID == characteristic1.UUID || request.characteristic.UUID == characteristic3.UUID)
        {
            [ViewController processBytes:data];
        }
        else
        { // set the label name from this one
            NSString *clientName = [[NSString alloc]  initWithBytes:[data bytes]
            length:[data length] encoding: NSUTF8StringEncoding];
            [ViewController showText:clientName];
        }
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

// UART code

-(void)openPort
{
    int result;
   speed_t baudRate;
   baudRate = 9600;
   NSString *serialPortFile = @"/dev/cu.wchusbserial1410";
   const char *bsdPath = [serialPortFile cStringUsingEncoding:NSUTF8StringEncoding];
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
@end
