#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <ostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

namespace ASCEND_VIRGO
{
    typedef std::pair<std::string, float> Predictioin;
    static float prob_sigmoid(float x) { return (1 / (1 + exp(-x))); }
    class ClassifyDvpp;
    class ClassifyPrivate;
    class Classify
    {
    public:
        Classify(const std::string &model_path, const std::string &name_Path, size_t deviceId);
        ~Classify();
        void Precess(const std::vector<cv::Mat> &);
        void Classification(std::vector<std::vector<Predictioin>> &);
        size_t GetBatch();
        size_t GetInputSize();

    private:
        std::shared_ptr<ClassifyPrivate> m_pHandlerClassifyPrivate;
        std::shared_ptr<ClassifyDvpp> m_pHandlerClassifyDvpp;
    };
};
