#ifndef SLIDERCONTROL_H
#define SLIDERCONTROL_H

#include <QWidget>
#include <QTimer>
#include "bluetooth.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SliderControl; }
QT_END_NAMESPACE

class SliderControl : public QWidget
{
    Q_OBJECT

public:
    explicit SliderControl(Bluetooth *bluetooth, QWidget *parent = nullptr);
    ~SliderControl();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // UI事件处理
    void on_bacl_clicked();         // 返回按钮
    void on_sendall_clicked();      // 发送所有角度
    void on_reset_clicked();        // 重置按钮
    void on_statue_clicked();       // 获取状态
    void on_limits_clicked();       // 显示限制
    void on_clearlog_clicked();     // 清除日志

    // 滑块和数字框联动
    void onASliderChanged(int value);
    void onBSliderChanged(int value);
    void onCSliderChanged(int value);
    void onGSliderChanged(int value);

    void onASpinBoxChanged(int value);
    void onBSpinBoxChanged(int value);
    void onCSpinBoxChanged(int value);
    void onGSpinBoxChanged(int value);

    // 蓝牙数据处理
    void onDataReceived(const QString &data);
    void onBluetoothLogMessage(const QString &message);

private:
    Ui::SliderControl *ui;
    Bluetooth *m_bluetooth;

    // 状态管理
    bool m_updatingSliders;  // 防止递归更新

    // 辅助方法
    void setupConnections();
    void updateCSliderRange();  // 根据B轴角度更新C轴范围
    void appendLog(const QString &message);
    void sendCommand(const QString &command);
    void updateSlidersFromStatus(const QString &statusData);
};

#endif // SLIDERCONTROL_H
