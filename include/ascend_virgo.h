#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>


namespace ASCEND_VIRGO
{

    class ClassifyPrivate;
    class Classify
    {
    public:
        Classify();
        ~Classify();
        void doClassify();

    private:
        std::shared_ptr<ClassifyPrivate> m_pHandlerClassifyPrivate;
    };
};
