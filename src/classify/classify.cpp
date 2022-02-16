#include "ascend_virgo.h"
#include "acl/acl.h"
#include "utils.h"
#include "model_process.h"

namespace ASCEND_VIRGO
{

    class ClassifyPrivate
    {
    public:
        ClassifyPrivate()
        {
            // std::cout << "ClassifyPrivate" << std::endl;
            deviceId_ = 0;
            Result ret = InitResource();
        }
        Result InitResource()
        {
            const char *aclConfigPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/src/acl.json";
            aclError ret = aclInit(aclConfigPath);
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

            ModelProcess modelProcess;
            const char *omModelPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/model/resnet18.om";
            ret = modelProcess.LoadModel(omModelPath);
            if (ret != SUCCESS)
            {
                ERROR_LOG("execute LoadModel failed");
                return FAILED;
            }

            return SUCCESS;
        }

        ~ClassifyPrivate()
        {
            // std::cout << "~ClassifyPrivate" << std::endl;
        }
        void doClassify()
        {
            // std::cout << "do ClassifyPrivate" << std::endl;
        }

    private:
        int32_t deviceId_;
        aclrtContext context_;
        aclrtStream stream_;
    };
    Classify::Classify()
    {
        // std::cout << "Classify" << std::endl;
        m_pHandlerClassifyPrivate = std::make_shared<ClassifyPrivate>();
    }
    Classify::~Classify()
    {
        // std::cout << "~Classify" << std::endl;
    }
    void Classify::doClassify()
    {
        // std::cout << "doClassify" << std::endl;
        m_pHandlerClassifyPrivate->doClassify();
    }
};