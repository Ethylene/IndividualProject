#include "bluetooth.h"

// ESP32蓝牙服务的UUID (与ESP32代码中的UUID保持一致)
const QString Bluetooth::SERVICE_UUID = "b9c7eaa3-f8de-4f40-9ad3-4e7e49bd431a";
const QString Bluetooth::CHAR_UUID_RX = "d05182d7-e26f-4bfc-8a69-6e3efb177358";
const QString Bluetooth::CHAR_UUID_TX = "a0c4ca34-247c-4651-86a9-bf61b28b7bff";
const QString Bluetooth::DEVICE_NAME = "RobotArm";

Bluetooth::Bluetooth(QObject *parent)
    : QObject(parent)
    , m_discoveryAgent(nullptr)
    , m_localDevice(nullptr)
    , m_controller(nullptr)
    , m_service(nullptr)
    , m_connected(false)
{
    // 初始化本地蓝牙设备
    m_localDevice = new QBluetoothLocalDevice(this);
    if (!m_localDevice->isValid()) {
        log("本地蓝牙设备无效");
        return;
    }

    // 确保蓝牙已开启
    if (m_localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        log("蓝牙已关闭，请开启蓝牙");
        return;
    }

    // 初始化设备发现代理
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_discoveryAgent->setLowEnergyDiscoveryTimeout(10000); // 10秒扫描超时

    // 连接设备发现信号
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &Bluetooth::onDeviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &Bluetooth::onDiscoveryFinished);
    connect(m_discoveryAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
            this, &Bluetooth::onDiscoveryError);

    log("蓝牙管理器初始化完成");
}

Bluetooth::~Bluetooth()
{
    disconnectFromDevice();
}

void Bluetooth::startDeviceDiscovery()
{
    if (!m_discoveryAgent) {
        log("设备发现代理未初始化");
        return;
    }

    m_discoveredDevices.clear();
    log("开始扫描蓝牙设备...");
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void Bluetooth::stopDeviceDiscovery()
{
    if (m_discoveryAgent && m_discoveryAgent->isActive()) {
        m_discoveryAgent->stop();
        log("停止扫描蓝牙设备");
    }
}

void Bluetooth::connectToDevice(const QBluetoothDeviceInfo &device)
{
    if (m_connected) {
        log("已连接到设备，请先断开连接");
        return;
    }

    log(QString("正在连接到设备: %1 (%2)").arg(device.name()).arg(device.address().toString()));
    setupController(device);
}

void Bluetooth::disconnectFromDevice()
{
    if (m_controller) {
        m_controller->disconnectFromDevice();
    }
}

void Bluetooth::sendCommand(const QString &command)
{
    if (!m_connected || !m_service || !m_rxCharacteristic.isValid()) {
        log("未连接到设备或服务未准备好");
        return;
    }

    QByteArray data = command.toUtf8();
    m_service->writeCharacteristic(m_rxCharacteristic, data);
    log(QString("发送命令: %1").arg(command));
    emit commandSent(command);
}

bool Bluetooth::isConnected() const
{
    return m_connected;
}

QList<QBluetoothDeviceInfo> Bluetooth::getDiscoveredDevices() const
{
    return m_discoveredDevices;
}

void Bluetooth::onDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    // 过滤掉不是我们目标设备的设备
    if (device.name().isEmpty() || device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        // 检查是否是我们的机械臂设备
        if (device.name() == DEVICE_NAME || device.name().contains("RobotArm")) {
            log(QString("发现设备: %1 (%2)").arg(device.name()).arg(device.address().toString()));
            m_discoveredDevices.append(device);
            emit deviceDiscovered(device);
        }
    }
}

void Bluetooth::onDiscoveryFinished()
{
    log(QString("扫描完成，发现 %1 个目标设备").arg(m_discoveredDevices.size()));
    emit discoveryFinished();
}

void Bluetooth::onDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    QString errorMsg;
    switch (error) {
    case QBluetoothDeviceDiscoveryAgent::PoweredOffError:
        errorMsg = "蓝牙已关闭";
        break;
    case QBluetoothDeviceDiscoveryAgent::InputOutputError:
        errorMsg = "输入输出错误";
        break;
    case QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError:
        errorMsg = "无效的蓝牙适配器";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnsupportedPlatformError:
        errorMsg = "平台不支持";
        break;
    default:
        errorMsg = "未知错误";
        break;
    }
    log(QString("设备发现错误: %1").arg(errorMsg));
}

void Bluetooth::setupController(const QBluetoothDeviceInfo &device)
{
    // 清理之前的连接
    if (m_controller) {
        m_controller->deleteLater();
        m_controller = nullptr;
    }

    // 创建新的控制器
    m_controller = QLowEnergyController::createCentral(device, this);

    // 连接控制器信号
    connect(m_controller, &QLowEnergyController::connected,
            this, &Bluetooth::onControllerConnected);
    connect(m_controller, &QLowEnergyController::disconnected,
            this, &Bluetooth::onControllerDisconnected);
    connect(m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            this, &Bluetooth::onControllerError);
    connect(m_controller, &QLowEnergyController::stateChanged,
            this, &Bluetooth::onControllerStateChanged);
    connect(m_controller, &QLowEnergyController::serviceDiscovered,
            this, &Bluetooth::onServiceDiscovered);

    // 开始连接
    m_controller->connectToDevice();
}

