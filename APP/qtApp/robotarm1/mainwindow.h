#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QSpinBox;
class QPushButton;
class QComboBox;
class QTextEdit;
QT_END_NAMESPACE

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
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_refreshButton_clicked();
    void on_resetButton_clicked();
    void on_statusButton_clicked();
    void on_limitsButton_clicked();
    void on_sendAllButton_clicked();
    void on_clearLogButton_clicked();

    void on_baseSlider_valueChanged(int value);
    void on_shoulderSlider_valueChanged(int value);
    void on_elbowSlider_valueChanged(int value);
    void on_gripperSlider_valueChanged(int value);

    void on_baseSpinBox_valueChanged(int value);
    void on_shoulderSpinBox_valueChanged(int value);
    void on_elbowSpinBox_valueChanged(int value);
    void on_gripperSpinBox_valueChanged(int value);

    void readSerialData();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QTimer *readTimer;
    bool isConnected;

    void setupSerialConnection();
    void sendCommand(const QString &command);
    void logMessage(const QString &message);
    void updateElbowLimits();
    void refreshSerialPorts();
    void resetToDefaults();
    void syncSliderAndSpinBox(int jointId, int value, bool fromSlider);
};

#endif // MAINWINDOW_H
