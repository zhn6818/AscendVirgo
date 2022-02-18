#include "ascend_virgo.h"
#include "acl/acl.h"
#include "utils.h"
#include "model_process.h"
#include "classify.h"
#include "classifyDvpp.h"

namespace ASCEND_VIRGO
{

    Classify::Classify(const std::string &model_path, const std::string &name_Path, size_t deviceId)
    {
        m_pHandlerClassifyPrivate = std::make_shared<ClassifyPrivate>(model_path, name_Path, deviceId);
    }
    Classify::~Classify()
    {
    }
    size_t Classify::GetBatch()
    {
        return m_pHandlerClassifyPrivate->GetBatch();
    }
    void Classify::Precess(const std::vector<cv::Mat> &imgs)
    {
        m_pHandlerClassifyPrivate->Precess(imgs);
    }
    void Classify::Classification(std::vector<std::vector<Predictioin>> &result)
    {
        m_pHandlerClassifyPrivate->Classification(result);
    }
    size_t Classify::GetInputSize()
    {
        return m_pHandlerClassifyPrivate->GetInputSize();
    }
    // Classify::Classify(const std::string &model_path, const std::string &name_Path, size_t deviceId)
    // {
    //     m_pHandlerClassifyDvpp = std::make_shared<ClassifyDvpp>(model_path, name_Path, deviceId);
    // }
    // Classify::~Classify()
    // {
    // }
    // size_t Classify::GetBatch()
    // {
    //     return m_pHandlerClassifyDvpp->GetBatch();
    // }
    // void Classify::doClassify()
    // {
    //     m_pHandlerClassifyDvpp->doClassify();
    // }
};