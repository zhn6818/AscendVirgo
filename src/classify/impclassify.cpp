#include "classify.h"
namespace ASCEND_VIRGO
{
    ClassifyPrivate::ClassifyPrivate(const std::string &model_path, const std::string &name_Path, size_t deviceId)
    {

        deviceId_ = deviceId;
        modelPath = model_path;
        namesPath = name_Path;

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
    Result ClassifyPrivate::InitResource()
    {
        // const char *aclConfigPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/src/acl.json";
        // aclError ret = aclInit(aclConfigPath);
        aclError ret = aclInit(nullptr);
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl init failed, errorCode = %d", static_cast<int32_t>(ret));
            return FAILED;
        }
        INFO_LOG("acl init success");
        // set device
        ret = aclrtSetDevice(deviceId_);
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl set device %d failed, errorCode = %d", deviceId_, static_cast<int32_t>(ret));
            return FAILED;
        }
        INFO_LOG("set device %d success", deviceId_);

        // create context (set current)
        ret = aclrtCreateContext(&context_, deviceId_);
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl create context failed, deviceId = %d, errorCode = %d",
                      deviceId_, static_cast<int32_t>(ret));
            return FAILED;
        }
        INFO_LOG("create context success");

        // create stream
        ret = aclrtCreateStream(&stream_);
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl create stream failed, deviceId = %d, errorCode = %d",
                      deviceId_, static_cast<int32_t>(ret));
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
        ret = modelProcess.LoadModel(modelPath.c_str());
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute LoadModel failed");
            return FAILED;
        }
        ret = modelProcess.CreateModelDesc();
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute CreateModelDesc failed");
            return FAILED;
        }
        ret = modelProcess.GetInputSizeByIndex(0, devBufferSize);
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

        ret = modelProcess.CreateInput(picDevBuffer, devBufferSize);
        if (ret != SUCCESS)
        {
            ERROR_LOG("execute CreateInput failed");
            aclrtFree(picDevBuffer);
            return FAILED;
        }
        ret = modelProcess.CreateOutput();
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
        modelProcess.DestroyInput();
        modelProcess.DestroyOutput();

        aclrtFree(picDevBuffer);
    }
    void ClassifyPrivate::doClassify()
    {
        aclError ret;

        for (size_t index = 0; index < testFile.size(); ++index)
        {
            INFO_LOG("start to process file:%s", testFile[index].c_str());
            // copy image data to device buffer
            ret = Utils::MemcpyFileToDeviceBuffer(testFile[index], picDevBuffer, devBufferSize);
            if (ret != SUCCESS)
            {
                aclrtFree(picDevBuffer);
                ERROR_LOG("memcpy device buffer failed, index is %zu", index);
                // return FAILED;
            }
            ret = modelProcess.Execute();

            if (ret != SUCCESS)
            {
                ERROR_LOG("execute inference failed");
                aclrtFree(picDevBuffer);
                // return FAILED;
            }
            modelProcess.OutputModelResult();
        }
    }
    size_t ClassifyPrivate::GetBatch()
    {
        size_t inputNumber;
        aclError ret = modelProcess.GetInputSize(inputNumber);
        ;
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("acl init failed, errorCode = %d", static_cast<int32_t>(ret));
            return FAILED;
        }
        return inputNumber;
    }
}