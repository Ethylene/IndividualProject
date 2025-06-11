QT += core widgets serialport

CONFIG += c++11

TARGET = RobotArmController
TEMPLATE = app

SOURCES += \
    main.cpp \
    RobotArmController.cpp

HEADERS += \
    RobotArmController.h \
    mainwindow.h

# 设置应用程序图标（可选）
# win32:RC_ICONS = icon.ico

# 设置版本信息
VERSION = 1.0.0

# 编译器设置
QMAKE_CXXFLAGS += -std=c++11

# 输出目录
DESTDIR = $$PWD/build

FORMS += \
    mainwindow.ui
