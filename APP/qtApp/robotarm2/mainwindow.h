#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBluetoothDeviceInfo>
#include <QTimer>
#include "bluetooth.h"
#include "slidercontrol.h"
#include "directioncontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // UI事件处理
    void on_pushButton_clicked();      // 扫描按钮
    void on_connect_clicked();         // 连接按钮
    void on_disconnect_clicked();      // 断开连接按钮
    void on_slider_clicked();          // 滑块控制按钮
    void on_pushButton_2_clicked();    // 方向控制按钮

    // 蓝牙事件处理
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);
    void onDiscoveryFinished();
    void onBluetoothConnected();
    void onBluetoothDisconnected();
    void onBluetoothError(const QString &error);
    void onBluetoothLogMessage(const QString &message);
    void onDataReceived(const QString &data);

    // 控制窗口返回
    void onControlWindowClosed();

private:
    Ui::MainWindow *ui;

    // 蓝牙管理器
    Bluetooth *m_bluetooth;

    // 控制窗口
    SliderControl *m_sliderControl;
    DirectionControl *m_directionControl;

    // 状态管理
    bool m_isConnected;
    QList<QBluetoothDeviceInfo> m_discoveredDevices;

    // 辅助方法
    void updateUI();
    void updateConnectionStatus();
    void populateDeviceList();
    void appendLog(const QString &message);
};

#endif // MAINWINDOW_H
