#include "RobotArmController.h"
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QSplitter>

RobotArmController::RobotArmController(QWidget *parent)
    : QMainWindow(parent)
    , serialPort(nullptr)
    , readTimer(new QTimer(this))
    , isConnected(false)
{
    // 初始化关节限制信息
    jointLimits[0] = {0, 180, 90, "基座 (舵机A)", "基座旋转轴，控制机械臂左右转动"};
    jointLimits[1] = {0, 77, 0, "肩部 (舵机B)", "肩部关节，控制机械臂抬升角度"};
    jointLimits[2] = {0, 180, 180, "肘部 (舵机C)", "肘部关节，范围随肩部角度动态变化"};
    jointLimits[3] = {0, 37, 0, "抓手 (舵机G)", "末端抓手，控制抓取动作"};

    setupUI();
    setupSerialConnection();

    // 设置窗口属性
    setWindowTitle("机械臂控制器 v1.0");
    setMinimumSize(800, 600);
    resize(1000, 700);

    // 初始化滑杆位置为默认值
    for(int i = 0; i < 4; i++) {
        sliders[i]->setValue(jointLimits[i].defaultPos);
        spinBoxes[i]->setValue(jointLimits[i].defaultPos);
    }

    // 连接定时器
    connect(readTimer, &QTimer::timeout, this, &RobotArmController::readSerialData);

    logMessage("程序启动完成，请选择串口连接机械臂");
}

RobotArmController::~RobotArmController()
{
    if(serialPort && serialPort->isOpen()) {
        serialPort->close();
    }
}

void RobotArmController::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 使用分割器布局
    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);

    // === 连接控制区域 ===
    connectionGroup = new QGroupBox("串口连接", this);
    connectionLayout = new QHBoxLayout(connectionGroup);

    serialPortCombo = new QComboBox(this);
    refreshButton = new QPushButton("刷新端口", this);
    connectButton = new QPushButton("连接", this);
    disconnectButton = new QPushButton("断开", this);

    connectButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    disconnectButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; }");
    disconnectButton->setEnabled(false);

    connectionLayout->addWidget(new QLabel("端口:"));
    connectionLayout->addWidget(serialPortCombo);
    connectionLayout->addWidget(refreshButton);
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(disconnectButton);
    connectionLayout->addStretch();

    // === 关节控制区域 ===
    jointsGroup = new QGroupBox("关节角度控制", this);
    jointsLayout = new QGridLayout(jointsGroup);

    // 创建每个关节的控制界面
    for(int i = 0; i < 4; i++) {
        // 关节标签
        jointLabels[i] = new QLabel(jointLimits[i].name, this);
        jointLabels[i]->setFont(QFont("Arial", 10, QFont::Bold));

        // 范围标签
        rangeLabels[i] = new QLabel(QString("范围: %1° - %2°").arg(jointLimits[i].min).arg(jointLimits[i].max), this);
        rangeLabels[i]->setStyleSheet("color: #666; font-size: 9pt;");

        // 滑杆
        sliders[i] = new QSlider(Qt::Horizontal, this);
        sliders[i]->setMinimum(jointLimits[i].min);
        sliders[i]->setMaximum(jointLimits[i].max);
        sliders[i]->setValue(jointLimits[i].defaultPos);
        sliders[i]->setTickPosition(QSlider::TicksBelow);
        sliders[i]->setTickInterval((jointLimits[i].max - jointLimits[i].min) / 10);

        // 数值框
        spinBoxes[i] = new QSpinBox(this);
        spinBoxes[i]->setMinimum(jointLimits[i].min);
        spinBoxes[i]->setMaximum(jointLimits[i].max);
        spinBoxes[i]->setValue(jointLimits[i].defaultPos);
        spinBoxes[i]->setSuffix("°");

        // 连接信号
        connect(sliders[i], QOverload<int>::of(&QSlider::valueChanged), this, &RobotArmController::onSliderValueChanged);
        connect(spinBoxes[i], QOverload<int>::of(&QSpinBox::valueChanged), this, &RobotArmController::onSpinBoxValueChanged);

        // 布局
        jointsLayout->addWidget(jointLabels[i], i, 0);
        jointsLayout->addWidget(rangeLabels[i], i, 1);
        jointsLayout->addWidget(sliders[i], i, 2);
        jointsLayout->addWidget(spinBoxes[i], i, 3);
    }

    // === 命令控制区域 ===
    commandGroup = new QGroupBox("控制命令", this);
    commandLayout = new QHBoxLayout(commandGroup);

    sendAllButton = new QPushButton("发送所有角度", this);
    resetButton = new QPushButton("复位", this);
    statusButton = new QPushButton("获取状态", this);
    limitsButton = new QPushButton("显示限制", this);

    sendAllButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; }");
    resetButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; }");

    commandLayout->addWidget(sendAllButton);
    commandLayout->addWidget(resetButton);
    commandLayout->addWidget(statusButton);
    commandLayout->addWidget(limitsButton);
    commandLayout->addStretch();

    // === 日志区域 ===
    logGroup = new QGroupBox("通信日志", this);
    logLayout = new QVBoxLayout(logGroup);

    logTextEdit = new QTextEdit(this);
    logTextEdit->setMaximumHeight(150);
    logTextEdit->setFont(QFont("Consolas", 9));
    logTextEdit->setReadOnly(true);

    clearLogButton = new QPushButton("清除日志", this);
    clearLogButton->setMaximumWidth(100);

    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    logButtonLayout->addStretch();
    logButtonLayout->addWidget(clearLogButton);

    logLayout->addWidget(logTextEdit);
    logLayout->addLayout(logButtonLayout);

    // === 主布局组装 ===
    QWidget *topWidget = new QWidget();
    QVBoxLayout *topLayout = new QVBoxLayout(topWidget);
    topLayout->addWidget(connectionGroup);
    topLayout->addWidget(jointsGroup);
    topLayout->addWidget(commandGroup);

    mainSplitter->addWidget(topWidget);
    mainSplitter->addWidget(logGroup);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 0);

    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->addWidget(mainSplitter);

    // 连接按钮信号
    connect(refreshButton, &QPushButton::clicked, this, &RobotArmController::refreshSerialPorts);
    connect(connectButton, &QPushButton::clicked, this, &RobotArmController::connectToSerial);
    connect(disconnectButton, &QPushButton::clicked, this, &RobotArmController::disconnectFromSerial);
    connect(sendAllButton, &QPushButton::clicked, this, &RobotArmController::sendAllJointsCommand);
    connect(resetButton, &QPushButton::clicked, this, &RobotArmController::sendResetCommand);
    connect(statusButton, &QPushButton::clicked, this, &RobotArmController::sendStatusCommand);
    connect(limitsButton, &QPushButton::clicked, this, &RobotArmController::sendLimitsCommand);
    connect(clearLogButton, &QPushButton::clicked, logTextEdit, &QTextEdit::clear);

    // 刷新可用端口
    refreshSerialPorts();
}

