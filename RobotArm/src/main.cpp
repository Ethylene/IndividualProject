#include <Arduino.h>
#include "movement/movement.h"
#include "bluetooth/bluetooth.h"
#include "command_break/command_break.h"
//#include "anglefeedback/anglefeedback.h"



void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  Serial.println("启动机械臂控制系统...");
  
  // 初始化机械臂
  initRobotArm();

  // 初始化电位器
  // initanglefeedback();

  initBluetooth();
}

void loop() {
  // 检查是否有串口输入命令
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    executeCommand(command, false);  // 来自串口
  }
  // 检查蓝牙命令
  String btCommand = getBluetooth();
  if (btCommand.length() > 0) {
    Serial.print("蓝牙收到命令: ");
    Serial.println(btCommand);
    executeCommand(btCommand, true);  // 来自蓝牙
  }

  
  delay(10); // 短暂延时避免CPU占用过高
}

