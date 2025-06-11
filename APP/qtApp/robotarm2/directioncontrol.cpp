#include "directioncontrol.h"
#include "ui_directioncontrol.h"
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>

DirectionControl::DirectionControl(Bluetooth *bluetooth, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DirectionControl)
    , m_bluetooth(bluetooth)
    , m_moveTimer(new QTimer(this))
    , m_currentDirection(MOVE_STOP)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("Direction Control - Robot Arm");

    // 初始化位置
    initializePosition();

    // 设置定时器
    m_moveTimer->setSingleShot(false);
    connect(m_moveTimer, &QTimer::timeout, this, &DirectionControl::onMoveTimer);

    // 连接蓝牙管理器信号
    if (m_bluetooth) {
        connect(m_bluetooth, &Bluetooth::dataReceived,
                this, &DirectionControl::onDataReceived);
        connect(m_bluetooth, &Bluetooth::logMessage,
                this, &DirectionControl::onBluetoothLogMessage);
    }

    appendLog("方向控制界面已启动");
    appendLog("使用方向键控制机械臂移动，长按持续移动");

    // 获取当前状态
    sendCommand("status");
}

DirectionControl::~DirectionControl()
{
    stopMoving();
    delete ui;
}

void DirectionControl::closeEvent(QCloseEvent *event)
{
    stopMoving();
    emit windowClosed();
    event->accept();
}

void DirectionControl::initializePosition()
{
    // 初始化为默认位置
    m_currentPosition.aAngle = 90;   // 基座
    m_currentPosition.bAngle = 0;    // 肩部
    m_currentPosition.cAngle = 180;  // 肘部
    m_currentPosition.gAngle = 0;    // 抓手
}

void DirectionControl::on_bacl_clicked()
{
    // 返回按钮
    close();
}

void DirectionControl::on_reset_clicked()
{
    // 重置按钮
    stopMoving();
    sendCommand("reset");
    initializePosition();
    appendLog("发送重置命令");
}

void DirectionControl::on_statue_clicked()
{
    // 获取状态
    sendCommand("status");
    appendLog("请求机械臂状态");
}

void DirectionControl::on_limits_clicked()
{
    // 显示限制
    sendCommand("limits");
    appendLog("请求角度限制信息");
}

void DirectionControl::on_clearlog_clicked()
{
    // 清除日志
    ui->textEdit->clear();
    appendLog("日志已清除");
}

void DirectionControl::on_up_pressed()
{
    // 上按钮按下 - 机械臂向上移动 (增加B角度)
    startMoving(MOVE_UP);
    appendLog("开始向上移动");
}

void DirectionControl::on_up_released()
{
    // 上按钮释放
    stopMoving();
    appendLog("停止向上移动");
}

void DirectionControl::on_down_pressed()
{
    // 下按钮按下 - 机械臂向下移动 (减少B角度)
    startMoving(MOVE_DOWN);
    appendLog("开始向下移动");
}

void DirectionControl::on_down_released()
{
    // 下按钮释放
    stopMoving();
    appendLog("停止向下移动");
}

void DirectionControl::on_left_pressed()
{
    // 左按钮按下 - 机械臂向左转 (减少A角度)
    startMoving(MOVE_LEFT);
    appendLog("开始向左转动");
}

void DirectionControl::on_left_released()
{
    // 左按钮释放
    stopMoving();
    appendLog("停止向左转动");
}

void DirectionControl::on_right_pressed()
{
    // 右按钮按下 - 机械臂向右转 (增加A角度)
    startMoving(MOVE_RIGHT);
    appendLog("开始向右转动");
}

void DirectionControl::on_right_released()
{
    // 右按钮释放
    stopMoving();
    appendLog("停止向右转动");
}

void DirectionControl::on_stop_clicked()
{
    // 停止按钮
    stopMoving();
    appendLog("紧急停止");
}

void DirectionControl::on_open_clicked()
{
    // 打开抓手
    sendCommand("set 3 37");  // 抓手关节ID为3，37度为打开
    m_currentPosition.gAngle = 37;
    appendLog("打开抓手");
}

void DirectionControl::on_grip_clicked()
{
    // 抓取
    sendCommand("set 3 0");  // 抓手关节ID为3，0度为抓取
    m_currentPosition.gAngle = 0;
    appendLog("抓手抓取");
}

void DirectionControl::startMoving(MoveDirection direction)
{
    m_currentDirection = direction;
    if (!m_moveTimer->isActive()) {
        m_moveTimer->start(MOVE_INTERVAL);
    }
}

void DirectionControl::stopMoving()
{
    m_currentDirection = MOVE_STOP;
    if (m_moveTimer->isActive()) {
        m_moveTimer->stop();
    }
}

void DirectionControl::onMoveTimer()
{
    if (m_currentDirection != MOVE_STOP) {
        moveInDirection(m_currentDirection);
    }
}

