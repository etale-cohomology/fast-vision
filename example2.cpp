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
