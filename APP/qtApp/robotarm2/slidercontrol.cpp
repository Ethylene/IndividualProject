#include "slidercontrol.h"
#include "ui_slidercontrol.h"
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>

SliderControl::SliderControl(Bluetooth *bluetooth, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SliderControl)
    , m_bluetooth(bluetooth)
    , m_updatingSliders(false)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("Slider Control - Robot Arm");

    // 设置初始值
    ui->A_slider->setValue(90);   // 基座默认90度
    ui->B_slider->setValue(0);    // 肩部默认0度
    ui->C_slider->setValue(180);  // 肘部默认180度
    ui->G_slider->setValue(0);    // 抓手默认0度

    ui->A_spinBox->setValue(90);
    ui->B_spinBox->setValue(0);
    ui->C_spinBox->setValue(180);
    ui->G_spinBox->setValue(0);

    // 设置连接
    setupConnections();

    // 连接蓝牙管理器信号
    if (m_bluetooth) {
        connect(m_bluetooth, &Bluetooth::dataReceived,
                this, &SliderControl::onDataReceived);
        connect(m_bluetooth, &Bluetooth::logMessage,
                this, &SliderControl::onBluetoothLogMessage);
    }

    appendLog("滑块控制界面已启动");
    appendLog("调整滑块设置关节角度，点击'Send all angles'发送到机械臂");
}

SliderControl::~SliderControl()
{
    delete ui;
}

void SliderControl::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}

void SliderControl::setupConnections()
{
    // 滑块值变化连接
    connect(ui->A_slider, QOverload<int>::of(&QSlider::valueChanged),
            this, &SliderControl::onASliderChanged);
    connect(ui->B_slider, QOverload<int>::of(&QSlider::valueChanged),
            this, &SliderControl::onBSliderChanged);
    connect(ui->C_slider, QOverload<int>::of(&QSlider::valueChanged),
            this, &SliderControl::onCSliderChanged);
    connect(ui->G_slider, QOverload<int>::of(&QSlider::valueChanged),
            this, &SliderControl::onGSliderChanged);

    // 数字框值变化连接
    connect(ui->A_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SliderControl::onASpinBoxChanged);
    connect(ui->B_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SliderControl::onBSpinBoxChanged);
    connect(ui->C_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SliderControl::onCSpinBoxChanged);
    connect(ui->G_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SliderControl::onGSpinBoxChanged);
}

void SliderControl::on_bacl_clicked()
{
    // 返回按钮
    close();
}

void SliderControl::on_sendall_clicked()
{
    // 发送所有角度
    int aAngle = ui->A_slider->value();
    int bAngle = ui->B_slider->value();
    int cAngle = ui->C_slider->value();
    int gAngle = ui->G_slider->value();

    QString command = QString("setall %1 %2 %3 %4").arg(aAngle).arg(bAngle).arg(cAngle).arg(gAngle);
    sendCommand(command);

    appendLog(QString("发送角度: A=%1°, B=%2°, C=%3°, G=%4°").arg(aAngle).arg(bAngle).arg(cAngle).arg(gAngle));
}

void SliderControl::on_reset_clicked()
{
    // 重置按钮
    sendCommand("reset");

    // 重置滑块到默认位置
    m_updatingSliders = true;
    ui->A_slider->setValue(90);
    ui->B_slider->setValue(0);
    ui->C_slider->setValue(180);
    ui->G_slider->setValue(0);
    m_updatingSliders = false;

    appendLog("发送重置命令");
}

void SliderControl::on_statue_clicked()
{
    // 获取状态
    sendCommand("status");
    appendLog("请求机械臂状态");
}

void SliderControl::on_limits_clicked()
{
    // 显示限制
    sendCommand("limits");
    appendLog("请求角度限制信息");
}

void SliderControl::on_clearlog_clicked()
{
    // 清除日志
    ui->textEdit->clear();
    appendLog("日志已清除");
}

void SliderControl::onASliderChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->A_spinBox->setValue(value);
        m_updatingSliders = false;
    }
}

void SliderControl::onBSliderChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->B_spinBox->setValue(value);
        updateCSliderRange();  // B轴变化时更新C轴范围
        m_updatingSliders = false;
    }
}

