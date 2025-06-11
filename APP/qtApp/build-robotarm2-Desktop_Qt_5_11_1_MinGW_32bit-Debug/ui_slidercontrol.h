/********************************************************************************
** Form generated from reading UI file 'slidercontrol.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLIDERCONTROL_H
#define UI_SLIDERCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SliderControl
{
public:
    QWidget *widget;
    QPushButton *bacl;
    QLabel *label;
    QGroupBox *groupBox;
    QLabel *A_servo;
    QLabel *A_range;
    QSlider *A_slider;
    QSpinBox *A_spinBox;
    QLabel *B_servo;
    QLabel *B_range;
    QSlider *B_slider;
    QSpinBox *B_spinBox;
    QLabel *C_servo;
    QLabel *C_range;
    QSlider *C_slider;
    QSpinBox *C_spinBox;
    QLabel *G_servo;
    QLabel *G_range;
    QSlider *G_slider;
    QSpinBox *G_spinBox;
    QGroupBox *command;
    QPushButton *sendall;
    QPushButton *reset;
    QPushButton *statue;
    QPushButton *limits;
    QGroupBox *communication;
    QTextEdit *textEdit;
    QWidget *command_2;
    QPushButton *clearlog;

    void setupUi(QWidget *SliderControl)
    {
        if (SliderControl->objectName().isEmpty())
            SliderControl->setObjectName(QStringLiteral("SliderControl"));
        SliderControl->resize(800, 600);
        widget = new QWidget(SliderControl);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 0, 791, 51));
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
        label->setGeometry(QRect(320, 10, 81, 31));
        groupBox = new QGroupBox(SliderControl);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(30, 80, 761, 221));
        A_servo = new QLabel(groupBox);
        A_servo->setObjectName(QStringLiteral("A_servo"));
        A_servo->setGeometry(QRect(10, 50, 121, 16));
        A_range = new QLabel(groupBox);
        A_range->setObjectName(QStringLiteral("A_range"));
        A_range->setGeometry(QRect(140, 50, 101, 21));
        A_slider = new QSlider(groupBox);
        A_slider->setObjectName(QStringLiteral("A_slider"));
        A_slider->setGeometry(QRect(260, 50, 251, 21));
        A_slider->setMaximum(180);
        A_slider->setValue(90);
        A_slider->setOrientation(Qt::Horizontal);
        A_slider->setTickPosition(QSlider::TicksBelow);
        A_slider->setTickInterval(18);
        A_spinBox = new QSpinBox(groupBox);
        A_spinBox->setObjectName(QStringLiteral("A_spinBox"));
        A_spinBox->setGeometry(QRect(540, 50, 51, 22));
        A_spinBox->setMaximum(180);
        A_spinBox->setValue(90);
        B_servo = new QLabel(groupBox);
        B_servo->setObjectName(QStringLiteral("B_servo"));
        B_servo->setGeometry(QRect(20, 100, 91, 16));
        B_range = new QLabel(groupBox);
        B_range->setObjectName(QStringLiteral("B_range"));
        B_range->setGeometry(QRect(140, 100, 101, 21));
        B_slider = new QSlider(groupBox);
        B_slider->setObjectName(QStringLiteral("B_slider"));
        B_slider->setGeometry(QRect(260, 100, 251, 21));
        B_slider->setMaximum(77);
        B_slider->setValue(0);
        B_slider->setOrientation(Qt::Horizontal);
        B_slider->setTickPosition(QSlider::TicksBelow);
        B_slider->setTickInterval(8);
        B_spinBox = new QSpinBox(groupBox);
        B_spinBox->setObjectName(QStringLiteral("B_spinBox"));
        B_spinBox->setGeometry(QRect(540, 100, 51, 22));
        B_spinBox->setMaximum(77);
        B_spinBox->setValue(0);
        C_servo = new QLabel(groupBox);
        C_servo->setObjectName(QStringLiteral("C_servo"));
        C_servo->setGeometry(QRect(20, 150, 91, 16));
        C_range = new QLabel(groupBox);
        C_range->setObjectName(QStringLiteral("C_range"));
        C_range->setGeometry(QRect(140, 150, 111, 21));
        C_slider = new QSlider(groupBox);
        C_slider->setObjectName(QStringLiteral("C_slider"));
        C_slider->setGeometry(QRect(260, 150, 251, 21));
        C_slider->setMinimum(140);
        C_slider->setMaximum(180);
        C_slider->setValue(180);
        C_slider->setOrientation(Qt::Horizontal);
        C_slider->setTickPosition(QSlider::TicksBelow);
        C_slider->setTickInterval(4);
        C_spinBox = new QSpinBox(groupBox);
        C_spinBox->setObjectName(QStringLiteral("C_spinBox"));
        C_spinBox->setGeometry(QRect(540, 150, 51, 22));
        C_spinBox->setMinimum(140);
        C_spinBox->setMaximum(180);
        C_spinBox->setValue(180);
        G_servo = new QLabel(groupBox);
        G_servo->setObjectName(QStringLiteral("G_servo"));
        G_servo->setGeometry(QRect(20, 190, 121, 16));
        G_range = new QLabel(groupBox);
        G_range->setObjectName(QStringLiteral("G_range"));
        G_range->setGeometry(QRect(140, 190, 111, 21));
        G_slider = new QSlider(groupBox);
        G_slider->setObjectName(QStringLiteral("G_slider"));
        G_slider->setGeometry(QRect(260, 190, 251, 21));
        G_slider->setMaximum(37);
        G_slider->setValue(0);
        G_slider->setOrientation(Qt::Horizontal);
        G_slider->setTickPosition(QSlider::TicksBelow);
        G_slider->setTickInterval(4);
        G_spinBox = new QSpinBox(groupBox);
        G_spinBox->setObjectName(QStringLiteral("G_spinBox"));
        G_spinBox->setGeometry(QRect(540, 190, 51, 22));
        G_spinBox->setMinimum(0);
        G_spinBox->setMaximum(37);
        G_spinBox->setValue(0);
        command = new QGroupBox(SliderControl);
        command->setObjectName(QStringLiteral("command"));
        command->setGeometry(QRect(110, 330, 611, 61));
        sendall = new QPushButton(command);
        sendall->setObjectName(QStringLiteral("sendall"));
        sendall->setGeometry(QRect(10, 20, 131, 28));
        sendall->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #2196F3; \n"
"    color: white; \n"
"    font-weight: bold; \n"
"    padding: 8px 16px; \n"
"    border-radius: 4px; \n"
"    border: none;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #1976D2; \n"
"}"));
        reset = new QPushButton(command);
        reset->setObjectName(QStringLiteral("reset"));
        reset->setGeometry(QRect(190, 20, 93, 28));
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
        statue->setGeometry(QRect(330, 20, 93, 28));
        limits = new QPushButton(command);
        limits->setObjectName(QStringLiteral("limits"));
        limits->setGeometry(QRect(460, 20, 93, 28));
        communication = new QGroupBox(SliderControl);
        communication->setObjectName(QStringLiteral("communication"));
        communication->setGeometry(QRect(110, 400, 611, 150));
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

        retranslateUi(SliderControl);

        QMetaObject::connectSlotsByName(SliderControl);
    } // setupUi

    void retranslateUi(QWidget *SliderControl)
    {
        SliderControl->setWindowTitle(QApplication::translate("SliderControl", "Form", nullptr));
        bacl->setText(QApplication::translate("SliderControl", "\342\206\220", nullptr));
        label->setText(QApplication::translate("SliderControl", "Slider Control", nullptr));
        groupBox->setTitle(QApplication::translate("SliderControl", "Joint angle control", nullptr));
        A_servo->setText(QApplication::translate("SliderControl", "Basement (servo A)", nullptr));
        A_range->setText(QApplication::translate("SliderControl", "range: 0\302\260 - 180\302\260", nullptr));
        A_spinBox->setSuffix(QApplication::translate("SliderControl", "\302\260", nullptr));
        B_servo->setText(QApplication::translate("SliderControl", "Axis B (servo B)", nullptr));
        B_range->setText(QApplication::translate("SliderControl", "range: 0\302\260 - 77\302\260", nullptr));
        B_spinBox->setSuffix(QApplication::translate("SliderControl", "\302\260", nullptr));
        C_servo->setText(QApplication::translate("SliderControl", "Axis C (servo C)", nullptr));
        C_range->setText(QApplication::translate("SliderControl", "range: 140\302\260 - 180\302\260", nullptr));
        C_spinBox->setSuffix(QApplication::translate("SliderControl", "\302\260", nullptr));
        G_servo->setText(QApplication::translate("SliderControl", "Gripper (servo G)", nullptr));
        G_range->setText(QApplication::translate("SliderControl", "range: 0\302\260 - 37\302\260", nullptr));
        G_spinBox->setSuffix(QApplication::translate("SliderControl", "\302\260", nullptr));
        command->setTitle(QApplication::translate("SliderControl", "Control commands", nullptr));
        sendall->setText(QApplication::translate("SliderControl", "Send all angles", nullptr));
        reset->setText(QApplication::translate("SliderControl", "Reset", nullptr));
        statue->setText(QApplication::translate("SliderControl", "Get Status", nullptr));
        limits->setText(QApplication::translate("SliderControl", "Display limits", nullptr));
        communication->setTitle(QApplication::translate("SliderControl", "Communication Log", nullptr));
        clearlog->setText(QApplication::translate("SliderControl", "Clear Log", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SliderControl: public Ui_SliderControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLIDERCONTROL_H
