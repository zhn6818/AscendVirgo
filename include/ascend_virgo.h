#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>

namespace ASCEND_VIRGO
{
    typedef std::pair<std::string, float> Predictioin;

    class ClassifyPrivate;
    class Classify
    {
    public:
        Classify(const std::string &model_path);
        ~Classify();
        void doClassify();
        size_t GetBatch();

    private:
        std::shared_ptr<ClassifyPrivate> m_pHandlerClassifyPrivate;
    };
};
