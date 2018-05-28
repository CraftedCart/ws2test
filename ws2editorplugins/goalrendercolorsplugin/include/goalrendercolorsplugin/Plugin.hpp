/**
 * @file
 * @brief Goal render colors plugin header
 *
 * Provides colors to goals when rendering them in the world
 */

#ifndef SMBLEVELWORKSHOP2_WS2EDITORPLUGINS_GOALRENDERCOLORSPLUGIN_PLUGIN_HPP
#define SMBLEVELWORKSHOP2_WS2EDITORPLUGINS_GOALRENDERCOLORSPLUGIN_PLUGIN_HPP

#include "ws2editor/plugin/IEditorPlugin.hpp"
#include "ws2editor/ui/StageEditorWindow.hpp"
#include <QMouseEvent>

namespace WS2EditorPlugins {
    namespace GoalRenderColorsPlugin {
        class Plugin : public QObject, public WS2Editor::Plugin::IEditorPlugin {
            Q_OBJECT

            //Required macros to tell Qt about our plugin
            Q_PLUGIN_METADATA(IID WS2EDITOR_IEDITORPLUGIN_IID)
            Q_INTERFACES(WS2Editor::Plugin::IEditorPlugin)

            public:
                /**
                 * @brief The init function is called for each plugin while ws2editor is loading
                 *
                 * @return Whether this plugin initialized successfully or not
                 */
                virtual bool init() override;

            public slots:
                /**
                 * @brief Called after the StageEditorWindow has been constructed, so that the plugin can get a handle
                 *        on the window and hook into its functions/events there
                 *
                 * @param w The StageEditorWindow
                 */
                void onStageEditorWindowConstructed(WS2Editor::UI::StageEditorWindow &w);

                void onPostEnqueueSceneNodeRenderMesh(
                        WS2Common::Scene::SceneNode *node,
                        WS2Editor::Rendering::MeshRenderCommand *command
                        );
        };
    }
}

#endif

