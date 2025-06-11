#ifndef ROBOTARMCONTROLLER_H
#define ROBOTARMCONTROLLER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QStatusBar>
#include <QMainWindow>

class RobotArmController : public QMainWindow
{
    Q_OBJECT

public:
    explicit RobotArmController(QWidget *parent = nullptr);
    ~RobotArmController();

private slots:
    void onSliderValueChanged();
    void onSpinBoxValueChanged();
    void connectToSerial();
    void disconnectFromSerial();
    void sendResetCommand();
    void sendStatusCommand();
    void sendLimitsCommand();
    void refreshSerialPorts();
    void readSerialData();
    void sendAllJointsCommand();
    void updateShoulderLimits();

private:
    void setupUI();
    void setupSerialConnection();
    void sendCommand(const QString &command);
    void updateSliderRange(int jointId);
    void logMessage(const QString &message);

    // UI组件
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;

    // 关节控制组
    QGroupBox *jointsGroup;
    QGridLayout *jointsLayout;

    // 滑杆和数值框
    QSlider *sliders[4];
    QSpinBox *spinBoxes[4];
    QLabel *jointLabels[4];
    QLabel *rangeLabels[4];

    // 连接控制组
    QGroupBox *connectionGroup;
    QHBoxLayout *connectionLayout;
    QComboBox *serialPortCombo;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *refreshButton;

    // 命令控制组
    QGroupBox *commandGroup;
    QHBoxLayout *commandLayout;
    QPushButton *resetButton;
    QPushButton *statusButton;
    QPushButton *limitsButton;
    QPushButton *sendAllButton;

    // 日志显示
    QGroupBox *logGroup;
    QVBoxLayout *logLayout;
    QTextEdit *logTextEdit;
    QPushButton *clearLogButton;

    // 串口通信
    QSerialPort *serialPort;
    QTimer *readTimer;

    // 关节限制信息
    struct JointLimits {
        int min;
        int max;
        int defaultPos;
        QString name;
        QString description;
    };

    JointLimits jointLimits[4];

    // 状态信息
    bool isConnected;
    QString lastCommand;
};

#endif // ROBOTARMCONTROLLER_H
