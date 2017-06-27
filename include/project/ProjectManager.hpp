/**
 * @file
 * @brief Header for the WS2::Project::ProjectManager namespace
 */

#ifndef SMBLEVELWORKSHOP2_PROJECT_PROJECTMANAGER_HPP
#define SMBLEVELWORKSHOP2_PROJECT_PROJECTMANAGER_HPP

#include "project/Project.hpp"

namespace WS2 {
    namespace Project {
        namespace ProjectManager {
            /**
             * @brief Internal members for ProjectManager. It is recommended to use use the functions in ProjectManager
             *        over accessing the internals directly.
             */
            namespace ProjectManagerInternal {
                extern Project *activeProject;
            }

            /**
             * @brief Getter for WS2::Project::ProjectManager::ProjectManagerInternal::activeProject
             *
             * @return A pointer to the active project
             */
            Project* getActiveProject();
        }
    }
}

#endif
