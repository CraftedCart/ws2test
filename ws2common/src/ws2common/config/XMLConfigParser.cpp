#include "ws2common/config/XMLConfigParser.hpp"
#include "ws2common/scene/MeshCollisionSceneNode.hpp"
#include "ws2common/SerializeUtils.hpp"
#include <QXmlStreamReader>
#include <QDebug>
#include <QCoreApplication>

namespace WS2Common {
    namespace Config {
        Stage* XMLConfigParser::parseStage(QString config, QDir relativeRoot) {
            Stage *stage = new Stage();

            //Read and parse the XML
            QXmlStreamReader xml(config);
            while (!xml.atEnd()) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "superMonkeyBallStage") { //TODO: Check superMonkeyBallStage version attribute
                    //Parse the stages
                    while (!(xml.isEndElement() && xml.name() == "superMonkeyBallStage")) {
                        //Keep reading until the </superMonkeyBallStage> tag
                        xml.readNext();
                        if (!xml.isStartElement()) continue; //Ignore all end elements, again

                        if (xml.name() == "modelImport") {
                            QUrl url = parseModelImport(xml, relativeRoot);
                            if (!url.isEmpty()) stage->addModel(url);
                        } else if (xml.name() == "start") {
                            stage->getRootNode()->addChild(parseStart(xml));
                        } else if (xml.name() == "backgroundModel") {
                            stage->getFirstBackgroundGroup(true)->addChild(parseBackgroundModel(xml));
                        } else if (xml.name() == "falloutPlane") {
                            stage->setFalloutY(SerializeUtils::getAttribute(xml.attributes(), "y").toFloat());
                        } else if (xml.name() == "fog") {
                            qWarning() << "fog not yet implemented!";
                            xml.skipCurrentElement();
                        } else if (xml.name() == "itemGroup") {
                            stage->getRootNode()->addChild(parseItemGroup(xml));
                        } else {
                            qWarning().noquote() << "Unrecognised tag:" << xml.name();
                        }
                    }

                } else {
                    qWarning().noquote() << QString("XML root tag is %1! It should be superMonkeyBallStage").arg(xml.name().toString());
                    //Don't spam the log with every single tag - Get outta here after the root one
                    //TODO: Throw an exception or something
                    break;
                }
            }

            if (xml.hasError()) {
                //TODO: Throw an exception or something
                qWarning().noquote() << "XML parsing error:" << xml.errorString();
            }

            //Done parsing - now link wormholes
            QHashIterator<Scene::WormholeSceneNode*, QString> i(wormholeDestMap);
            while (i.hasNext()) {
                i.next();

                //Iterate over all wormholes to find the one with the matching destination name
                QHashIterator<Scene::WormholeSceneNode*, QString> j(wormholeDestMap);
                while (j.hasNext()) {
                    j.next();

                    if (i.value() == j.key()->getName()) {
                        i.key()->setDestination(j.key());
                        break;
                    }
                }
            }

