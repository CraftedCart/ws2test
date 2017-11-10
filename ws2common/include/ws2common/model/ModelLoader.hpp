/**
 * @file
 * @brief Header for the ModelLoader namespace
 */

#ifndef SMBLEVELWORKSHOP2_WS2COMMON_MODEL_MODELLOADER_HPP
#define SMBLEVELWORKSHOP2_WS2COMMON_MODEL_MODELLOADER_HPP

#include "ws2common/resource/ResourceMesh.hpp"
#include <assimp/scene.h>
#include <QVector>
#include <QFile>
#include <QDir>

namespace WS2Common {
    namespace Model {
        namespace ModelLoader {
                /**
                 * @brief Loads a model into the scene.
                 *
                 * @param file The model file to append
                 * @param shouldLoad Should the model be loaded into the GPU
                 * @param resources A vector of resources (Optional) - Used to prevent creating duplicate resources, and
                 *                  is appended to when loading models and textures
                 *
                 * @return A vector of added meshes
                 */
                QVector<WS2Common::Resource::ResourceMesh*> loadModel(QFile &file, QVector<Resource::AbstractResource*> *resources);

                /**
                 * @brief Append a model to the scene from the file given
                 *
                 * @param filePath The file path to the model file to append
                 * @param resources A vector of resources (Optional) - Used to prevent creating duplicate resources, and
                 *                  is appended to when loading models and textures
                 *
                 * @return A vector of added meshes
                 */
                QVector<WS2Common::Resource::ResourceMesh*> addModelFromFile(const char *filePath, QVector<Resource::AbstractResource*> *resources);

                /**
                 * @brief Recursive function that keeps calling itself for each child node in the parent node given.
                 *        This function is used to convert data generated by Assimp into data useable by WS2.
                 *
                 * @param node The parent node to process. This is usually the root node if calling it yourself.
                 * @param scene The scene that the node belongs to
                 * @param globalTransform A matrix transformation to apply to all vertices
                 * @param parentDir The parent directory of the file
                 * @param meshVector A reference to a vector that will be populated with all meshes processed
                 * @param resources A vector of resources (Optional) - Used to prevent creating duplicate resources, and
                 *                  is appended to when loading models and textures
                 */
                void processNode(
                        const aiNode *node,
                        const aiScene *scene,
                        const glm::mat4 globalTransform,
                        const QString *filePath,
                        const QDir *parentDir,
                        QVector<WS2Common::Resource::ResourceMesh*> &meshVector,
                        QVector<Resource::AbstractResource*> *resources
                        );

                /**
                 * @brief Converts an aiMesh into a WS2Editor::Model::Mesh
                 *
                 * @param mesh The aiMesh to convert
                 * @param scene The scene that the aiMesh belongs to
                 * @param globalTransform A matrix transformation to apply to all vertices
                 * @param parentDir The parent directory of the file
                 * @param resources A vector of resources (Optional) - Used to prevent creating duplicate resources, and
                 *                  is appended to when loading models and textures
                 *
                 * @return The converted mesh
                 */
                WS2Common::Model::MeshSegment* processMeshSegment(
                        const aiMesh *mesh,
                        const aiScene *scene,
                        const glm::mat4 globalTransform,
                        const QDir *parentDir,
                        QVector<Resource::AbstractResource*> *resources
                        );

                /**
                 * @brief Loads textures for a material
                 *
                 * @param material The material to load textures for
                 * @param type The type of textures to load
                 * @param parentDir The parent directory of the file
                 * @param shouldLoad Should the model be loaded into the GPU
                 * @param resources A vector of resources (Optional) - Used to prevent creating duplicate resources, and
                 *                  is appended to when loading models and textures
                 *
                 * @return A vector of textures
                 */
                QVector<WS2Common::Resource::ResourceTexture*> loadMaterialTextures(
                        aiMaterial *material,
                        aiTextureType type,
                        const QDir *parentDir,
                        QVector<Resource::AbstractResource*> *resources
                        );

                /**
                 * @brief Gets a resource originating from the given file path
                 *
                 * The resource is statically casted to the type T specified
                 *
                 * @tparam T The type to cast the resource to. This should be a pointer type.
                 * @param filePath The file path of the resource
                 * @param vec The resource vector to search through
                 *
                 * @return A pointer to the resource if it already exists, or nullptr otherwise
                 */
                template <class T>
                T getResourceFromFilePath(QString filePath, QVector<Resource::AbstractResource*> &vec);
        }
    }
}

#include "ws2common/model/ModelLoader.ipp"

#endif
