// gcc raw.c -o raw -lX11 -lXext -Ofast -funroll-loops && ./raw
// gcc raw.c -o raw -lX11 -lXext -Ofast -funroll-loops -mfpmath=both -march=native -m64 -mavx2 && ./raw

// The MIT-SHM extension allows for shared-memory XImage objects
// This requires OS support for SYSV (System-V) shared memory, and X support for the MIT-SHM extension.
// Shared memory PIXMAPS can only be supported when the X server can use regular virtual memory for pixmap data; if
// the pixmaps are stored in some magic graphics hardware, you're out of luck. Xdpyinfo(1) gives NO information about this!

// Depth 16: each pixel has 16 bits. Red: 5 bits, green: 6 bits, blue: 5 bits. Total: 65536 colors


#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdio.h>
#include <stdint.h>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))

typedef unsigned int uint;

// Using one display DOESN'T improve performance! Querying a smaller subset of the screen DOES
#define WIDTH  1920>>1
#define HEIGHT 1080>>1

static const char FORMATS[3][9] = {"XYBitmap", "XYPixmap", "ZPixmap "};

#define XIMG_META(ximg) \
    do { \
    printf("w %u  h %u  depth %u  B/line %u  b/pixel %u  format %s  offset %u  bm unit %u  B %08lx  G %08lx  R %08lx\n", \
           ximg->width, ximg->height, ximg->depth, ximg->bytes_per_line, ximg->bits_per_pixel, FORMATS[ximg->format], \
           ximg->xoffset, ximg->bitmap_unit, ximg->blue_mask, ximg->green_mask, ximg->red_mask); \
    } while(0)


#define PRINT_XIMG(ximg) \
    do{ \
        for(uint i=0; i<((1920>>0)*(1080>>0)); i++) \
            printf(" %x", ximg->data[i]); \
        puts("\n"); \
    } while(0)



// -------------------------------------------------------
int main(){
    Display* display = XOpenDisplay(NULL);

    int ignore, major, minor;
    Bool pixmaps;
    if(XQueryExtension(display, "MIT-SHM", &ignore, &ignore, &ignore))
        if(XShmQueryVersion(display, &major, &minor, &pixmaps))
            printf("XShm extension v%d.%d %s shared pixmaps\n", major, minor, pixmaps ? "with" : "without");

    Window root = DefaultRootWindow(display);  // Macro to return the root window! It's a simple uint32
    XWindowAttributes window_attributes;
    XGetWindowAttributes(display, root, &window_attributes);
    Screen* screen = window_attributes.screen;
    XShmSegmentInfo shminfo;
    XImage* ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen), ZPixmap, NULL, &shminfo, WIDTH, HEIGHT);
    XIMG_META(ximg);

    shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|0777);
    shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;
    if(shminfo.shmid < 0)
        puts("Fatal shminfo error!");;
    Status s1 = XShmAttach(display, &shminfo);
    printf("XShmAttach() %s\n", s1 ? "success!" : "failure!");

    double start;
    for(uint i; ; i++){
        start = clock();

        XShmGetImage(display, root, ximg, 0, 0, 0x00ffffff);
        char* raw_bytes = ximg->data;

        break;
    }
    double fps = FPS(start), spf = 1 / FPS(start);

    PRINT_XIMG(ximg);
    XIMG_META(ximg);
    puts("Finished printing a single frame, as raw bytes!");

    XShmDetach(display, &shminfo);
    XDestroyImage(ximg);
    shmdt(shminfo.shmaddr);

    printf("fps %4.f  spf %.4f\n", fps, spf);

    puts("exit success!");

}
