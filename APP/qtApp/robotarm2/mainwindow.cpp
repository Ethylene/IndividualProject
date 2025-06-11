#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bluetooth(nullptr)
    , m_sliderControl(nullptr)
    , m_directionControl(nullptr)
    , m_isConnected(false)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("Robot Arm Controller");

    // 初始化蓝牙管理器
    m_bluetooth = new Bluetooth(this);

    // 连接蓝牙管理器信号
    connect(m_bluetooth, &Bluetooth::deviceDiscovered,
            this, &MainWindow::onDeviceDiscovered);
    connect(m_bluetooth, &Bluetooth::discoveryFinished,
            this, &MainWindow::onDiscoveryFinished);
    connect(m_bluetooth, &Bluetooth::connected,
            this, &MainWindow::onBluetoothConnected);
    connect(m_bluetooth, &Bluetooth::disconnected,
            this, &MainWindow::onBluetoothDisconnected);
    connect(m_bluetooth, &Bluetooth::connectionError,
            this, &MainWindow::onBluetoothError);
    connect(m_bluetooth, &Bluetooth::logMessage,
            this, &MainWindow::onBluetoothLogMessage);
    connect(m_bluetooth, &Bluetooth::dataReceived,
            this, &MainWindow::onDataReceived);

    // 初始化UI状态
    updateUI();

    // 在日志中显示欢迎信息
    appendLog("机械臂控制系统已启动");
    appendLog("请点击'Scan'扫描蓝牙设备");
}

MainWindow::~MainWindow()
{
    // 清理资源
    if (m_sliderControl) {
        m_sliderControl->deleteLater();
    }
    if (m_directionControl) {
        m_directionControl->deleteLater();
    }
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    // 扫描按钮被点击
    appendLog("开始扫描蓝牙设备...");
    ui->comboBox->clear();
    m_discoveredDevices.clear();
    m_bluetooth->startDeviceDiscovery();

    // 更新按钮状态
    ui->pushButton->setEnabled(false);
    ui->pushButton->setText("Scanning...");
}

void MainWindow::on_connect_clicked()
{
    // 连接按钮被点击
    int currentIndex = ui->comboBox->currentIndex();
    if (currentIndex < 0 || currentIndex >= m_discoveredDevices.size()) {
        QMessageBox::warning(this, "警告", "请先选择要连接的设备");
        return;
    }

    QBluetoothDeviceInfo device = m_discoveredDevices.at(currentIndex);
    appendLog(QString("正在连接到设备: %1").arg(device.name()));

    m_bluetooth->connectToDevice(device);

    // 更新按钮状态
    ui->connect->setEnabled(false);
    ui->connect->setText("Connecting...");
}

void MainWindow::on_disconnect_clicked()
{
    // 断开连接按钮被点击
    appendLog("正在断开连接...");
    m_bluetooth->disconnectFromDevice();
}

void MainWindow::on_slider_clicked()
{
    // 滑块控制按钮被点击
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "请先连接到机械臂设备");
        return;
    }

    if (!m_sliderControl) {
        m_sliderControl = new SliderControl(m_bluetooth);
        connect(m_sliderControl, &SliderControl::windowClosed,
                this, &MainWindow::onControlWindowClosed);
    }

    m_sliderControl->show();
    this->hide();
}

void MainWindow::on_pushButton_2_clicked()
{
    // 方向控制按钮被点击
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "请先连接到机械臂设备");
        return;
    }

    if (!m_directionControl) {
        m_directionControl = new DirectionControl(m_bluetooth);
        connect(m_directionControl, &DirectionControl::windowClosed,
                this, &MainWindow::onControlWindowClosed);
    }

    m_directionControl->show();
    this->hide();
}

void MainWindow::onDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    // 发现新设备
    m_discoveredDevices.append(device);
    QString deviceInfo = QString("%1 (%2)").arg(device.name()).arg(device.address().toString());
    ui->comboBox->addItem(deviceInfo);
    appendLog(QString("发现设备: %1").arg(deviceInfo));
}

void MainWindow::onDiscoveryFinished()
{
    // 扫描完成
    appendLog(QString("扫描完成，共发现 %1 个设备").arg(m_discoveredDevices.size()));

    // 恢复按钮状态
    ui->pushButton->setEnabled(true);
    ui->pushButton->setText("Scan");

    // 如果发现了设备，启用连接按钮
    if (!m_discoveredDevices.isEmpty()) {
        ui->connect->setEnabled(true);
    }
}

void MainWindow::onBluetoothConnected()
{
    // 蓝牙连接成功
    m_isConnected = true;
    appendLog("蓝牙连接成功！");
    updateConnectionStatus();
    updateUI();
}

void MainWindow::onBluetoothDisconnected()
{
    // 蓝牙连接断开
    m_isConnected = false;
    appendLog("蓝牙连接已断开");
    updateConnectionStatus();
    updateUI();
}

void MainWindow::onBluetoothError(const QString &error)
{
    // 蓝牙连接错误
    appendLog(QString("蓝牙错误: %1").arg(error));

    // 恢复连接按钮状态
    ui->connect->setEnabled(true);
    ui->connect->setText("connect");

    QMessageBox::critical(this, "连接错误", QString("蓝牙连接失败: %1").arg(error));
}

void MainWindow::onBluetoothLogMessage(const QString &message)
{
    // 蓝牙日志消息
    appendLog(message);
}

void MainWindow::onDataReceived(const QString &data)
{
    // 接收到数据
    appendLog(QString("收到数据: %1").arg(data));
}

void MainWindow::onControlWindowClosed()
{
    // 控制窗口关闭，显示主窗口
    this->show();
}

void MainWindow::updateUI()
{
    // 更新UI状态
    updateConnectionStatus();

    // 根据连接状态启用/禁用控制按钮
    ui->slider->setEnabled(m_isConnected);
    ui->pushButton_2->setEnabled(m_isConnected);

    // 根据连接状态设置断开连接按钮
    ui->disconnect->setEnabled(m_isConnected);

    // 如果已连接，禁用连接相关控件
    if (m_isConnected) {
        ui->connect->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->comboBox->setEnabled(false);
    } else {
        ui->connect->setEnabled(!m_discoveredDevices.isEmpty());
        ui->pushButton->setEnabled(true);
        ui->comboBox->setEnabled(true);
        ui->connect->setText("connect");
    }
}

void MainWindow::updateConnectionStatus()
{
    // 更新连接状态显示
    if (m_isConnected) {
        ui->label_2->setText("Status: Connected");
        ui->label_2->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->label_2->setText("Status: Not connected");
        ui->label_2->setStyleSheet("color: red;");
    }
}

void MainWindow::populateDeviceList()
{
    // 填充设备列表
    ui->comboBox->clear();
    for (const QBluetoothDeviceInfo &device : m_discoveredDevices) {
        QString deviceInfo = QString("%1 (%2)").arg(device.name()).arg(device.address().toString());
        ui->comboBox->addItem(deviceInfo);
    }
}

void MainWindow::appendLog(const QString &message)
{
    // 添加日志消息
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    ui->textEdit->append(logEntry);

    // 自动滚动到底部
    ui->textEdit->moveCursor(QTextCursor::End);
}