void DirectionControl::moveInDirection(MoveDirection direction)
{
    int newAngle = 0;
    int jointId = 0;
    bool isValid = false;

    switch (direction) {
    case MOVE_UP:
        // 向上移动 - 增加B角度 (肩部向上)
        jointId = 1;
        newAngle = m_currentPosition.bAngle + MOVE_STEP;
        isValid = isAngleValid(jointId, newAngle);
        if (isValid) {
            m_currentPosition.bAngle = newAngle;
        }
        break;

    case MOVE_DOWN:
        // 向下移动 - 减少B角度 (肩部向下)
        jointId = 1;
        newAngle = m_currentPosition.bAngle - MOVE_STEP;
        isValid = isAngleValid(jointId, newAngle);
        if (isValid) {
            m_currentPosition.bAngle = newAngle;
        }
        break;

    case MOVE_LEFT:
        // 向左转 - 减少A角度 (基座向左)
        jointId = 0;
        newAngle = m_currentPosition.aAngle - MOVE_STEP;
        isValid = isAngleValid(jointId, newAngle);
        if (isValid) {
            m_currentPosition.aAngle = newAngle;
        }
        break;

    case MOVE_RIGHT:
        // 向右转 - 增加A角度 (基座向右)
        jointId = 0;
        newAngle = m_currentPosition.aAngle + MOVE_STEP;
        isValid = isAngleValid(jointId, newAngle);
        if (isValid) {
            m_currentPosition.aAngle = newAngle;
        }
        break;

    default:
        return;
    }

    if (isValid) {
        // 发送移动命令
        QString command = QString("set %1 %2").arg(jointId).arg(newAngle);
        sendCommand(command);
    } else {
        // 到达限制，停止移动
        stopMoving();
        appendLog(QString("到达关节 %1 的角度限制").arg(jointId));
    }
}

bool DirectionControl::isAngleValid(int joint, int angle)
{
    switch (joint) {
    case 0: // 基座 (A轴)
        return (angle >= 0 && angle <= 180);

    case 1: // 肩部 (B轴)
        return (angle >= 0 && angle <= 77);

    case 2: // 肘部 (C轴)
        {
            int minC = calculateCAngleRange(m_currentPosition.bAngle, true);
            int maxC = calculateCAngleRange(m_currentPosition.bAngle, false);
            return (angle >= minC && angle <= maxC);
        }

    case 3: // 抓手 (G轴)
        return (angle >= 0 && angle <= 37);

    default:
        return false;
    }
}

int DirectionControl::calculateCAngleRange(int bAngle, bool getMin)
{
    // 根据ESP32代码中的公式计算C轴范围
    int minC = 140 - bAngle;
    int maxC = 180;

    if (196 - bAngle < 180) {
        maxC = 196 - bAngle;
    }

    // 确保范围合理
    if (minC < 0) minC = 0;
    if (maxC > 180) maxC = 180;
    if (minC > maxC) minC = maxC;

    return getMin ? minC : maxC;
}

void DirectionControl::onDataReceived(const QString &data)
{
    appendLog(QString("收到数据: %1").arg(data));

    // 解析状态数据
    if (data.contains(",") && data.split(",").size() == 4) {
        updatePositionFromStatus(data);
    }
}

void DirectionControl::onBluetoothLogMessage(const QString &message)
{
    // 选择性显示重要的蓝牙日志
    if (message.contains("错误") || message.contains("失败") || message.contains("断开")) {
        appendLog(message);
    }
}

void DirectionControl::updatePositionFromStatus(const QString &statusData)
{
    // 解析状态数据并更新当前位置 (格式: "90,45,150,20")
    QStringList angles = statusData.split(",");
    if (angles.size() == 4) {
        bool ok1, ok2, ok3, ok4;
        int aAngle = angles[0].toInt(&ok1);
        int bAngle = angles[1].toInt(&ok2);
        int cAngle = angles[2].toInt(&ok3);
        int gAngle = angles[3].toInt(&ok4);

        if (ok1 && ok2 && ok3 && ok4) {
            m_currentPosition.aAngle = aAngle;
            m_currentPosition.bAngle = bAngle;
            m_currentPosition.cAngle = cAngle;
            m_currentPosition.gAngle = gAngle;

            appendLog(QString("位置更新: A=%1°, B=%2°, C=%3°, G=%4°")
                     .arg(aAngle).arg(bAngle).arg(cAngle).arg(gAngle));
        }
    }
}

void DirectionControl::sendCommand(const QString &command)
{
    if (m_bluetooth && m_bluetooth->isConnected()) {
        m_bluetooth->sendCommand(command);
    } else {
        appendLog("错误: 未连接到设备");
        stopMoving(); // 如果连接断开，停止移动
        QMessageBox::warning(this, "警告", "未连接到机械臂设备");
    }
}

void DirectionControl::appendLog(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    ui->textEdit->append(logEntry);

    // 自动滚动到底部
    ui->textEdit->moveCursor(QTextCursor::End);
}
