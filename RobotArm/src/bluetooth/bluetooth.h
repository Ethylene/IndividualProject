#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// 蓝牙设备名称和UUID
#define BT_DEVICE_NAME "RobotArm"
#define SERVICE_UUID "b9c7eaa3-f8de-4f40-9ad3-4e7e49bd431a"
#define CHAR_UUID_RX "d05182d7-e26f-4bfc-8a69-6e3efb177358"  
#define CHAR_UUID_TX "a0c4ca34-247c-4651-86a9-bf61b28b7bff"

// 全局变量
extern bool bluetoothConnected;
extern String receivedCommand;
extern bool hasNewCommand;

/******测试加入******/
extern bool isTestCommand;
extern unsigned long androidSendTime;
extern unsigned long esp32ReceiveTime;
extern String testCommand;;
/******测试加入******/

// 简单函数
void initBluetooth();
void sendBluetooth(String message);
String getBluetooth();
bool isBluetoothConnected();





#endif