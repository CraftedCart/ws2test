/**
 * @file
 * @brief Header for the StageIdeaGeneratorWindow class
 */

#ifndef SMBLEVELWORKSHOP2_WS2EDITOR_UI_STAGEIDEAGENERATORWINDOW_HPP
#define SMBLEVELWORKSHOP2_WS2EDITOR_UI_STAGEIDEAGENERATORWINDOW_HPP

#include <QDialog>

namespace Ui {
    class StageIdeaGeneratorWindow;
}

namespace WS2Editor {
    namespace UI {
        class StageIdeaGeneratorWindow : public QDialog {
            Q_OBJECT

            public:
                explicit StageIdeaGeneratorWindow(QWidget *parent = 0);
                ~StageIdeaGeneratorWindow();

            protected:
                QStringList themes;
                QStringList mechanics;

            private:
                Ui::StageIdeaGeneratorWindow *ui;

            protected:
                void selectCombination();
                void selectTheme();
                void selectMechanic();
        };
    }
}

#endif

