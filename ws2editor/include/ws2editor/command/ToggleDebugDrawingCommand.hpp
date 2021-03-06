/**
 * @file
 * @brief Header for the ToggleDebugDrawingCommand class
 */

#ifndef SMBLEVELWORKSHOP2_WS2EDITOR_COMMAND_TOGGLEDEBUGDRAWINGCOMMAND_HPP
#define SMBLEVELWORKSHOP2_WS2EDITOR_COMMAND_TOGGLEDEBUGDRAWINGCOMMAND_HPP

#include "ws2editor/command/ICommandHandler.hpp"

namespace WS2Editor {
    namespace Command {
        class ToggleDebugDrawingCommand : public ICommandHandler {
            public:
                virtual QString getCommand() override;
                virtual CommandReply executeCommand(QString command) override;
        };
    }
}

#endif

