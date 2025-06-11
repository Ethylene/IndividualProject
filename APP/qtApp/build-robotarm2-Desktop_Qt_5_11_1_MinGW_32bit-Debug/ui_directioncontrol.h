/********************************************************************************
** Form generated from reading UI file 'directioncontrol.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIRECTIONCONTROL_H
#define UI_DIRECTIONCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DirectionControl
{
public:
    QWidget *widget;
    QPushButton *bacl;
    QLabel *label;
    QGroupBox *direction;
    QGroupBox *arm;
    QLabel *up_lable;
    QPushButton *up;
    QPushButton *left;
    QPushButton *right;
    QPushButton *down;
    QPushButton *stop;
    QGroupBox *gripper;
    QPushButton *open;
    QPushButton *grip;
    QGroupBox *command;
    QPushButton *reset;
    QPushButton *statue;
    QPushButton *limits;
    QGroupBox *communication;
    QTextEdit *textEdit;
    QWidget *command_2;
    QPushButton *clearlog;

    void setupUi(QWidget *DirectionControl)
    {
        if (DirectionControl->objectName().isEmpty())
            DirectionControl->setObjectName(QStringLiteral("DirectionControl"));
        DirectionControl->resize(800, 700);
        widget = new QWidget(DirectionControl);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 0, 791, 51));
        bacl = new QPushButton(widget);
        bacl->setObjectName(QStringLiteral("bacl"));
        bacl->setGeometry(QRect(10, 10, 81, 31));
        bacl->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #607D8B; \n"
"    color: white; \n"
"    border: none; \n"
"    padding: 8px 16px; \n"
"    border-radius: 4px; \n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #455A64; \n"
"}"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(320, 10, 111, 31));
        direction = new QGroupBox(DirectionControl);
        direction->setObjectName(QStringLiteral("direction"));
        direction->setGeometry(QRect(50, 70, 621, 351));
        arm = new QGroupBox(direction);
        arm->setObjectName(QStringLiteral("arm"));
        arm->setGeometry(QRect(10, 90, 261, 221));
        up_lable = new QLabel(arm);
        up_lable->setObjectName(QStringLiteral("up_lable"));
        up_lable->setGeometry(QRect(60, 20, 55, 16));
        up = new QPushButton(arm);
        up->setObjectName(QStringLiteral("up"));
        up->setGeometry(QRect(110, 30, 31, 41));
        up->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #4CAF50; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 40px; \n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #45a049; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #3d8b40; \n"
"}"));
        up->setIconSize(QSize(80, 80));
        left = new QPushButton(arm);
        left->setObjectName(QStringLiteral("left"));
        left->setGeometry(QRect(20, 110, 41, 31));
        left->setMinimumSize(QSize(31, 0));
        left->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #4CAF50; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 40px; \n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #45a049; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #3d8b40; \n"
"}"));
        left->setIconSize(QSize(80, 80));
        right = new QPushButton(arm);
        right->setObjectName(QStringLiteral("right"));
        right->setGeometry(QRect(200, 110, 41, 31));
        right->setMinimumSize(QSize(31, 0));
        right->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #4CAF50; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 40px; \n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #45a049; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #3d8b40; \n"
"}"));
        right->setIconSize(QSize(80, 80));
        down = new QPushButton(arm);
        down->setObjectName(QStringLiteral("down"));
        down->setGeometry(QRect(110, 170, 31, 41));
        down->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #4CAF50; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 40px; \n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #45a049; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #3d8b40; \n"
"}"));
        down->setIconSize(QSize(80, 80));
        stop = new QPushButton(arm);
        stop->setObjectName(QStringLiteral("stop"));
        stop->setGeometry(QRect(90, 80, 80, 80));
        stop->setMinimumSize(QSize(80, 80));
        stop->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #f44336; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 40px; \n"
"    font-weight: bold;\n"
"    min-width: 80px;\n"
"    max-width: 80px;\n"
"    min-height: 80px;\n"
"    max-height: 80px;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #d32f2f; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #b71c1c; \n"
"}"));
        stop->setIconSize(QSize(80, 80));
        gripper = new QGroupBox(direction);
        gripper->setObjectName(QStringLiteral("gripper"));
        gripper->setGeometry(QRect(360, 100, 231, 211));
        open = new QPushButton(gripper);
        open->setObjectName(QStringLiteral("open"));
        open->setGeometry(QRect(80, 40, 93, 28));
        open->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #2196F3; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 8px; \n"
"    font-weight: bold;\n"
"    margin: 5px;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #1976D2; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #0D47A1; \n"
"}"));
        grip = new QPushButton(gripper);
        grip->setObjectName(QStringLiteral("grip"));
        grip->setGeometry(QRect(80, 160, 93, 28));
        grip->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #2196F3; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 8px; \n"
"    font-weight: bold;\n"
"    margin: 5px;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #1976D2; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #0D47A1; \n"
"}"));
        command = new QGroupBox(DirectionControl);
        command->setObjectName(QStringLiteral("command"));
        command->setGeometry(QRect(50, 420, 611, 61));
        reset = new QPushButton(command);
        reset->setObjectName(QStringLiteral("reset"));
        reset->setGeometry(QRect(20, 20, 93, 28));
        reset->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #FF9800; \n"
"    color: white; \n"
"    font-weight: bold; \n"
"    padding: 8px 16px; \n"
"    border-radius: 4px; \n"
"    border: none;\n"
"}"));
        statue = new QPushButton(command);
        statue->setObjectName(QStringLiteral("statue"));
        statue->setGeometry(QRect(150, 20, 93, 28));
        limits = new QPushButton(command);
        limits->setObjectName(QStringLiteral("limits"));
        limits->setGeometry(QRect(260, 20, 93, 28));
        communication = new QGroupBox(DirectionControl);
        communication->setObjectName(QStringLiteral("communication"));
        communication->setGeometry(QRect(50, 500, 611, 150));
        communication->setMaximumSize(QSize(16777215, 150));
        textEdit = new QTextEdit(communication);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(20, 20, 571, 91));
        textEdit->setReadOnly(true);
        command_2 = new QWidget(communication);
        command_2->setObjectName(QStringLiteral("command_2"));
        command_2->setGeometry(QRect(120, 110, 471, 41));
        clearlog = new QPushButton(command_2);
        clearlog->setObjectName(QStringLiteral("clearlog"));
        clearlog->setGeometry(QRect(370, 10, 93, 28));

        retranslateUi(DirectionControl);

        QMetaObject::connectSlotsByName(DirectionControl);
    } // setupUi

    void retranslateUi(QWidget *DirectionControl)
    {
        DirectionControl->setWindowTitle(QApplication::translate("DirectionControl", "Form", nullptr));
        bacl->setText(QApplication::translate("DirectionControl", "\342\206\220", nullptr));
        label->setText(QApplication::translate("DirectionControl", "Direction Control", nullptr));
        direction->setTitle(QApplication::translate("DirectionControl", "Direction Control", nullptr));
        arm->setTitle(QApplication::translate("DirectionControl", "Robot Arm", nullptr));
        up_lable->setText(QString());
        up->setText(QApplication::translate("DirectionControl", "\342\206\221", nullptr));
        left->setText(QApplication::translate("DirectionControl", "\342\206\220", nullptr));
        right->setText(QApplication::translate("DirectionControl", "\342\206\222", nullptr));
        down->setText(QApplication::translate("DirectionControl", "\342\206\223", nullptr));
        stop->setText(QApplication::translate("DirectionControl", "Stop", nullptr));
        gripper->setTitle(QApplication::translate("DirectionControl", "Gripper", nullptr));
        open->setText(QApplication::translate("DirectionControl", "open", nullptr));
        grip->setText(QApplication::translate("DirectionControl", "grip", nullptr));
        command->setTitle(QApplication::translate("DirectionControl", "Control commands", nullptr));
        reset->setText(QApplication::translate("DirectionControl", "Reset", nullptr));
        statue->setText(QApplication::translate("DirectionControl", "Get Status", nullptr));
        limits->setText(QApplication::translate("DirectionControl", "Display limits", nullptr));
        communication->setTitle(QApplication::translate("DirectionControl", "Communication Log", nullptr));
        clearlog->setText(QApplication::translate("DirectionControl", "Clear Log", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DirectionControl: public Ui_DirectionControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIRECTIONCONTROL_H
