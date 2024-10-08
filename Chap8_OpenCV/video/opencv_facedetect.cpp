#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#define FBDEV           "/dev/fb0"
#define CAMERA_COUNT    3
#define CAM_WIDTH       640
#define CAM_HEIGHT      480

using namespace cv;

const static char* cascade_name = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml";

int main(int argc, char **argv) {
    int fbfd;
    struct fb_var_screeninfo vinfo;

    unsigned char *buffer, *pfbmap;
    unsigned int x, y, i, j, screensize;
    VideoCapture vc(0);
    CascadeClassifier cascade;
    Mat frame(CAM_WIDTH, CAM_HEIGHT, CV_8UC3, Scalar(255));
    Point pt1, pt2;

    if(!cascade.load(cascade_name)) {
        perror("load()");
        return EXIT_FAILURE;
    }

    vc.set(CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
    vc.set(CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);

    fbfd = open(FBDEV, O_RDWR);
    if(fbfd == -1) {
        perror("open() : framebuffer device");
        return EXIT_FAILURE;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information.");
        return EXIT_FAILURE;
    }

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8.;
    pfbmap = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if((int)pfbmap == -1) {
        perror("mmap() : framebuffer device to memory");
        return EXIT_FAILURE;
    }

    memset(pfbmap, 0, screensize);

    for(int i=0; i<CAMERA_COUNT; i++) {
        int colors = vinfo.bits_per_pixel/8;
        long location = 0;
        int istride = frame.cols*colors;

        vc >> frame;

        Mat image(CAM_WIDTH, CAM_HEIGHT, CV_8UC1, Scalar(255));
        cvtColor(frame, image, COLOR_BGR2GRAY);
        equalizeHist(image, image);
    
        std::vector<Rect> faces;
        cascade.detectMultiScale(image, faces, 1.1, 2, 0, Size(30, 30));

        for(j=0; j<faces.size(); j++) {
            pt1.x = faces[j].x; pt2.x = (faces[j].x + faces[j].width);
            pt1.y = faces[j].y; pt2.y = (faces[j].y + faces[j].height);

            rectangle(frame, pt1, pt2, Scalar(255, 0, 0), 3, 8);
        }
    
        buffer = (uchar*)frame.data;
        
        for(y=0, location=0; y<frame.rows; y++) {
            for(x=0; x<vinfo.xres; x++) {
                if(x >= frame.cols) {
                    location += colors;
                    continue;
                }
                pfbmap[location++] = *(buffer+(y*frame.cols+x)*3+0);
                pfbmap[location++] = *(buffer+(y*frame.cols+x)*3+1);
                pfbmap[location++] = *(buffer+(y*frame.cols+x)*3+2);
                pfbmap[location++] = 0xFF;
            }
        }
    }

    munmap(pfbmap, screensize);

    close(fbfd);
    return 0;
}