            return stage;
        }

        QUrl XMLConfigParser::parseModelImport(QXmlStreamReader &xml, QDir relativeRoot) {
            QString value = xml.readElementText();

            if (value.startsWith("//")) { //Beginning with // denotes a relative path
                QFileInfo fileInfo(relativeRoot, value.mid(2));

                //Check if the file is missing, and return am empty QUrl if so
                if (!fileInfo.exists()) {
                    qWarning().noquote() << "modelImport file missing:" << fileInfo.absoluteFilePath();
                    return QUrl();
                }

                return QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            } else { //Else it should be a URL (Like file:///something/or/other.obj)
                QUrl url(value);
                QFileInfo fileInfo(url.toLocalFile());

                //Check if the file is missing, and return am empty QUrl if so
                if (!fileInfo.exists()) {
                    qWarning().noquote() << "modelImport file missing:" << fileInfo.absoluteFilePath();
                    return QUrl();
                }

                return url;
            }
        }

        Scene::StartSceneNode* XMLConfigParser::parseStart(QXmlStreamReader &xml) {
            //Default name is "Start", translated
            Scene::StartSceneNode *start = new Scene::StartSceneNode(QCoreApplication::translate("XMLConfigParser", "Start"));

            while (!(xml.isEndElement() && xml.name() == "start")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    start->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    start->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    start->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else {
                    qWarning().noquote() << "Unrecognised tag: start >" << xml.name();
                }
            }

            return start;
        }

        Scene::MeshSceneNode* XMLConfigParser::parseBackgroundModel(QXmlStreamReader &xml) {
            //A valid XML config should set the name of the bg model - It should never be UNDEFINED!
            Scene::MeshSceneNode *bg = new Scene::MeshSceneNode("UNDEFINED!");

            while (!(xml.isEndElement() && xml.name() == "backgroundModel")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    QString name = xml.readElementText();

                    bg->setName(name);
                    bg->setMeshName(name);
                } else if (xml.name() == "position") {
                    bg->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    bg->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "scale") {
                    bg->setScale(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "animKeyframes") { //TODO
                    qWarning() << "backgroundModel > animKeyframes not yet implemented!";
                    xml.skipCurrentElement();
                } else if (xml.name() == "animLoopTime") { //TODO
                    qWarning() << "backgroundModel > animLoopTime not yet implemented!";
                    xml.skipCurrentElement();
                } else if (xml.name() == "textureScroll") { //TODO
                    qWarning() << "backgroundModel > textureScroll not yet implemented!";
                    xml.skipCurrentElement();
                } else {
                    qWarning().noquote() << "Unrecognised tag: backgroundModel >" << xml.name();
                }
            }

            return bg;
        }

        Scene::GroupSceneNode* XMLConfigParser::parseItemGroup(QXmlStreamReader &xml) {
            //Default name is "Group", translated
            Scene::GroupSceneNode *group = new Scene::GroupSceneNode(QCoreApplication::tr("XMLConfigParser", "Group"));

            //Some stuff that needs to be linked after parsing the entire group
            Animation::TransformAnimation *anim = nullptr;
            EnumPlaybackState initialState = EnumPlaybackState::PLAY;
            Animation::EnumLoopType loopType = Animation::EnumLoopType::LOOPING;
            float loopTime = 0.0f;

            while (!(xml.isEndElement() && xml.name() == "itemGroup")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    group->setName(xml.readElementText());
                } else if (xml.name() == "rotationCenter") { //TODO
                    group->setOriginPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "initialRotation") { //TODO
                    group->setOriginRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "animSeesawType") { //TODO
                    QPair<EnumAnimationSeesawType, Animation::EnumLoopType> type = parseAnimLoopType(xml);
                    group->setAnimationSeesawType(type.first);
                    loopType = type.second; //loopType will be linked with an animation later if needed
                } else if (xml.name() == "conveyorSpeed") {
                    group->setConveyorSpeed(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "seesawSensitivity") {
                    group->setSeesawSensitivity(xml.readElementText().toFloat()); //TODO: Error checking
                } else if (xml.name() == "seesawResetStiffness") {
                    group->setSeesawResetStiffness(xml.readElementText().toFloat()); //TODO: Error checking
                } else if (xml.name() == "seesawRotationBounds") {
                    group->setSeesawRotationBounds(xml.readElementText().toFloat()); //TODO: Error checking
                } else if (xml.name() == "animKeyframes") {
                    Animation::TransformAnimation *transformAnim = parseTransformAnimation(xml);
                    group->setTransformAnimation(transformAnim);
                    anim = transformAnim; //For later linking (So that the loop type is set in the transformAnim)
                } else if (xml.name() == "animLoopTime") {
                    loopTime = xml.readElementText().toFloat(); //For later linking
                } else if (xml.name() == "animGroupId") {
                    group->setAnimationGroupId(xml.readElementText().toUInt());
                } else if (xml.name() == "animInitialState") {
                    initialState = PlaybackState::fromString(xml.readElementText());
                } else if (xml.name() == "collisionGrid") {
                    group->setCollisionGrid(parseCollisionGrid(xml));
                } else if (xml.name() == "goal") {
                    group->addChild(parseGoal(xml));
                } else if (xml.name() == "bumper") {
                    group->addChild(parseBumper(xml));
                } else if (xml.name() == "jamabar") {
                    group->addChild(parseJamabar(xml));
                } else if (xml.name() == "banana") {
                    group->addChild(parseBanana(xml));
                } else if (xml.name() == "falloutVolume") {
                    group->addChild(parseFalloutVolume(xml));
                } else if (xml.name() == "wormhole") {
                    group->addChild(parseWormhole(xml));
                } else if (xml.name() == "switch") {
                    group->addChild(parseSwitch(xml));
                } else if (xml.name() == "levelModel") { //Deprecated
                    group->addChild(parseLevelModel(xml));
                } else if (xml.name() == "stageModel") {
                    qWarning() << "itemGroup > stageModel not yet implemented!";
                    xml.skipCurrentElement();
                } else {
                    qWarning().noquote() << "Unrecognised tag: itemGroup >" << xml.name();
                }
            }

            if (anim != nullptr) {
                anim->setInitialState(initialState);
                anim->setLoopType(loopType);
                anim->setLoopTime(loopTime);
            }

            return group;
        }

        Scene::GoalSceneNode* XMLConfigParser::parseGoal(QXmlStreamReader &xml) {
            //Default name is "Goal x", translated
            static unsigned int id = 0;
            Scene::GoalSceneNode *goal = new Scene::GoalSceneNode(QCoreApplication::translate("XMLConfigParser", "Goal %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "goal")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    goal->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    goal->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    goal->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "type") {
                    goal->setType(GoalType::fromString(xml.readElementText()));
                } else {
                    qWarning().noquote() << "Unrecognised tag: goal >" << xml.name();
                }
            }

            id++;

            return goal;
        }

        Scene::BumperSceneNode* XMLConfigParser::parseBumper(QXmlStreamReader &xml) {
            //Default name is "Bumper x", translated
            static unsigned int id = 0;
            Scene::BumperSceneNode *bumper = new Scene::BumperSceneNode(QCoreApplication::translate("XMLConfigParser", "Bumper %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "bumper")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    bumper->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    bumper->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    bumper->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "scale") {
                    bumper->setScale(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else {
                    qWarning().noquote() << "Unrecognised tag: bumper >" << xml.name();
                }
            }

            id++;

            return bumper;
        }

        Scene::JamabarSceneNode* XMLConfigParser::parseJamabar(QXmlStreamReader &xml) {
            //Default name is "Jamabar x", translated
            static unsigned int id = 0;
            Scene::JamabarSceneNode *jamabar = new Scene::JamabarSceneNode(QCoreApplication::translate("XMLConfigParser", "Jamabar %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "jamabar")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    jamabar->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    jamabar->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    jamabar->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "scale") {
                    jamabar->setScale(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else {
                    qWarning().noquote() << "Unrecognised tag: jamabar >" << xml.name();
                }
            }

            id++;

            return jamabar;
        }

        Scene::BananaSceneNode* XMLConfigParser::parseBanana(QXmlStreamReader &xml) {
            //Default name is "Banana x", translated
            static unsigned int id = 0;
            Scene::BananaSceneNode *banana = new Scene::BananaSceneNode(QCoreApplication::translate("XMLConfigParser", "Banana %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "banana")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    banana->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    banana->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "type") {
                    banana->setType(BananaType::fromString(xml.readElementText()));
                } else {
                    qWarning().noquote() << "Unrecognised tag: banana >" << xml.name();
                }
            }

            id++;

            return banana;
        }

        Scene::FalloutVolumeSceneNode* XMLConfigParser::parseFalloutVolume(QXmlStreamReader &xml) {
            //Default name is "Fallout Volume x", translated
            static unsigned int id = 0;
            Scene::FalloutVolumeSceneNode *volume = new Scene::FalloutVolumeSceneNode(QCoreApplication::translate("XMLConfigParser", "Fallout Volume %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "falloutVolume")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    volume->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    volume->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    volume->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "scale") {
                    volume->setScale(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else {
                    qWarning().noquote() << "Unrecognised tag: falloutVolume >" << xml.name();
                }
            }

            id++;

            return volume;
        }

        Scene::WormholeSceneNode* XMLConfigParser::parseWormhole(QXmlStreamReader &xml) {
            //Default name is "Wormhole x", translated
            static unsigned int id = 0;
            Scene::WormholeSceneNode *wh = new Scene::WormholeSceneNode(QCoreApplication::translate("XMLConfigParser", "Wormhole %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "wormhole")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    wh->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    wh->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    wh->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "destinationName") {
                    wormholeDestMap[wh] = xml.readElementText();
                } else {
                    qWarning().noquote() << "Unrecognised tag: wormhole >" << xml.name();
                }
            }

            id++;

            return wh;
        }

        Scene::SwitchSceneNode* XMLConfigParser::parseSwitch(QXmlStreamReader &xml) {
            //Default name is "Switch x", translated
            static unsigned int id = 0;
            //Woo can't use switch as a variable name!
            Scene::SwitchSceneNode *sw = new Scene::SwitchSceneNode(QCoreApplication::translate("XMLConfigParser", "Switch %1").arg(id));

            while (!(xml.isEndElement() && xml.name() == "switch")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "name") {
                    sw->setName(xml.readElementText());
                } else if (xml.name() == "position") {
                    sw->setPosition(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "rotation") {
                    sw->setRotation(SerializeUtils::getVec3Attributes(xml.attributes()));
                } else if (xml.name() == "type") {
                    sw->setType(PlaybackState::fromString(xml.readElementText()));
                } else if (xml.name() == "animGroupId") {
                    sw->setLinkedAnimGroupId(xml.readElementText().toUShort());
                } else {
                    qWarning().noquote() << "Unrecognised tag: switch >" << xml.name();
                }
            }

            id++;

            return sw;
        }

        Scene::MeshSceneNode* XMLConfigParser::parseLevelModel(QXmlStreamReader &xml) {
            QString name = xml.readElementText();

            //This is deprecated!
            qWarning() << "levelModel is deprecated! Prefer stageModel instead";
            qWarning().noquote().nospace() << "    <stageModel>";
            qWarning().noquote().nospace() << "        <name>" << name << "</name>";
            qWarning().noquote().nospace() << "        <collision>";
            qWarning().noquote().nospace() << "            <meshCollision>" << name << "</meshCollision>";
            qWarning().noquote().nospace() << "        </collision>";
            qWarning().noquote().nospace() << "    </stageModel>";

            Scene::MeshSceneNode *mesh = new Scene::MeshSceneNode(name);
            mesh->setMeshName(name);

            //Add the mesh collision, using the same object for collision
            Scene::MeshCollisionSceneNode *collision = new Scene::MeshCollisionSceneNode("Mesh Collision: " + name);
            collision->setMeshName(name);
            mesh->addChild(collision);

            return mesh;
        }

        CollisionGrid XMLConfigParser::parseCollisionGrid(QXmlStreamReader &xml) {
            CollisionGrid grid;

            while (!(xml.isEndElement() && xml.name() == "collisionGrid")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "start") {
                    grid.setGridStart(SerializeUtils::getVec2Attributes(xml.attributes(), "x", "z"));
                } else if (xml.name() == "step") {
                    grid.setGridStep(SerializeUtils::getVec2Attributes(xml.attributes(), "x", "z"));
                } else if (xml.name() == "count") {
                    grid.setGridStepCount(SerializeUtils::getUVec2Attributes(xml.attributes(), "x", "z"));
                } else {
                    qWarning().noquote() << "Unrecognised tag: collisionGrid >" << xml.name();
                }
            }

            return grid;
        }

        QPair<EnumAnimationSeesawType, Animation::EnumLoopType> XMLConfigParser::parseAnimLoopType(QXmlStreamReader &xml) {
            QString str = xml.readElementText();
            EnumAnimationSeesawType animSeesawType;
            Animation::EnumLoopType loopType;

            if (str == "LOOPING_ANIMATION") {
                animSeesawType = ANIMATION;
                loopType = Animation::LOOPING;
            } else if (str == "PLAY_ONCE_ANIMATION") {
                animSeesawType = ANIMATION;
                loopType = Animation::PLAY_ONCE;
            } else if (str == "SEESAW") {
                animSeesawType = SEESAW;
            }

            return QPair<EnumAnimationSeesawType, Animation::EnumLoopType>(animSeesawType, loopType);
        }

        Animation::TransformAnimation* XMLConfigParser::parseTransformAnimation(QXmlStreamReader &xml) {
            Animation::TransformAnimation *anim = new Animation::TransformAnimation;

            while (!(xml.isEndElement() && xml.name() == "animKeyframes")) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "posX") {
                    parseKeyframes(xml, anim->getPosXKeyframes());
                } else if (xml.name() == "posY") {
                    parseKeyframes(xml, anim->getPosYKeyframes());
                } else if (xml.name() == "posZ") {
                    parseKeyframes(xml, anim->getPosZKeyframes());
                } else if (xml.name() == "rotX") {
                    parseKeyframes(xml, anim->getRotXKeyframes());
                } else if (xml.name() == "rotY") {
                    parseKeyframes(xml, anim->getRotYKeyframes());
                } else if (xml.name() == "rotZ") {
                    parseKeyframes(xml, anim->getRotZKeyframes());
                } else {
                    qWarning().noquote() << "Unrecognised tag: animKeyframes >" << xml.name();
                }
            }

            return anim;
        }

        void XMLConfigParser::parseKeyframes(
                QXmlStreamReader &xml,
                std::set<Animation::KeyframeF*, Animation::KeyframeCompare> &keyframes
                ) {
            using namespace Animation;

            QStringRef parentName = xml.name();

            while (!(xml.isEndElement() && xml.name() == parentName)) {
                xml.readNext();
                if (!xml.isStartElement()) continue; //Ignore all end elements

                if (xml.name() == "keyframe") {
                    float time;
                    float value;
                    EnumEasing easing;

                    //Search through the attributes for time, value, and easing
                    foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                        if (attr.name() == "time") {
                            time = attr.value().toFloat();
                        } else if (attr.name() == "value") {
                            value = attr.value().toFloat();
                        } else if (attr.name() == "easing") {
                            easing = Easing::fromString(attr.value().toString());
                        }
                    }

                    //Create the keyframe
                    KeyframeF *k = new KeyframeF(time, value, easing);
                    keyframes.insert(k);
                } else {
                    qWarning().noquote() << "Unrecognised tag: [keyframeIdentier] >" << xml.name();
                }
            }
        }
    }
}

