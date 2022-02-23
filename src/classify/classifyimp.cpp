#include "classify.h"
namespace ASCEND_VIRGO
{
    ClassifyPrivate::ClassifyPrivate(const std::string &model_path, const std::string &name_Path, size_t deviceId)
    {

        // deviceId_ = deviceId;
        modelPath = model_path;
        namesPath = name_Path;
        modelProcess = std::make_shared<ModelProcess>();
        std::ifstream fin(namesPath, std::ios::in);
        char line[1024] = {0};
        std::string name = "";
        while (fin.getline(line, sizeof(line)))
        {
            std::stringstream word(line);
            word >> name;
            std::cout << "name: " << name << std::endl;
            labels.push_back(name);
        }
        fin.clear();
        fin.close();

        testFile = {
            "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/data/test.bin"};

        Result ret = InitResource();
    }
    size_t ClassifyPrivate::GetInputSize()
    {
        return this->devBufferSize;
    }
    Result ClassifyPrivate::InitResource()
    {

        // create stream
        aclError ret = aclrtCreateStream(&stream_);
        if (ret != ACL_SUCCESS)
        {
            // ERROR_LOG("acl create stream failed, deviceId = %d, errorCode = %d",
            //   deviceId_, static_cast<int32_t>(ret));
            return FAILED;
        }
        INFO_LOG("create stream success");
        aclrtRunMode runMode;
        ret = aclrtGetRunMode(&runMode);
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl get run mode failed, errorCode = %d", static_cast<int32_t>(ret));
            return FAILED;
        }
        g_isDevice = (runMode == ACL_DEVICE);
        INFO_LOG("get run mode success");

        // const char *omModelPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/model/resnet18.om";
        std::cout << "modelPath: " << modelPath << std::endl;
        ret = modelProcess->LoadModel(modelPath.c_str());
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute LoadModel failed");
            return FAILED;
        }
        ret = modelProcess->CreateModelDesc();
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute CreateModelDesc failed");
            return FAILED;
        }
        ret = modelProcess->GetInputSizeByIndex(0, devBufferSize);
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute GetInputSizeByIndex failed");
            return FAILED;
        }

        aclError aclRet = aclrtMalloc(&picDevBuffer, devBufferSize, ACL_MEM_MALLOC_NORMAL_ONLY);
        if (aclRet != ACL_SUCCESS)
        {
            ERROR_LOG("malloc device buffer failed. size is %zu, errorCode is %d",
                      devBufferSize, static_cast<int32_t>(aclRet));
            return FAILED;
        }

        ret = modelProcess->CreateInput(picDevBuffer, devBufferSize);
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute CreateInput failed");
            aclrtFree(picDevBuffer);
            return FAILED;
        }
        ret = modelProcess->CreateOutput();
        if (ret != SUCCESS)
        {
            aclrtFree(picDevBuffer);
            ERROR_LOG("execute CreateOutput failed");
            return FAILED;
        }
        INFO_LOG("initial memory success");
        return SUCCESS;
    }

    ClassifyPrivate::~ClassifyPrivate()
    {
        modelProcess->DestroyInput();
        modelProcess->DestroyOutput();

        aclrtFree(picDevBuffer);
    }
    void ClassifyPrivate::Classification(std::vector<std::vector<Predictioin>> &result)
    {
        result.clear();
        aclError ret;

        // aclmdlDataset *modelInput = modelProcess->GetInputData();
        // Utils::print(modelInput);

        ret = modelProcess->Execute();

        if (ret != SUCCESS)
        {
            ERROR_LOG("execute inference failed");
            aclrtFree(picDevBuffer);
            // return FAILED;
        }
        std::vector<std::vector<float>> tmpFloat;
        modelProcess->OutputModelResult(tmpFloat);
        for (int i = 0; i < tmpFloat.size(); i++)
        {
            std::vector<Predictioin> tmpResult;
            int maxValue = tmpFloat[i][0];
            int index = 0;
            for (int j = 0; j < tmpFloat[i].size(); j++)
            {
                if (tmpFloat[i][j] > maxValue)
                {
                    maxValue = tmpFloat[i][j];
                    index = j;
                }
            }
            tmpResult.push_back(std::make_pair(labels[index], prob_sigmoid(maxValue)));
            result.push_back(tmpResult);
        }
    }
    void ClassifyPrivate::Precess(void *pDevbuff, size_t iLength)
    {
        INFO_LOG("start dev preprocess");
        aclError ret;
        if (iLength != devBufferSize)
        {
            INFO_LOG(" dev size error.");
        }
        ret = Utils::MemcpyDeviceToDeviceBuffer(pDevbuff, picDevBuffer, devBufferSize);
    }
    void ClassifyPrivate::Precess(const std::vector<cv::Mat> &imgs)
    {

        aclError ret;

        // for (size_t index = 0; index < testFile.size(); ++index)
        // {
        INFO_LOG("start to process file, batch is :%d", imgs.size());
        // copy image data to device buffer
        // ret = Utils::MemcpyFileToDeviceBuffer(testFile[index], picDevBuffer, devBufferSize);
        cv::Mat tmp = imgs[0].clone();
        int modelInputWidth;
        int modelInputHeight;

        ret = modelProcess->GetModelInputWH(modelInputWidth, modelInputHeight);
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute GetModelInputWH failed");
            // return FAILED;
        }

        ret = Utils::MemcpyImgToDeviceBuffer(tmp, picDevBuffer, devBufferSize, modelInputWidth, modelInputHeight);
        if (ret != SUCCESS)
        {
            aclrtFree(picDevBuffer);
            ERROR_LOG("memcpy device buffer failed");
            // return FAILED;
        }
    }
    size_t ClassifyPrivate::GetBatch()
    {
        size_t inputNumber;
        aclError ret = modelProcess->GetInputSize(inputNumber);
        ;
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl init failed, errorCode = %d", static_cast<int32_t>(ret));
            return FAILED;
        }
        return inputNumber;
    }
}