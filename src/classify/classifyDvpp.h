#include "ascend_virgo.h"
#include "acl/acl.h"
#include "utils.h"
#include "model_process.h"

namespace ASCEND_VIRGO
{
    class ClassifyDvpp
    {
    public:
        ClassifyDvpp(const std::string &model_path, const std::string &name_Path, size_t deviceId);

        Result InitResource();

        ~ClassifyDvpp();
        void doClassify();
        size_t GetBatch();

    private:
        int32_t deviceId_;
        aclrtContext context_;
        aclrtStream stream_;
        std::vector<std::string> testFile;
        size_t devBufferSize;
        void *picDevBuffer = nullptr;
        ModelProcess modelProcess;
        std::string modelPath, namesPath;
        std::vector<std::string> labels;
    };
};