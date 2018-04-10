/**
 * @file
 * @brief Header for the WS2Editor::RenderManager class
 */

#ifndef SMBLEVELWORKSHOP2_WS2EDITOR_RENDERMANAGER_HPP
#define SMBLEVELWORKSHOP2_WS2EDITOR_RENDERMANAGER_HPP

#include "ws2common/model/MeshSegment.hpp"
#include "ws2editor/CachedGlMesh.hpp"
#include "ws2editor/CachedGlTexture.hpp"
#include "ws2editor/rendering/IRenderCommand.hpp"
#include <QFile>
#include <QQueue>
#include <QHash>

namespace WS2Editor {
    using namespace WS2Common::Model;
    using namespace WS2Common::Resource;
    using namespace WS2Editor::Rendering;

    /**
     * @brief Don't forget to call init after constructig this
     *
     * init isn't called in the constructor as the correct OpenGL context may not be bound then
     */
    class RenderManager : public QObject {
        Q_OBJECT

        public:
            enum EnumVertexAttribs {
                VERTEX_POSITION = 0,
                VERTEX_NORMAL = 1,
                VERTEX_TEX_COORD = 2
            };

        protected:
            static const qint64 CACHE_TIMEOUT = 30 * 1000; //30s = 30 * 1000 ms

            QQueue<IRenderCommand*> renderFifo;
            QQueue<IRenderCommand*> renderSelectionFifo;

            QHash<const MeshSegment*, CachedGlMesh*> meshCache;
            QHash<const ResourceTexture*, CachedGlTexture*> textureCache;

            /**
             * @brief Used while a texture is loading/missing texture/etc.
             */
            CachedGlTexture *defaultTexture;

            GLuint fbo;
            GLuint fboColorTexture;
            GLuint fboCameraNormalTexture;
            GLuint fboDepthBuffer;

            GLuint fullscreenQuadVao;
            GLuint fullscreenQuadVbo;
            GLuint compositeShaderProg;
            GLuint compositeShaderTextureId;
            GLuint compositeShaderCameraNormalsTextureId;

            int viewportWidth;
            int viewportHeight;

        public:
            GLuint progID;
            GLuint shaderModelID;
            GLuint shaderViewID;
            GLuint shaderProjID;
            GLuint shaderNormID;
            //GLuint shaderTexID;
            GLuint shaderRenderCameraNormals;

            GLuint physicsDebugProgID;
            GLuint physicsDebugShaderModelID;
            GLuint physicsDebugShaderViewID;
            GLuint physicsDebugShaderProjID;

        protected:
            //Copied straight from Qt QGL
            static void convertToGLFormatHelper(QImage &dst, const QImage &img, GLenum texture_format);
            static QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format);

            void loadMesh(const MeshSegment *mesh);

            /**
             * @brief Loads a QImage texture into the GPU
             *
             * @param texture The texture to load into the GPU
             *
             * @return An object containing data about the loaded GL texture
             */
            CachedGlTexture* loadTexture(const QImage &texture);

            /**
             * @brief Loads a ResourceTexture to a GL texture
             *
             * The default texture will be loaded into the textureCache, before it is replaced with the provided texture
             * when it has loaded
             *
             * @param texture A loaded ResourceTexture
             */
            void loadTextureAsync(const ResourceTexture *texture);

            /**
             * @brief Generates textures/renderbuffers for the currently bound FBO
             *
             * @param fboWidth The width of the buffers to generate
             * @param fboHeight The height of the buffers to generate
             */
            void generateFboAttachments(int fboWidth, int fboHeight);

        public:
            //Copied straight from Qt QGL
            static QImage convertToGLFormat(const QImage &img);

            void init(int fboWidth, int fboHeight);

            /**
             * @brief Cleans up - note that the correct GL context must be bound
             */
            void destroy();

            /**
             * @brief Resizes the framebuffer attachments used for rendering
             *
             * @param width The width of the viewport
             * @param width The height of the viewport
             */
            void resizeViewport(int width, int height);

            /**
             * @brief Load, compile and link the shader files given
             *
             * @param vertFile The vertex shader file
             * @param fragFile The fragment shader file
             *
             * @return The program ID obtained when linking the shaders
             *
             * @todo Handle errors
             */
            GLuint loadShaders(QFile *vertFile, QFile *fragFile);

            /**
             * @brief Adds a RenderMesh command to the render fifo, to be rendered later with renderQueue()
             *
             * @param mesh
             * @param renderCameraNormals Used for the selection outline
             */
            void enqueueRenderMesh(const MeshSegment *mesh, bool renderCameraNormals);

            /**
             * @brief Renders all meshes in the render fifo
             *
             * @param targetFramebuffer The framebuffer to render to
             */
            void renderQueue(GLuint targetFramebuffer);

            /**
             * @brief Returns the GL texture for a ResourceTexture - will load one if it is not cached already
             *
             * @param tex The GL texture corresponding to the ResourceTexture provided
             */
            CachedGlTexture* getTextureForResourceTexture(const ResourceTexture *tex);

            /**
             * @brief Unloads all shaders used in regular rendering (This does not include physics debug shaders)
             */
            void unloadShaders();

            void addTexture(const QImage image, const ResourceTexture *tex);

            /**
             * @brief Checks for OpenGL errors and logs them if any are found
             *
             * @param location This text is tacked on to the end of the log message.
             *                 It's recommended you put where in the code the function is called, to aid with tracking
             *                 down issues.
             */
            static void checkErrors(QString location);

        public slots:
            void clearMeshCache();
            void clearTextureCache();
            void clearAllCaches();
    };
}

#endif
