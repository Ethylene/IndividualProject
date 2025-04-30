#include <Arduino.h>
#include "movement/movement.h"

// 函数声明
void processCommand(String command);

void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  Serial.println("启动机械臂控制系统...");
  
  // 初始化机械臂
  initRobotArm();
}

void loop() {
  // 检查是否有串口输入命令
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }
  
  // 这里可以添加其他控制逻辑
  delay(10); // 短暂延时避免CPU占用过高
}

// 处理命令函数
void processCommand(String command) {
  // 把命令转换为小写，方便处理
  command.trim();
  
  // 设置角度命令: set 0 90 (设置关节0到90度)
  if (command.startsWith("set ")) {
    command = command.substring(4); // 去掉"set "
    int spaceIndex = command.indexOf(' ');
    
    if (spaceIndex != -1) {
      int jointId = command.substring(0, spaceIndex).toInt();
      int angle = command.substring(spaceIndex + 1).toInt();
      
      if (setJointPosition(jointId, angle)) {
        Serial.print("设置关节");
        Serial.print(jointId);
        Serial.print("到");
        Serial.print(angle);
        Serial.println("度");
      } else {
        Serial.println("设置失败，角度可能超出限制");
      }
    }
  } 
  // 设置所有关节: setall 90 45 60 20
  else if (command.startsWith("setall ")) {
    command = command.substring(7); // 去掉"setall "
    
    // 分割命令获取四个角度值
    int values[4];
    int lastIndex = 0;
    int valueIndex = 0;
    
    for (int i = 0; i < command.length(); i++) {
      if (command.charAt(i) == ' ' || i == command.length() - 1) {
        String numStr;
        if (i == command.length() - 1) {
          numStr = command.substring(lastIndex);
        } else {
          numStr = command.substring(lastIndex, i);
        }
        
        if (valueIndex < 4) {
          values[valueIndex] = numStr.toInt();
          valueIndex++;
        }
        
        lastIndex = i + 1;
      }
    }
    
    if (valueIndex == 4) {
      if (setAllJointPositions(values[0], values[1], values[2], values[3])) {
        Serial.println("成功设置所有关节角度");
      } else {
        Serial.println("设置失败，某些角度超出限制");
      }
    } else {
      Serial.println("命令格式错误，需要4个角度值");
    }
  }
  // 重置命令
  else if (command == "reset") {
    resetToDefaultPosition();
    Serial.println("重置所有关节到默认位置");
  }
  // 状态命令
  else if (command == "status") {
    int positions[JOINT_COUNT];
    getCurrentPositions(positions);
    
    Serial.println("当前关节角度:");
    for (int i = 0; i < JOINT_COUNT; i++) {
      Serial.print("关节");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(positions[i]);
      Serial.println("度");
    }
  }
  // 打印角度限制与原点信息
  else if (command == "limits" || command == "info") {
    printJointLimits();
  }
  // 帮助命令
  else if (command == "help") {
    Serial.println("可用命令:");
    Serial.println("set <关节ID> <角度> - 设置单个关节角度");
    Serial.println("  关节ID: 0=基座(舵机a), 1=肩部(舵机b), 2=肘部(舵机c), 3=抓手(舵机g)");
    Serial.println("setall <基座> <肩部> <肘部> <抓手> - 设置所有关节角度");
    Serial.println("reset - 重置到各舵机原点位置");
    Serial.println("status - 显示当前角度");
    Serial.println("limits 或 info - 显示角度限制和原点信息");
    Serial.println("help - 显示帮助");
  }
  else {
    Serial.println("未知命令，输入'help'获取帮助");
  }
}