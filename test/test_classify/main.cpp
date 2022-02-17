#include <iostream>
#include "ascend_virgo.h"
using namespace ASCEND_VIRGO;

int main(int argc, char **argv)
{
    const std::string modelPath = "/data1/cxj/darknet2caffe/samples/cplusplus/level2_simple_inference/1_classification/resnet50_imagenet_classification/model/resnet18.om";
    const std::string namesPath = "/data1/code/names.txt";
    size_t deviceId = 0;
    std::shared_ptr<Classify> dfg = std::make_shared<Classify>(modelPath, namesPath, deviceId);
    size_t batchSize = dfg->GetBatch();
    dfg->doClassify();
    return 0;
}