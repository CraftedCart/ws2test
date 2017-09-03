/**
 * @file
 * @brief Header for the WS2::Task::Task class
 */

#ifndef SMBLEVELWORKSHOP2_TASK_TASK_HPP
#define SMBLEVELWORKSHOP2_TASK_TASK_HPP

#include "Progress.hpp"
#include <QString>

namespace WS2 {
    namespace Task {
        class Task {
            public:
                virtual ~Task() {}
                virtual void exec(Progress *prog) = 0;
                virtual QString getTranslatedMessage() = 0;
        };
    }
}

#endif
