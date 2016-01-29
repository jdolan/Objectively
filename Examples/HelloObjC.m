// HelloObjC.m

#import <Foundation/Foundation.h>

#include <Objectively.h>

int main(int argc, char **argv)
{
    Object *object = $(alloc(Object), init);

    release(object);
    
    String *string = str("Hello World!");
    
    NSLog(@"%@", [NSString stringWithUTF8String:string->chars]);
	
	release(string);
    
    return 0;
}