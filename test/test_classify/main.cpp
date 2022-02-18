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
    cv::resize(img, resizedimage, cv::Size(150, 150));
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
    void *picDevBuffer = nullptr;
    const std::string modelPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/model/resnet18.om";
    const std::string namesPath = "/data1/code/names.txt";
    size_t deviceId = 0;
    std::shared_ptr<Classify> dfg = std::make_shared<Classify>(modelPath, namesPath, deviceId);
    size_t batchSize = dfg->GetBatch();
    size_t devBufferSize = dfg->GetInputSize();
    aclError aclRet = aclrtMalloc(&picDevBuffer, devBufferSize, ACL_MEM_MALLOC_NORMAL_ONLY);
    if (aclRet != 0)
    {
        std::cout << "创建显存失败!!!!!!!!!!!" << std::endl;
        return 1;
    }
    for (int i = 0; i < 1;)
    {
        cv::Mat img = cv::Mat(150, 150, CV_8UC3, cv::Scalar::all(1));
        std::vector<cv::Mat> imgs;
        imgs.push_back(img);
        std::vector<std::vector<Predictioin>> resultT;

        MemcpyImgToDevice(img, picDevBuffer, devBufferSize);
        dfg->Precess(picDevBuffer, devBufferSize);
        // dfg->Precess(imgs);
        dfg->Classification(resultT);
        for (int i = 0; i < resultT.size(); i++)
        {
            std::cout << "result: " << resultT[i][0].first << " " << resultT[i][0].second << std::endl;
        }
    }

    return 0;
}