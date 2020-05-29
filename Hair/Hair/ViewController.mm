//
//  ViewController.m
//  hair
//
//  Created by Apple on 1/30/19.
//  Copyright © 2019 Apple. All rights reserved.
//
#import <opencv2/core/core.hpp>
#import <opencv2/imgproc/imgproc.hpp>
#import "ViewController.h"
#import "UIImage+Resize.h"
#import "HairChange/HairLib.h"
//#include "HairProcess.h"
#import "UIImage+OpenCvConversion.h"

@interface ViewController () <UIImagePickerControllerDelegate, UINavigationControllerDelegate>
{
    cv::Mat _cvImgDis;
    cv::Mat _cvImgMask;
}
@property (weak, nonatomic) IBOutlet UIImageView *imgResult;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSString * resourcePath = [[NSBundle mainBundle] resourcePath];
    //NSString *resourcePath = [[NSBundle mainBundle] bundlePath];
    NSString *dataPath = [resourcePath stringByAppendingPathComponent:@"Data"];
    char *pszDataPath = (char*)[resourcePath UTF8String];
    HairInit(pszDataPath);
}


- (IBAction)onTake:(id)sender {
    UIImagePickerController* vc = [[UIImagePickerController alloc] init];
    vc.delegate = self;
    vc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    [self presentViewController:vc animated:YES completion:NULL];
}

- (CGContextRef) newBitmapRGBA8ContextFromImage:(CGImageRef) image {
    CGContextRef context = NULL;
    CGColorSpaceRef colorSpace;
    uint32_t *bitmapData;
    
    size_t bitsPerPixel = 32;
    size_t bitsPerComponent = 8;
    size_t bytesPerPixel = bitsPerPixel / bitsPerComponent;
    
    size_t width = CGImageGetWidth(image);
    size_t height = CGImageGetHeight(image);
    
    size_t bytesPerRow = width * bytesPerPixel;
    size_t bufferLength = bytesPerRow * height;
    
    colorSpace = CGColorSpaceCreateDeviceRGB();
    
    if(!colorSpace) {
        NSLog(@"Error allocating color space RGB\n");
        return NULL;
    }
    
    // Allocate memory for image data
    bitmapData = (uint32_t *)malloc(bufferLength);
    
    if(!bitmapData) {
        NSLog(@"Error allocating memory for bitmap\n");
        CGColorSpaceRelease(colorSpace);
        return NULL;
    }
    
    //Create bitmap context
    
    context = CGBitmapContextCreate(bitmapData,
                                    width,
                                    height,
                                    bitsPerComponent,
                                    bytesPerRow,
                                    colorSpace,
                                    kCGImageAlphaPremultipliedLast);    // RGBA
    if(!context) {
        free(bitmapData);
        NSLog(@"Bitmap context not created");
    }
    
    CGColorSpaceRelease(colorSpace);
    
    return context;
}

- (void) convertUIImageToBitmapRGBA8:(UIImage *) image newBitmap:(unsigned char *)newBitmap{
    
    CGImageRef imageRef = image.CGImage;
    
    // Create a bitmap context to draw the uiimage into
    CGContextRef context = [self newBitmapRGBA8ContextFromImage:imageRef];
    
    if(!context) {
        return;
    }
    
    size_t width = CGImageGetWidth(imageRef);
    size_t height = CGImageGetHeight(imageRef);
    
    CGRect rect = CGRectMake(0, 0, width, height);
    
    // Draw image into the context to get the raw image data
    CGContextDrawImage(context, rect, imageRef);
    
    // Get a pointer to the data
    unsigned char *bitmapData = (unsigned char *)CGBitmapContextGetData(context);
    
    // Copy the data and release the memory (return memory allocated with new)
    size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
    size_t bufferLength = bytesPerRow * height;
    
    
    if(bitmapData) {
        //newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * bytesPerRow * height);
        
        if(newBitmap) {    // Copy the data
            int nIndex = 0;
            for(int i = 0; i < bufferLength / 4; ++i) {
                //if (i % 4 != 3)
                {
                    
                    newBitmap[i * 3 + 0] = bitmapData[i * 4 + 0];
                    newBitmap[i * 3 + 1] = bitmapData[i * 4 + 1];
                    newBitmap[i * 3 + 2] = bitmapData[i * 4 + 2];
                    nIndex ++;
                }
            }
        }
        
        free(bitmapData);
        
    } else {
        NSLog(@"Error getting bitmap pixel data\n");
    }
    
    CGContextRelease(context);
    
    return;
}

