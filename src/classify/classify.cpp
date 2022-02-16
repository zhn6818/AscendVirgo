#include "ascend_virgo.h"
#include "acl/acl.h"
// #include "utils.h"

namespace ASCEND_VIRGO
{

    class ClassifyPrivate
    {
    public:
        ClassifyPrivate()
        {
            // std::cout << "ClassifyPrivate" << std::endl;
        }
        // Result InitResource()
        // {
        //     // const char *aclConfigPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/src/acl.json";
        //     // aclError ret = aclInit(aclConfigPath);
        //     // if (ret != ACL_SUCCESS)
        //     // {
        //     //     ERROR_LOG("acl init failed, errorCode = %d", static_cast<int32_t>(ret));
        //     //     return FAILED;
        //     // }
        //     // INFO_LOG("acl init success");
        // }
        ~ClassifyPrivate()
        {
            // std::cout << "~ClassifyPrivate" << std::endl;
        }
        void doClassify()
        {
            // std::cout << "do ClassifyPrivate" << std::endl;
        }

    private:
        int a;
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