void RobotArmController::setupSerialConnection()
{
    serialPort = new QSerialPort(this);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(serialPort, &QSerialPort::readyRead, this, &RobotArmController::readSerialData);
}

void RobotArmController::refreshSerialPorts()
{
    serialPortCombo->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        serialPortCombo->addItem(info.portName() + " - " + info.description(), info.portName());
    }

    if(serialPortCombo->count() == 0) {
        serialPortCombo->addItem("无可用端口");
        connectButton->setEnabled(false);
    } else {
        connectButton->setEnabled(true);
    }
}

void RobotArmController::connectToSerial()
{
    if(serialPortCombo->count() == 0 || serialPortCombo->itemData(0).toString().isEmpty()) {
        QMessageBox::warning(this, "连接错误", "没有可用的串口端口");
        return;
    }

    QString portName = serialPortCombo->itemData(serialPortCombo->currentIndex()).toString();
    serialPort->setPortName(portName);

    if(serialPort->open(QIODevice::ReadWrite)) {
        isConnected = true;
        connectButton->setEnabled(false);
        disconnectButton->setEnabled(true);
        serialPortCombo->setEnabled(false);
        refreshButton->setEnabled(false);

        // 启动读取定时器
        readTimer->start(100);

        logMessage(QString("成功连接到端口: %1").arg(portName));

        // 连接成功后获取一次状态
        QTimer::singleShot(1000, this, &RobotArmController::sendStatusCommand);

    } else {
        QMessageBox::critical(this, "连接错误",
            QString("无法打开端口 %1\n错误: %2").arg(portName).arg(serialPort->errorString()));
    }
}

void RobotArmController::disconnectFromSerial()
{
    if(serialPort && serialPort->isOpen()) {
        serialPort->close();
        readTimer->stop();
    }

    isConnected = false;
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    serialPortCombo->setEnabled(true);
    refreshButton->setEnabled(true);

    logMessage("串口连接已断开");
}

void RobotArmController::onSliderValueChanged()
{
    QSlider *slider = qobject_cast<QSlider*>(sender());
    if(!slider) return;

    // 找到是哪个滑杆
    int jointId = -1;
    for(int i = 0; i < 4; i++) {
        if(sliders[i] == slider) {
            jointId = i;
            break;
        }
    }

    if(jointId >= 0) {
        int value = slider->value();

        // 同步更新数值框
        spinBoxes[jointId]->blockSignals(true);
        spinBoxes[jointId]->setValue(value);
        spinBoxes[jointId]->blockSignals(false);

        // 如果是肩部关节，需要更新肘部关节的范围
        if(jointId == 1) {
            updateShoulderLimits();
        }

        // 发送单个关节命令
        if(isConnected) {
            QString command = QString("set %1 %2").arg(jointId).arg(value);
            sendCommand(command);
        }
    }
}

