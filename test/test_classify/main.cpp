#include <iostream>
#include <vector>
#include "ascend_virgo.h"
#include "acl/acl.h"
using namespace ASCEND_VIRGO;

size_t MemcpyImgToDevice(cv::Mat &img, void *&picDevBuffer, size_t inputBuffSize)
{
    void *inputBuff = nullptr;
    uint32_t fileSize = img.cols * img.rows * img.channels() * sizeof(float);

    auto ret = 0;

    if (inputBuffSize != static_cast<size_t>(fileSize))
    {

        std::cout << "!!!!!!!       size error     !!!!!!!!!!!!" << std::endl;
        return 1;
    }
    cv::Mat resizedimage;
    cv::Mat imagef;
    cv::resize(img, resizedimage, cv::Size(224, 224));
    resizedimage.convertTo(imagef, CV_32FC3);
    float *imagetrans = (float *)malloc(inputBuffSize);
    int index = 0;
    int step = resizedimage.cols * resizedimage.rows;
    for (int i = 0; i < resizedimage.rows; i++)
    {
        for (int j = 0; j < resizedimage.cols; j++)
        {
            *(imagetrans + index) = imagef.at<cv::Vec3f>(i, j)[0];
            *(imagetrans + index + step) = imagef.at<cv::Vec3f>(i, j)[1];
            *(imagetrans + index + 2 * step) = imagef.at<cv::Vec3f>(i, j)[2];
            index++;
        }
    }
    aclError aclRet = aclrtMemcpy(picDevBuffer, inputBuffSize, imagetrans, inputBuffSize, ACL_MEMCPY_HOST_TO_DEVICE);
    if (aclRet != 0)
    {
        (void)aclrtFreeHost(inputBuff);
        return 1;
    }
    (void)aclrtFreeHost(inputBuff);
    return 0;
}

int main(int argc, char **argv)
{
    int32_t deviceId_ = 0;
    aclrtContext context_;
    void *picDevBuffer = nullptr;
    const std::string modelPath = "/data1/resnet18_classify.om";
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
    std::shared_ptr<Classify>
        dfg = std::make_shared<Classify>(modelPath, namesPath, deviceId);
    // std::shared_ptr<Classify> dfg2 = std::make_shared<Classify>(modelPath, namesPath, deviceId);
    size_t batchSize = dfg->GetBatch();
    size_t devBufferSize = dfg->GetInputSize();
    aclError aclRet = aclrtMalloc(&picDevBuffer, devBufferSize, ACL_MEM_MALLOC_NORMAL_ONLY);
    if (aclRet != 0)
    {
        std::cout << "??????????????????!!!!!!!!!!!" << std::endl;
        return 1;
    }
    cv::Mat img = cv::Mat(224, 224, CV_8UC3, cv::Scalar::all(1));
    // cv::Mat img = cv::imread("/data1/cxj2/1.jpg");
    // cv::resize(img, img, cv::Size(224, 224));
    std::cout << "img: " << (int)img.at<cv::Vec3b>(0, 0)[0] << " " << (int)img.at<cv::Vec3b>(0, 0)[1] << " " << (int)img.at<cv::Vec3b>(0, 0)[2] << std::endl;
    std::vector<cv::Mat> imgs;
    imgs.push_back(img);
    std::vector<std::vector<Predictioin>> resultT;

    for (int i = 0; i < 1;)
    {

        //
        MemcpyImgToDevice(img, picDevBuffer, devBufferSize);
        dfg->Precess(picDevBuffer, devBufferSize);
        // dfg->Precess(imgs);
        dfg->Classification(resultT);

        // MemcpyImgToDevice(img, picDevBuffer, devBufferSize);
        // dfg2->Precess(picDevBuffer, devBufferSize);
        // // dfg->Precess(imgs);
        // dfg2->Classification(resultT);
        for (int i = 0; i < resultT.size(); i++)
        {
            std::cout << "result: " << resultT[i][0].first << " " << resultT[i][0].second << std::endl;
        }
    }

    return 0;
}