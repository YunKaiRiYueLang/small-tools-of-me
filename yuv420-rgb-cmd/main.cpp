/*
1920*1080 yuv420 planner
用海思pq工具从vo截取下的图测试转换可行。
*/

#include<opencv.hpp>
#include<fstream>
using namespace cv;
static void help(int argc) {
    if (argc == 1) {

        //printf("usage:\n\t yuv-rgb.exe <index 0 1 2> yuvfilename\n");
        printf("usage:\n\t yuv-rgb.exe yuvfilename\n");
        //printf("\n0:1920x1080 yuv420\n");
        exit(0);
    }
}

cv::Mat readYuv(const cv::String& pathname) {
    std::ifstream file(pathname, std::ios::binary);
    if (!file.is_open()) {
        perror(pathname.c_str());
        exit(0);
    }
    file.seekg(0, file.end);
    size_t size = file.tellg();
    file.seekg(0, file.beg);
    cv::Mat src;
    switch (size)
    {
    case 3110400://yuv420 1920x1080
    {
        int height = 1080 + 540;
        src = cv::Mat(1080 + 540, 1920,  CV_8UC1, cv::Scalar(0));
        file.read((char*)(src.data), size);
    }
    break;
    default:
        break;
    }
    return src;
}


int main(int argc, char* argv[]) {

    help(argc);
    cv::Mat src=readYuv(argv[1]);
    if (src.data == NULL) {
        perror(argv[1]);
        return 0;
    }
    cv::Mat dst;
    cv::cvtColor(src,dst,cv::COLOR_YUV2BGR_I420);
    cv::imwrite("dst.bmp",dst);
    return 0;
}

