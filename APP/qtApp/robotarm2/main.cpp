#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序信息
    a.setApplicationName("Robot Arm Controller");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Yingxi Chen");

    // 设置应用程序图标（如果有的话）
    // a.setWindowIcon(QIcon(":/icons/robot_arm.png"));

    // 创建并显示主窗口
    MainWindow w;
    w.show();

    return a.exec();
}