- (UIImage *) convertBitmapRGBA8ToUIImage:(unsigned char *) buffer
                                withWidth:(int) width
                               withHeight:(int) height {
    
    
    size_t bufferLength = width * height * 4;
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, bufferLength, NULL);
    size_t bitsPerComponent = 8;
    size_t bitsPerPixel = 32;
    size_t bytesPerRow = 4 * width;
    
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    if(colorSpaceRef == NULL) {
        NSLog(@"Error allocating color space");
        CGDataProviderRelease(provider);
        return nil;
    }
    
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    
    CGImageRef iref = CGImageCreate(width,
                                    height,
                                    bitsPerComponent,
                                    bitsPerPixel,
                                    bytesPerRow,
                                    colorSpaceRef,
                                    bitmapInfo,
                                    provider,    // data provider
                                    NULL,        // decode
                                    YES,            // should interpolate
                                    renderingIntent);
    
    uint32_t* pixels = (uint32_t*)malloc(bufferLength);
    
    if(pixels == NULL) {
        NSLog(@"Error: Memory not allocated for bitmap");
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpaceRef);
        CGImageRelease(iref);
        return nil;
    }
    
    CGContextRef context = CGBitmapContextCreate(pixels,
                                                 width,
                                                 height,
                                                 bitsPerComponent,
                                                 bytesPerRow,
                                                 colorSpaceRef,
                                                 kCGImageAlphaPremultipliedLast);
    
    if(context == NULL) {
        NSLog(@"Error context not created");
        free(pixels);
    }
    
    UIImage *image = nil;
    if(context) {
        
        CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, width, height), iref);
        
        CGImageRef imageRef = CGBitmapContextCreateImage(context);
        
        // Support both iPad 3.2 and iPhone 4 Retina displays with the correct scale
        if([UIImage respondsToSelector:@selector(imageWithCGImage:scale:orientation:)]) {
            float scale = [[UIScreen mainScreen] scale];
            image = [UIImage imageWithCGImage:imageRef scale:scale orientation:UIImageOrientationUp];
        } else {
            image = [UIImage imageWithCGImage:imageRef];
        }
        
        CGImageRelease(imageRef);
        CGContextRelease(context);
    }
    
    CGColorSpaceRelease(colorSpaceRef);
    CGImageRelease(iref);
    CGDataProviderRelease(provider);
    
    if(pixels) {
        free(pixels);
    }
    return image;
}
#pragma mark - ImagePickerController Delegate
//#include "caffe/HairProcess.h"

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
//    char* strDataPath = "Users⁩/⁨sim/⁨Desktop/⁨Hair 2/data⁩";
//    initProcess(strDataPath);
    
    
    UIImage* image = [info objectForKey:UIImagePickerControllerOriginalImage];
    
    CGImageRef imgRef = image.CGImage;
    
    float ratioStrech = 1.0f;
    
    int nWidth = CGImageGetWidth(imgRef);
    int nHeight = CGImageGetHeight(imgRef);
    if(nWidth > 1024 || nHeight > 1024) {
        ratioStrech = nWidth > nHeight ? 1024 / (float)nWidth : 1024 / (float)nHeight;
    }
    nWidth = nWidth * ratioStrech;
    nHeight = nHeight * ratioStrech;
    CGSize  srcSize = CGSizeMake(nWidth, nHeight);
    image = [image resizedImageToSize:srcSize];
    imgRef = image.CGImage;
    srcSize = CGSizeMake(CGImageGetWidth(imgRef), CGImageGetHeight(imgRef));
    
    nWidth = srcSize.width;
    nHeight = srcSize.height;
    unsigned char* pImageIn = (unsigned char*)malloc(3 * nWidth * nHeight);
    [self convertUIImageToBitmapRGBA8:image newBitmap:pImageIn];
    //Mat srcImg = [UIImage toCvMat:image];
    cv::Mat srcImg(nHeight, nWidth, CV_8UC3);
    memcpy(srcImg.data, pImageIn, 3*nWidth*nHeight*sizeof(unsigned char));
    cv::Rect rt;
    cv::Mat cropImg, disImg;
    int ret = CropImage(srcImg, rt, cropImg, disImg);
    if(ret == -1){
        //NSLOG("failed to detect face!")
        return;
    }
    UIImage *imageLut = [UIImage imageNamed:@"LutFile.bmp"];
    cv::Mat cvImageLut = [UIImage toCvMat:imageLut];
    HairSegment(cropImg, cvImageLut, disImg);
    _cvImgMask = MaskImageTransfor(srcImg, disImg, rt);
    //HairColorTransform(disImg, MaskImg, disImg, 150, 150, 150);
    _cvImgDis = srcImg;
    UIImage* imageResult = [UIImage fromCvMat:_cvImgDis];
    self.imgResult.image = imageResult;

    [picker dismissViewControllerAnimated:YES completion:nil];
}

-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}
- (IBAction)onYellow:(id)sender {
    //FFFB00
    cv::Mat disImg = _cvImgDis;
    HairColorTransform(disImg, _cvImgMask, disImg, 193, 125, 70);
    UIImage* imageResult = [UIImage fromCvMat:disImg];
    self.imgResult.image = imageResult;
}
- (IBAction)onPurple:(id)sender {
    //942192
    cv::Mat disImg = _cvImgDis;
    HairColorTransform(disImg, _cvImgMask, disImg, 128, 41, 60);
    UIImage* imageResult = [UIImage fromCvMat:disImg];
    self.imgResult.image = imageResult;
}
- (IBAction)onGray:(id)sender {
    //EEEEEE
    cv::Mat disImg = _cvImgDis;
    HairColorTransform(disImg, _cvImgMask, disImg, 99, 106, 109);
    UIImage* imageResult = [UIImage fromCvMat:disImg];
    self.imgResult.image = imageResult;
}

@end
