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
    // Classify *tt = new Classify();
    // tt->doClassify();
    // delete tt;
    std::shared_ptr<Classify> dfg= std::make_shared<Classify>();
    dfg->doClassify();
    return 0;
}