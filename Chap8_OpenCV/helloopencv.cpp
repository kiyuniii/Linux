#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>  // Mat ???? ???? ?? core.hpp? ??

using namespace cv;  // cv ?????? ?? ??
using namespace std;

int main() {
    Mat image(300, 400, CV_8UC1, Scalar(255));  // Scalar ?? ??
    imshow("Hello World!", image);
    waitKey(0);
    return 0;
}
