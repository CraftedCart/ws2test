/**
 * @file
 * @brief Header for the message handler, for logging output
 */

#include <QtGlobal>
#include <QString>
#include <QDebug>
#include <glm/glm.hpp>

#ifndef SMBLEVELWORKSHOP2_WS2COMMON_MESSAGEHANDLER_HPP
#define SMBLEVELWORKSHOP2_WS2COMMON_MESSAGEHANDLER_HPP

namespace WS2Common {
    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    //Custom types
    QDebug operator<<(QDebug debug, const glm::vec3 &vec);
}

#endif

