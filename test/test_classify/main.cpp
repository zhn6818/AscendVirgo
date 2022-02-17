// #include <iostream>
// #include "sample_process.h"
// #include "utils.h"
// using namespace std;
// bool g_isDevice = false;

// int main(int argc, char **argv)
// {
//     SampleProcess sampleProcess;
//     Result ret = sampleProcess.InitResource();
//     if (ret != SUCCESS)
//     {
//         ERROR_LOG("sample init resource failed");
//         return FAILED;
//     }

//     ret = sampleProcess.Process();
//     if (ret != SUCCESS)
//     {
//         ERROR_LOG("sample process failed");
//         return FAILED;
//     }

//     INFO_LOG("execute sample success");
//     return SUCCESS;
//     // return 0;
// }
#include <iostream>
#include "ascend_virgo.h"
using namespace ASCEND_VIRGO;

int main(int argc, char **argv)
{
    const std::string modelPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/model/resnet18.om";
    std::shared_ptr<Classify> dfg = std::make_shared<Classify>(modelPath);
    size_t batchSize = dfg->GetBatch();
    dfg->doClassify();
    return 0;
}