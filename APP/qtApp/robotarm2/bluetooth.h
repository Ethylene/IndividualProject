#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>
#include <QTimer>
#include <QDebug>

class Bluetooth : public QObject
{
    Q_OBJECT

public:
    explicit Bluetooth(QObject *parent = nullptr);
    ~Bluetooth();

    // 蓝牙操作
    void startDeviceDiscovery();
    void stopDeviceDiscovery();
    void connectToDevice(const QBluetoothDeviceInfo &device);
    void disconnectFromDevice();

    // 命令发送
    void sendCommand(const QString &command);

    // 状态查询
    bool isConnected() const;
    QList<QBluetoothDeviceInfo> getDiscoveredDevices() const;

signals:
    // 设备发现信号
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void discoveryFinished();

    // 连接状态信号
    void connected();
    void disconnected();
    void connectionError(const QString &error);

    // 数据通信信号
    void dataReceived(const QString &data);
    void commandSent(const QString &command);

    // 日志信号
    void logMessage(const QString &message);

private slots:
    // 设备发现相关
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);
    void onDiscoveryFinished();
    void onDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error);

    // 连接相关
    void onControllerConnected();
    void onControllerDisconnected();
    void onControllerError(QLowEnergyController::Error error);
    void onControllerStateChanged(QLowEnergyController::ControllerState state);

    // 服务相关
    void onServiceDiscovered(const QBluetoothUuid &serviceUuid);
    void onServiceDetailsDiscovered(QLowEnergyService::ServiceState newState);

    // 特征值相关
    void onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void onCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);

private:
    // ESP32蓝牙服务的UUID (与ESP32代码中的UUID保持一致)
    static const QString SERVICE_UUID;
    static const QString CHAR_UUID_RX;  // ESP32接收数据的特征值
    static const QString CHAR_UUID_TX;  // ESP32发送数据的特征值
    static const QString DEVICE_NAME;   // ESP32设备名称

    // 蓝牙组件
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QBluetoothLocalDevice *m_localDevice;
    QLowEnergyController *m_controller;
    QLowEnergyService *m_service;

    // 特征值
    QLowEnergyCharacteristic m_rxCharacteristic;  // 向ESP32发送数据
    QLowEnergyCharacteristic m_txCharacteristic;  // 从ESP32接收数据

    // 状态变量
    bool m_connected;
    QList<QBluetoothDeviceInfo> m_discoveredDevices;

    // 辅助方法
    void setupController(const QBluetoothDeviceInfo &device);
    void setupService();
    void log(const QString &message);
};



#endif // BLUETOOTH_H
