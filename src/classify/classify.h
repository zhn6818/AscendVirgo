#include "ascend_virgo.h"
#include "acl/acl.h"
#include "utils.h"
#include "model_process.h"

namespace ASCEND_VIRGO
{
    class ClassifyPrivate
    {
    public:
        ClassifyPrivate(const std::string &model_path, const std::string &name_Path, size_t deviceId);

        Result InitResource();

        ~ClassifyPrivate();
        void Precess(const std::vector<cv::Mat> &imgs);
        void Precess(void *pDevbuff, size_t);
        void Classification(std::vector<std::vector<Predictioin>> &);
        size_t GetBatch();
        size_t GetInputSize();

    private:
        
        
        aclrtStream stream_;
        std::vector<std::string> testFile;
        size_t devBufferSize;
        void *picDevBuffer = nullptr;
        // ModelProcess modelProcess;
        std::shared_ptr<ModelProcess> modelProcess;
        std::string modelPath, namesPath;
        std::vector<std::string> labels;
    };
}