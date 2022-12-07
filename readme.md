# FastVision

__FastVision__ allows access to the screen's pixel values at __thousands of fps__ on platforms running the [X Window System](https://en.wikipedia.org/wiki/X_Window_System) (eg. [Linux](https://www.kernel.org/)).

This is useful to run computer vision algorithms in real time on the screen's contents, although it can also be used to take screenshots or record video as a sequence of screenshots. (Note that "the display's contents" can be a virtual machine running (say) Windows, and in that way it's possible to get the pixels of programs that run only on (say) Windows.)


## Features

FastVision is fast, barebones, and header-only.

It's available as a C++ header file (using an interface by StackOverflow user [abc](http://stackoverflow.com/users/3993187/abc)) or as raw C code. It allows access to rectangular subsets of the screen (including the full screen, and all screens in a multi-display setup).


## Examples

Simplest example: visualize one frame on the screen. __WARNING__: requires OpenCV!

```c++
// Compile using:
    // g++ example0.cpp -o example0 -lX11 -lXext `pkg-config opencv --cflags --libs` && ./example0

#include "fast_vision.hpp"

// This includes most headers! Use only for testing!
#include <opencv2/opencv.hpp>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


int main(){
    // Set this to your screen's resolution!
    const uint WIDTH = 1920>>1, HEIGHT = 1080>>1;

    cv::Mat img;

    // Connect to the X server and create a shared memory segment!
    // This will capture the main screen starting at the upper left corner
    FastVision screen(0, 0, WIDTH, HEIGHT);

    for(uint i;; ++i){
        double start = clock();

        // This function returns a pointer to the shared memory segment!
        char* raw_bytes = screen.get_screen();
        img = cv::Mat(HEIGHT, WIDTH, CV_8UC4, raw_bytes);

        if(!(i & 0b111111))
            printf("fps %4.f  spf %.4f\n", FPS(start), 1 / FPS(start));
        break;
    }

    cv::imshow("img", img);
    cv::waitKey(0);
}
```


Another example: __silently__ query the screen as fast as possible!

```c++

// Compile using:
    // g++ example1.cpp -o example1 -lX11 -lXext && ./example1

#include "fast_vision.hpp"

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


int main(){
    // Set this to your screen's resolution!
    const uint WIDTH = 1920>>1, HEIGHT = 1080>>1;

    // Connect to the X server and create a shared memory segment!
    // This will capture the main screen starting at the upper left corner
    FastVision screen(0, 0, WIDTH, HEIGHT);

    for(uint i;; ++i){
        double start = clock();

        // This function returns a pointer to the shared memory segment!
        char* raw_bytes = screen.get_screen();

        if(!(i & 0b111111)){
            printf("fps %4.f  spf %.4f  ", FPS(start), 1 / FPS(start));
            printf("First byte 0x%x  Another byte 0x%x\n",
                   raw_bytes[0] & 0xff, raw_bytes[WIDTH*HEIGHT] & 0xff);
        }
    }

}
```


Yet another example: __visualize__ the screen in a window! __WARNING__: requires OpenCV!

```c++
// Compile using:
    // g++ example2.cpp -o example2 -lX11 -lXext `pkg-config opencv --cflags --libs` && ./example2

// WARNING: This example requires OpenCV!

#include "fast_vision.hpp"

// This includes most headers! Use only for testing!
#include <opencv2/opencv.hpp>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))


int main(){
    puts("Press x to exit!\n");

    // Set this to your screen's resolution!
    const uint WIDTH = 1920/3, HEIGHT = 1080/3;
    int key, i;

    cv::Mat img;
    cv::namedWindow("img", cv::WINDOW_NORMAL | cv::WINDOW_OPENGL);
    cv::moveWindow("img", WIDTH, 50);
    cv::resizeWindow("img", WIDTH, HEIGHT);

    // Connect to the X server and create a shared memory segment!
    // This will capture the main screen starting at the upper left corner
    FastVision screen(0, 0, WIDTH, HEIGHT);

    while(key != 120){
        double start = clock();
        ++i;

        // This function returns a pointer to the shared memory segment!
        char* raw_bytes = screen.get_screen();
        img = cv::Mat(HEIGHT, WIDTH, CV_8UC4, raw_bytes);

        if(!(i & 0b111111)){
            printf("fps %4.f  spf %.4f\n", FPS(start), 1 / FPS(start));
            i = 0;
        }

        cv::imshow("img", img);
        key = cv::waitKey(1);
    }

}
```


## How

Such performance is possible thanks to the [MIT Shared Memory Extension](https://www.x.org/releases/X11R7.7/doc/xextproto/shm.html), which provides [shared memory](https://en.wikipedia.org/wiki/Shared_memory) [XImages](http://rpm.pbone.net/index.php3/stat/45/idpl/1646374/numer/1/nazwa/ximage). An __XImage__ is an X _client side_ storage mechanism for an X __pixel map__.

Using "vanilla" X, at most 180 fps were achieved with the same hardware. This repo also includes such "vanilla" implementation, for reference!


## Why

We wanted access to the screen's pixel values as fast as possible to run computer vision algorithms on the screen's contents, but neither the  [X](ftp://www.x.org/pub/current/doc/libX11/libX11/libX11.html) interface (which provides the [XGetImage](https://linux.die.net/man/3/xgetimage) function) nor the more recent [xcb](https://xcb.freedesktop.org/) interface gave satisfactory results, even with multi-threading and SIMD extensions.


## Dependencies

[X Window System](https://en.wikipedia.org/wiki/X_Window_System) (UNIX-like platforms)  
[MIT Shared Memory Extension](https://www.x.org/releases/X11R7.7/doc/xextproto/shm.html) (an extension to the X Window System)  
[OpenCV](https://github.com/opencv/opencv) (Optional). To visualize your screen as a video feed, and to run computer vision stuff  

To quote [from](https://www.x.org/releases/X11R7.7/doc/xextproto/shm.html):

_To find out if your server supports the extension, use xdpyinfo(1). In particular, to be able to use this extension, your system must provide the SYSV shared memory primitives. There is not an mmap-based version of this extension._
