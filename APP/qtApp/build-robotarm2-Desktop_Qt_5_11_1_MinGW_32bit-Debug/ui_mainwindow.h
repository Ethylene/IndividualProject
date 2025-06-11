/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *titleLabel;
    QGroupBox *Bluetooth;
    QWidget *widget;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QWidget *widget_2;
    QLabel *label_2;
    QPushButton *connect;
    QPushButton *disconnect;
    QGroupBox *connection;
    QWidget *widget_3;
    QPushButton *slider;
    QPushButton *pushButton_2;
    QGroupBox *Log;
    QTextEdit *textEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(600, 500);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        titleLabel = new QLabel(centralWidget);
        titleLabel->setObjectName(QStringLiteral("titleLabel"));
        titleLabel->setGeometry(QRect(190, 10, 201, 16));
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setMargin(20);
        Bluetooth = new QGroupBox(centralWidget);
        Bluetooth->setObjectName(QStringLiteral("Bluetooth"));
        Bluetooth->setGeometry(QRect(0, 80, 331, 121));
        widget = new QWidget(Bluetooth);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 20, 311, 51));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 101, 16));
        comboBox = new QComboBox(widget);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(130, 10, 73, 22));
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(210, 10, 93, 28));
        pushButton->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #4CAF50; \n"
"    color: white; \n"
"    border: none; \n"
"    padding: 8px 16px; \n"
"    border-radius: 4px; \n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #45a049; \n"
"}"));
        widget_2 = new QWidget(Bluetooth);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(0, 80, 321, 31));
        label_2 = new QLabel(widget_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 10, 131, 16));
        connect = new QPushButton(widget_2);
        connect->setObjectName(QStringLiteral("connect"));
        connect->setGeometry(QRect(140, 0, 81, 28));
        connect->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #2196F3; \n"
"    color: white; \n"
"    border: none; \n"
"    padding: 8px 16px; \n"
"    border-radius: 4px; \n"
"}\n"
"QPushButton:disabled { \n"
"    background-color: #cccccc; \n"
"}"));
        disconnect = new QPushButton(widget_2);
        disconnect->setObjectName(QStringLiteral("disconnect"));
        disconnect->setGeometry(QRect(230, 0, 93, 28));
        disconnect->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #f44336; \n"
"    color: white; \n"
"    border: none; \n"
"    padding: 8px 16px; \n"
"    border-radius: 4px; \n"
"}\n"
"QPushButton:disabled { \n"
"    background-color: #cccccc; \n"
"}"));
        connection = new QGroupBox(centralWidget);
        connection->setObjectName(QStringLiteral("connection"));
        connection->setGeometry(QRect(340, 70, 251, 151));
        widget_3 = new QWidget(connection);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        widget_3->setEnabled(true);
        widget_3->setGeometry(QRect(20, 10, 231, 131));
        slider = new QPushButton(widget_3);
        slider->setObjectName(QStringLiteral("slider"));
        slider->setEnabled(true);
        slider->setGeometry(QRect(10, 10, 201, 51));
        slider->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #FF9800; \n"
"    color: white; \n"
"    border: none; \n"
"    padding: 16px; \n"
"    border-radius: 8px; \n"
"}\n"
"QPushButton:disabled { \n"
"    background-color: #cccccc; \n"
"}"));
        pushButton_2 = new QPushButton(widget_3);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 80, 201, 51));
        pushButton_2->setStyleSheet(QLatin1String("QPushButton { \n"
"    background-color: #9C27B0; \n"
"    color: white; \n"
"    border: none; \n"
"    padding: 16px; \n"
"    border-radius: 8px; \n"
"}\n"
"QPushButton:disabled { \n"
"    background-color: #cccccc; \n"
"}"));
        Log = new QGroupBox(centralWidget);
        Log->setObjectName(QStringLiteral("Log"));
        Log->setGeometry(QRect(60, 240, 461, 181));
        textEdit = new QTextEdit(Log);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(30, 20, 411, 151));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 26));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        MainWindow->insertToolBarBreak(mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        titleLabel->setText(QApplication::translate("MainWindow", "Robot Arm Controller", nullptr));
        Bluetooth->setTitle(QApplication::translate("MainWindow", "Bluetooth Set", nullptr));
        label->setText(QApplication::translate("MainWindow", "Select Equipment", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Scan", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Status: Not connected", nullptr));
        connect->setText(QApplication::translate("MainWindow", "connect", nullptr));
        disconnect->setText(QApplication::translate("MainWindow", "disconnect", nullptr));
        connection->setTitle(QApplication::translate("MainWindow", "Control Mode", nullptr));
        slider->setText(QApplication::translate("MainWindow", "Slider Control", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "Direction Control", nullptr));
        Log->setTitle(QApplication::translate("MainWindow", "Connection Log", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