void Bluetooth::onControllerConnected()
{
    log("蓝牙控制器已连接，开始发现服务...");
    m_controller->discoverServices();
}

void Bluetooth::onControllerDisconnected()
{
    log("蓝牙控制器已断开连接");
    m_connected = false;
    if (m_service) {
        m_service->deleteLater();
        m_service = nullptr;
    }
    emit disconnected();
}

void Bluetooth::onControllerError(QLowEnergyController::Error error)
{
    QString errorMsg;
    switch (error) {
    case QLowEnergyController::UnknownError:
        errorMsg = "未知错误";
        break;
    case QLowEnergyController::UnknownRemoteDeviceError:
        errorMsg = "未知远程设备错误";
        break;
    case QLowEnergyController::NetworkError:
        errorMsg = "网络错误";
        break;
    case QLowEnergyController::InvalidBluetoothAdapterError:
        errorMsg = "无效的蓝牙适配器";
        break;
    case QLowEnergyController::ConnectionError:
        errorMsg = "连接错误";
        break;
    case QLowEnergyController::AdvertisingError:
        errorMsg = "广播错误";
        break;
    case QLowEnergyController::RemoteHostClosedError:
        errorMsg = "远程主机关闭连接";
        break;
    default:
        errorMsg = "未知错误";
        break;
    }

    log(QString("控制器错误: %1").arg(errorMsg));
    emit connectionError(errorMsg);
}

void Bluetooth::onControllerStateChanged(QLowEnergyController::ControllerState state)
{
    QString stateMsg;
    switch (state) {
    case QLowEnergyController::UnconnectedState:
        stateMsg = "未连接";
        break;
    case QLowEnergyController::ConnectingState:
        stateMsg = "正在连接";
        break;
    case QLowEnergyController::ConnectedState:
        stateMsg = "已连接";
        break;
    case QLowEnergyController::DiscoveringState:
        stateMsg = "正在发现服务";
        break;
    case QLowEnergyController::DiscoveredState:
        stateMsg = "服务发现完成";
        break;
    case QLowEnergyController::ClosingState:
        stateMsg = "正在关闭连接";
        break;
    case QLowEnergyController::AdvertisingState:
        stateMsg = "正在广播";
        break;
    default:
        stateMsg = "未知状态";
        break;
    }
    log(QString("控制器状态: %1").arg(stateMsg));
}

void Bluetooth::onServiceDiscovered(const QBluetoothUuid &serviceUuid)
{
    log(QString("发现服务: %1").arg(serviceUuid.toString()));

    // 检查是否是我们需要的服务
    if (serviceUuid.toString().toLower() == QString("{%1}").arg(SERVICE_UUID).toLower()) {
        log("找到目标服务，正在连接...");
        setupService();
    }
}

void Bluetooth::setupService()
{
    if (!m_controller) {
        return;
    }

    // 获取服务
    QBluetoothUuid serviceUuid(SERVICE_UUID);
    m_service = m_controller->createServiceObject(serviceUuid, this);

    if (!m_service) {
        log("无法创建服务对象");
        return;
    }

    // 连接服务信号
    connect(m_service, &QLowEnergyService::stateChanged,
            this, &Bluetooth::onServiceDetailsDiscovered);
    connect(m_service, &QLowEnergyService::characteristicChanged,
            this, &Bluetooth::onCharacteristicChanged);
    connect(m_service, &QLowEnergyService::characteristicWritten,
            this, &Bluetooth::onCharacteristicWritten);

    // 发现服务详细信息
    m_service->discoverDetails();
}

void Bluetooth::onServiceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::ServiceDiscovered) {
        log("服务详细信息发现完成");

        // 获取特征值
        QBluetoothUuid rxUuid(CHAR_UUID_RX);
        QBluetoothUuid txUuid(CHAR_UUID_TX);

        m_rxCharacteristic = m_service->characteristic(rxUuid);
        m_txCharacteristic = m_service->characteristic(txUuid);

        if (m_rxCharacteristic.isValid()) {
            log("RX特征值准备就绪");
        } else {
            log("RX特征值无效");
        }

        if (m_txCharacteristic.isValid()) {
            log("TX特征值准备就绪");

            // 启用通知
            QLowEnergyDescriptor notification = m_txCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (notification.isValid()) {
                m_service->writeDescriptor(notification, QByteArray::fromHex("0100"));
                log("已启用TX特征值通知");
            }
        } else {
            log("TX特征值无效");
        }

        if (m_rxCharacteristic.isValid() && m_txCharacteristic.isValid()) {
            m_connected = true;
            log("蓝牙连接成功，准备就绪");
            emit connected();
        } else {
            log("特征值无效，连接失败");
            emit connectionError("特征值无效");
        }
    }
}

void Bluetooth::onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    if (characteristic.uuid() == QBluetoothUuid(CHAR_UUID_TX)) {
        QString data = QString::fromUtf8(newValue);
        log(QString("接收数据: %1").arg(data));
        emit dataReceived(data);
    }
}

void Bluetooth::onCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    if (characteristic.uuid() == QBluetoothUuid(CHAR_UUID_RX)) {
        log("数据发送成功");
    }
}

void Bluetooth::log(const QString &message)
{
    qDebug() << "Bluetooth:" << message;
    emit logMessage(message);
}
