#include "ws2common/scene/SceneNode.hpp"
#include <QtAlgorithms>

namespace WS2Common {
    namespace Scene {
        SceneNode::SceneNode(const QString name) {
            this->name = name;
        }

        SceneNode::~SceneNode() {
            qDeleteAll(children);
            if (animation != nullptr) delete animation;
        }

        const QString SceneNode::getName() const {
            return name;
        }

        void SceneNode::setName(const QString name) {
            this->name = name;
        }

        QVector<SceneNode*>& SceneNode::getChildren() {
            return children;
        }

        const QVector<SceneNode*>& SceneNode::getChildren() const {
            return children;
        }

        SceneNode* SceneNode::getChildByIndex(int index) {
            return children[index];
        }

        SceneNode* SceneNode::getChildByName(QString name) {
            auto node = std::find_if(
                    children.begin(),
                    children.end(),
                    [&name](const SceneNode *object) { return object->getName() == name; }
                    );

            if (node != children.end()) {
                //Node found
                return static_cast<SceneNode*>(*node);
            } else {
                //Node not found
                return nullptr;
            }
        }

        int SceneNode::getIndex() {
            if (parent) {
                return parent->getChildren().indexOf(const_cast<SceneNode*>(this));
            } else {
                return 0;
            }
        }

        void SceneNode::addChild(SceneNode *child) {
            child->setParent(this);
            children.append(child);
        }

        int SceneNode::getChildCount() const {
            return children.size();
        }

        SceneNode* SceneNode::getParent() {
            return parent;
        }

        void SceneNode::setParent(SceneNode *parent) {
            this->parent = parent;
        }

        glm::vec3 SceneNode::getOrigin() const {
            return origin;
        }

        void SceneNode::setOrigin(const glm::vec3 origin) {
            this->origin = origin;
        }

        glm::vec3 SceneNode::getPosition() const {
            return position;
        }

        void SceneNode::setPosition(const glm::vec3 position) {
            this->position = position;
        }

        glm::vec3 SceneNode::getRotation() const {
            return rotation;
        }

        void SceneNode::setRotation(const glm::vec3 rotation) {
            this->rotation = rotation;
        }

        glm::vec3 SceneNode::getScale() const {
            return scale;
        }

        void SceneNode::setScale(const glm::vec3 scale) {
            this->scale = scale;
        }

        void SceneNode::setTransformAnimation(Animation::TransformAnimation *anim) {
            animation = anim;
        }

        void SceneNode::removeTransformAnimation() {
            delete animation;
            animation = nullptr;
        }

        Animation::TransformAnimation* SceneNode::getTransformAnimation() {
            return animation;
        }

        const Animation::TransformAnimation* SceneNode::getTransformAnimation() const {
            return animation;
        }

        void SceneNode::setAnimationSeesawType(EnumAnimationSeesawType type) {
            this->animationSeesawType = type;
        }

        EnumAnimationSeesawType SceneNode::getAnimationSeesawType() const {
            return animationSeesawType;
        }

        float SceneNode::getSeesawSensitivity() const {
            return seesawSensitivity;
        }

        void SceneNode::setSeesawSensitivity(float value) {
            seesawSensitivity = value;
        }

        float SceneNode::getSeesawResetStiffness() const {
            return seesawResetStiffness;
        }

        void SceneNode::setSeesawResetStiffness(float value) {
            seesawResetStiffness = value;
        }

        float SceneNode::getSeesawRotationBounds() const {
            return seesawRotationBounds;
        }

        void SceneNode::setSeesawRotationBounds(float value) {
            seesawRotationBounds = value;
        }
    }
}

