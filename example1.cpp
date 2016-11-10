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
