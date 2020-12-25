//
//  ViewController.h
//  RemoteDisplay_Multi
//
//  Created by Laurence Bank on 12/20/20.
//

#import <UIKit/UIKit.h>
#import "../../rd_constants.h"

@interface ViewController : UIViewController
@property (weak, nonatomic) IBOutlet UIButton *Button0;
@property (weak, nonatomic) IBOutlet UIButton *Button1;
@property (weak, nonatomic) IBOutlet UIButton *Button2;
@property (weak, nonatomic) IBOutlet UIImageView *theImage;
+ (void) processBytes:(NSData *)thedata;
+ (int) getButtons;
- (void) showOLED;
- (void) showOLEDNotification:(NSNotification *) notification;

@end