void SliderControl::onCSliderChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->C_spinBox->setValue(value);
        m_updatingSliders = false;
    }
}

void SliderControl::onGSliderChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->G_spinBox->setValue(value);
        m_updatingSliders = false;
    }
}

void SliderControl::onASpinBoxChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->A_slider->setValue(value);
        m_updatingSliders = false;
    }
}

void SliderControl::onBSpinBoxChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->B_slider->setValue(value);
        updateCSliderRange();  // B轴变化时更新C轴范围
        m_updatingSliders = false;
    }
}

void SliderControl::onCSpinBoxChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->C_slider->setValue(value);
        m_updatingSliders = false;
    }
}

void SliderControl::onGSpinBoxChanged(int value)
{
    if (!m_updatingSliders) {
        m_updatingSliders = true;
        ui->G_slider->setValue(value);
        m_updatingSliders = false;
    }
}

void SliderControl::updateCSliderRange()
{
    // 根据ESP32代码中的公式更新C轴范围
    int bAngle = ui->B_slider->value();
    int minCAngle = 140 - bAngle;
    int maxCAngle = 180;

    if (196 - bAngle < 180) {
        maxCAngle = 196 - bAngle;
    }

    // 确保范围合理
    if (minCAngle < 0) minCAngle = 0;
    if (maxCAngle > 180) maxCAngle = 180;
    if (minCAngle > maxCAngle) minCAngle = maxCAngle;

    // 更新滑块和数字框范围
    ui->C_slider->setMinimum(minCAngle);
    ui->C_slider->setMaximum(maxCAngle);
    ui->C_spinBox->setMinimum(minCAngle);
    ui->C_spinBox->setMaximum(maxCAngle);

    // 更新范围标签
    ui->C_range->setText(QString("range: %1° - %2°").arg(minCAngle).arg(maxCAngle));

    // 如果当前值超出范围，调整到范围内
    int currentValue = ui->C_slider->value();
    if (currentValue < minCAngle) {
        ui->C_slider->setValue(minCAngle);
    } else if (currentValue > maxCAngle) {
        ui->C_slider->setValue(maxCAngle);
    }
}

void SliderControl::onDataReceived(const QString &data)
{
    appendLog(QString("收到数据: %1").arg(data));

    // 解析状态数据
    if (data.contains(",") && data.split(",").size() == 4) {
        updateSlidersFromStatus(data);
    }
}

void SliderControl::onBluetoothLogMessage(const QString &message)
{
    // 这里可以选择性地显示蓝牙日志，避免界面太乱
    // appendLog(message);
}

void SliderControl::updateSlidersFromStatus(const QString &statusData)
{
    // 解析状态数据并更新滑块 (格式: "90,45,150,20")
    QStringList angles = statusData.split(",");
    if (angles.size() == 4) {
        bool ok1, ok2, ok3, ok4;
        int aAngle = angles[0].toInt(&ok1);
        int bAngle = angles[1].toInt(&ok2);
        int cAngle = angles[2].toInt(&ok3);
        int gAngle = angles[3].toInt(&ok4);

        if (ok1 && ok2 && ok3 && ok4) {
            m_updatingSliders = true;

            ui->A_slider->setValue(aAngle);
            ui->B_slider->setValue(bAngle);
            ui->C_slider->setValue(cAngle);
            ui->G_slider->setValue(gAngle);

            updateCSliderRange();  // 更新C轴范围

            m_updatingSliders = false;

            appendLog(QString("状态更新: A=%1°, B=%2°, C=%3°, G=%4°").arg(aAngle).arg(bAngle).arg(cAngle).arg(gAngle));
        }
    }
}

void SliderControl::sendCommand(const QString &command)
{
    if (m_bluetooth && m_bluetooth->isConnected()) {
        m_bluetooth->sendCommand(command);
    } else {
        appendLog("错误: 未连接到设备");
        QMessageBox::warning(this, "警告", "未连接到机械臂设备");
    }
}

void SliderControl::appendLog(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    ui->textEdit->append(logEntry);

    // 自动滚动到底部
    ui->textEdit->moveCursor(QTextCursor::End);
}
