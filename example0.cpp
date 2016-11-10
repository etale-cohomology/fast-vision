// Compile using:
    // g++ example0.cpp -o example0 -lX11 -lXext `pkg-config opencv --cflags --libs` && ./example0

// This also could work:
    // g++ example0.cpp -o example0 -lX11 -lXext -Ofast -funroll-loops -mfpmath=both -march=native -m64 `pkg-config opencv --cflags --libs` && ./example0

// WARNING: This example requires OpenCV!

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
