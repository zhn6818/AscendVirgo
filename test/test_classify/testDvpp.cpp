#include <iostream>
#include <vector>
#include "ascend_virgo.h"
#include "acl/acl.h"
using namespace ASCEND_VIRGO;

size_t MemcpyImgToDevice(cv::Mat &bgr_mat, void *&picDevBuffer, size_t inputBuffSize)
{
    int height = bgr_mat.rows;
    int width = bgr_mat.cols;
    cv::Mat img_nv12;
    cv::Mat yuv_mat;
    cv::cvtColor(bgr_mat, yuv_mat, cv::COLOR_BGR2YUV_I420);
    // cv::cvtColor(bgr_mat, yuv_mat, cv::COLOR_BGR2YUV_YV12);
    uint8_t *yuv = yuv_mat.ptr<uint8_t>();
    img_nv12 = cv::Mat(height * 3 / 2, width, CV_8UC1);
    uint8_t *ynv12 = img_nv12.ptr<uint8_t>();

    int32_t uv_height = height / 2;
    int32_t uv_width = width / 2;

    // copy y data
    int32_t y_size = height * width;
    memcpy(ynv12, yuv, y_size);
    // memset(ynv12, 1, 50176);

    // copy uv data
    uint8_t *nv12 = ynv12 + y_size;
    uint8_t *u_data = yuv + y_size;
    uint8_t *v_data = u_data + uv_height * uv_width;

    for (int32_t i = 0; i < uv_width * uv_height; i++)
    {
        *nv12++ = *u_data++;
        *nv12++ = *v_data++;
    }
    // int32_t yuv_size = y_size + 2 * uv_height * uv_width;
    int32_t yuv_size = y_size + 2 * uv_height * uv_width;
    FILE *yuvFd = fopen("1.yuv", "w+");
    fwrite(img_nv12.ptr<uint8_t>(), 1, yuv_size, yuvFd);
    fclose(yuvFd);

    void *inputBuff = nullptr;
    uint32_t fileSize = y_size + 2 * uv_height * uv_width;

    // uint8_t *pp = ynv12;
    // for (int i = 50100; i < 50200; i++)
    // {
    //     std::cout << int(*(pp + i)) << "  ";
    // }
    // std::cout << std::endl;
    // auto ret = 0;

    if (inputBuffSize != static_cast<size_t>(fileSize))
    {

        std::cout << "!!!!!!!       size error      !!!!!!!!!!!!" << std::endl;
        return 1;
    }
    
    aclError aclRet = aclrtMemcpy(picDevBuffer, inputBuffSize, img_nv12.ptr<uint8_t>(), inputBuffSize, ACL_MEMCPY_HOST_TO_DEVICE);
    if (aclRet != 0)
    {
        (void)aclrtFreeHost(inputBuff);
        return 1;
    }
    (void)aclrtFreeHost(inputBuff);
    return 0;
}
void bgr2nv12(cv::Mat &bgr_mat)
{
}

int main(int argc, char **argv)
{
    int32_t deviceId_ = 0;
    aclrtContext context_;
    void *picDevBuffer = nullptr;
    const std::string modelPath = "/data1/resnet18_classify_aipp_bgr.om";
    const std::string namesPath = "/data1/code/names.txt";
    // size_t deviceId = 0;

    aclError ret = aclInit(nullptr);
    if (ret != ACL_SUCCESS)
    {
        std::cout << "acl init failed, errorCode " << static_cast<int32_t>(ret) << std::endl;
        // return FAILED;
    }
    std::cout << "acl init success" << std::endl;
    ret = aclrtSetDevice(deviceId_);
    if (ret != ACL_SUCCESS)
    {
        std::cout << "acl set device %d failed, errorCode = %d" << deviceId_ << static_cast<int32_t>(ret) << std::endl;
        // return FAILED;
    }
    std::cout << "set device %d success" << deviceId_ << std::endl;

    // create context (set current)
    ret = aclrtCreateContext(&context_, deviceId_);
    if (ret != ACL_SUCCESS)
    {
        std::cout << "acl create context failed, deviceId = %d, errorCode = %d" << deviceId_ << static_cast<int32_t>(ret) << std::endl;
        // return FAILED;
    }
    std::cout << "create context success" << std::endl;
    int deviceId = 0;
    std::shared_ptr<Classify> dfg = std::make_shared<Classify>(modelPath, namesPath, deviceId);
    // std::shared_ptr<Classify> dfg2 = std::make_shared<Classify>(modelPath, namesPath, deviceId);
    size_t batchSize = dfg->GetBatch();
    size_t devBufferSize = dfg->GetInputSize();
    aclError aclRet = aclrtMalloc(&picDevBuffer, devBufferSize, ACL_MEM_MALLOC_NORMAL_ONLY);
    if (aclRet != 0)
    {
        std::cout << "创建显存失败!!!!!!!!!!!" << std::endl;
        return 1;
    }
    cv::Mat img = cv::Mat(224, 224, CV_8UC3, cv::Scalar::all(1));
    // cv::Mat img = cv::imread("/data1/cxj2/1.jpg");
    // cv::Mat img = cv::imread("/data1/code/test.jpg");
    cv::resize(img, img, cv::Size(224, 224));
    // bgr2nv12(img);
    // cv::Mat img = cv::imread("/data1/cxj2/1.jpg");
    // cv::resize(img, img, cv::Size(224, 224));
    // std::cout << "img: " << (int)img.at<cv::Vec3b>(0, 0)[0] << " " << (int)img.at<cv::Vec3b>(0, 0)[1] << " " << (int)img.at<cv::Vec3b>(0, 0)[2] << std::endl;
    std::vector<cv::Mat> imgs;
    imgs.push_back(img);
    std::vector<std::vector<Predictioin>> resultT;

    for (int i = 0; i < 1;)
    {
        MemcpyImgToDevice(img, picDevBuffer, devBufferSize);
        dfg->Precess(picDevBuffer, devBufferSize);
        dfg->Classification(resultT);
        for (int i = 0; i < resultT.size(); i++)
        {
            std::cout << "result: " << resultT[i][0].first << " " << resultT[i][0].second << std::endl;
        }
    }

    return 0;
}