#ifndef DIRECTIONCONTROL_H
#define DIRECTIONCONTROL_H

#include <QWidget>
#include <QTimer>
#include "bluetooth.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DirectionControl; }
QT_END_NAMESPACE

class DirectionControl : public QWidget
{
    Q_OBJECT

public:
    explicit DirectionControl(Bluetooth *bluetooth, QWidget *parent = nullptr);
    ~DirectionControl();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // UI事件处理
    void on_bacl_clicked();         // 返回按钮
    void on_reset_clicked();        // 重置按钮
    void on_statue_clicked();       // 获取状态
    void on_limits_clicked();       // 显示限制
    void on_clearlog_clicked();     // 清除日志

    // 机械臂方向控制
    void on_up_pressed();           // 上按钮按下
    void on_up_released();          // 上按钮释放
    void on_down_pressed();         // 下按钮按下
    void on_down_released();        // 下按钮释放
    void on_left_pressed();         // 左按钮按下
    void on_left_released();        // 左按钮释放
    void on_right_pressed();        // 右按钮按下
    void on_right_released();       // 右按钮释放
    void on_stop_clicked();         // 停止按钮

    // 抓手控制
    void on_open_clicked();         // 打开抓手
    void on_grip_clicked();         // 抓取

    // 定时器处理
    void onMoveTimer();

    // 蓝牙数据处理
    void onDataReceived(const QString &data);
    void onBluetoothLogMessage(const QString &message);

private:
    Ui::DirectionControl *ui;
    Bluetooth *m_bluetooth;

    // 移动控制
    QTimer *m_moveTimer;
    enum MoveDirection {
        MOVE_STOP = 0,
        MOVE_UP,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT
    };
    MoveDirection m_currentDirection;

    // 当前位置状态
    struct RobotPosition {
        int aAngle;  // 基座
        int bAngle;  // 肩部
        int cAngle;  // 肘部
        int gAngle;  // 抓手
    };
    RobotPosition m_currentPosition;

    // 移动参数
    static const int MOVE_STEP = 1;      // 每步移动角度
    static const int MOVE_INTERVAL = 50; // 移动间隔(ms)

    // 辅助方法
    void initializePosition();
    void startMoving(MoveDirection direction);
    void stopMoving();
    void appendLog(const QString &message);
    void sendCommand(const QString &command);
    void updatePositionFromStatus(const QString &statusData);
    void moveInDirection(MoveDirection direction);
    bool isAngleValid(int joint, int angle);
    int calculateCAngleRange(int bAngle, bool getMin);
};

#endif // DIRECTIONCONTROL_H