void RobotArmController::onSpinBoxValueChanged()
{
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    if(!spinBox) return;

    // 找到是哪个数值框
    int jointId = -1;
    for(int i = 0; i < 4; i++) {
        if(spinBoxes[i] == spinBox) {
            jointId = i;
            break;
        }
    }

    if(jointId >= 0) {
        int value = spinBox->value();

        // 同步更新滑杆
        sliders[jointId]->blockSignals(true);
        sliders[jointId]->setValue(value);
        sliders[jointId]->blockSignals(false);

        // 如果是肩部关节，需要更新肘部关节的范围
        if(jointId == 1) {
            updateShoulderLimits();
        }

        // 发送单个关节命令
        if(isConnected) {
            QString command = QString("set %1 %2").arg(jointId).arg(value);
            sendCommand(command);
        }
    }
}

void RobotArmController::updateShoulderLimits()
{
    // 根据Arduino代码中的逻辑更新肘部关节范围
    int bAngle = spinBoxes[1]->value();  // 肩部角度
    int minCAngle = 140 - bAngle;
    int maxCAngle = 180;

    if(196 - bAngle < 180) {
        maxCAngle = 196 - bAngle;
    }

    // 确保范围有效
    if(minCAngle < 0) minCAngle = 0;
    if(maxCAngle > 180) maxCAngle = 180;
    if(minCAngle > maxCAngle) minCAngle = maxCAngle;

    // 更新肘部关节的范围
    sliders[2]->setMinimum(minCAngle);
    sliders[2]->setMaximum(maxCAngle);
    spinBoxes[2]->setMinimum(minCAngle);
    spinBoxes[2]->setMaximum(maxCAngle);

    // 更新范围标签
    rangeLabels[2]->setText(QString("范围: %1° - %2° (动态)").arg(minCAngle).arg(maxCAngle));

    // 如果当前值超出新范围，调整到范围内
    int currentValue = spinBoxes[2]->value();
    if(currentValue < minCAngle) {
        sliders[2]->setValue(minCAngle);
        spinBoxes[2]->setValue(minCAngle);
    } else if(currentValue > maxCAngle) {
        sliders[2]->setValue(maxCAngle);
        spinBoxes[2]->setValue(maxCAngle);
    }
}

void RobotArmController::sendAllJointsCommand()
{
    if(!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接串口");
        return;
    }

    QString command = QString("setall %1 %2 %3 %4")
        .arg(spinBoxes[0]->value())
        .arg(spinBoxes[1]->value())
        .arg(spinBoxes[2]->value())
        .arg(spinBoxes[3]->value());

    sendCommand(command);
}

void RobotArmController::sendResetCommand()
{
    if(!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接串口");
        return;
    }

    sendCommand("reset");

    // 重置界面到默认位置
    for(int i = 0; i < 4; i++) {
        sliders[i]->blockSignals(true);
        spinBoxes[i]->blockSignals(true);

        sliders[i]->setValue(jointLimits[i].defaultPos);
        spinBoxes[i]->setValue(jointLimits[i].defaultPos);

        sliders[i]->blockSignals(false);
        spinBoxes[i]->blockSignals(false);
    }

    // 重置肘部范围
    updateShoulderLimits();
}

void RobotArmController::sendStatusCommand()
{
    if(!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接串口");
        return;
    }

    sendCommand("status");
}

void RobotArmController::sendLimitsCommand()
{
    if(!isConnected) {
        QMessageBox::warning(this, "错误", "请先连接串口");
        return;
    }

    sendCommand("limits");
}

void RobotArmController::sendCommand(const QString &command)
{
    if(!serialPort || !serialPort->isOpen()) {
        logMessage("错误: 串口未连接");
        return;
    }

    QString cmdWithNewline = command + "\n";
    qint64 bytesWritten = serialPort->write(cmdWithNewline.toUtf8());

    if(bytesWritten > 0) {
        lastCommand = command;
        logMessage(QString("发送: %1").arg(command));
    } else {
        logMessage(QString("发送失败: %1").arg(command));
    }
}

void RobotArmController::readSerialData()
{
    if(!serialPort) return;

    QByteArray data = serialPort->readAll();
    if(!data.isEmpty()) {
        QString received = QString::fromUtf8(data).trimmed();
        if(!received.isEmpty()) {
            logMessage(QString("接收: %1").arg(received));
        }
    }
}

void RobotArmController::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);

    logTextEdit->append(logEntry);

    // 自动滚动到底部
    QTextCursor cursor = logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    logTextEdit->setTextCursor(cursor);
}
