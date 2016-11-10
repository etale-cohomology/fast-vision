#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

#include <sys/ipc.h>  // UNIX interprocess communication stuff
#include <sys/shm.h>  // UNIX shared memory stuff

#include <stdio.h>

struct FastVision{

    FastVision(uint x, uint y, uint width, uint height):
               x(x), y(y), width(width), height(height){

        display = XOpenDisplay(nullptr);
        root = DefaultRootWindow(display);

        XGetWindowAttributes(display, root, &window_attributes);
        screen = window_attributes.screen;
        ximg = XShmCreateImage(display, DefaultVisualOfScreen(screen), DefaultDepthOfScreen(screen),
                               ZPixmap, NULL, &shminfo, width, height);

        shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|0777);
        shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
        shminfo.readOnly = False;

        if(shminfo.shmid < 0)
            puts("Fatal shminfo error!");;

        Status s1 = XShmAttach(display, &shminfo);
        printf("XShmAttach() %s\n", s1 ? "success!" : "failure!");

        init = true;
    }

    ~FastVision(){
        if(!init)
            XDestroyImage(ximg);

        XShmDetach(display, &shminfo);
        shmdt(shminfo.shmaddr);
        XCloseDisplay(display);
    }

    char* get_screen(){
        if(init)
            init = false;
        XShmGetImage(display, root, ximg, 0, 0, 0x00ffffff);
        return ximg->data;
    }

    Display* display;
    Window root;
    XWindowAttributes window_attributes;
    Screen* screen;
    XImage* ximg;
    XShmSegmentInfo shminfo;

    int x, y, width, height;
    bool init;
};
