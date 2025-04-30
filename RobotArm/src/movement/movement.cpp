#include "movement.h"
#include <Arduino.h>

// 全局变量
JointSystem jointSystem;
Ticker moveTimer;

// 舵机移动控制函数
void moveJointHelper(int id, int targetAngle, int currentAngle) {
  if(targetAngle > currentAngle) {
    currentAngle++;
    jointSystem.servos[id].write(currentAngle);
  } else if(targetAngle < currentAngle) {
    currentAngle--;
    jointSystem.servos[id].write(currentAngle);
  }
  jointSystem.currentAngles[id] = currentAngle;
}

// 定时器回调函数
void moveTimerCallback() {
  for(int i = 0; i < JOINT_COUNT; i++) {
    moveJointHelper(i, jointSystem.targetAngles[i], jointSystem.currentAngles[i]);
  }
}

// 检查角度是否在允许范围内
bool checkAngleValid(int jointId, int angle) {
  // 基座舵机a (0-180度)
  if(jointId == 0) {
    return (angle >= 0 && angle <= 180);
  }
  // 肩部舵机b (0-77度)
  else if(jointId == 1) {
    return (angle >= 0 && angle <= 77);
  }
  // 肘部舵机c (公式范围)
  else if(jointId == 2) {
    int b_Angle = jointSystem.currentAngles[1];
    int minc_Angle = 140 - b_Angle;
    int maxc_Angle = 180;
    if(196 - b_Angle < 180) {
      maxc_Angle = 196 - b_Angle;
    }
    return (angle >= minc_Angle && angle <= maxc_Angle);
  }
  // 抓手舵机g (0-37.5度)
  else if(jointId == 3) {
    return (angle >= 0 && angle <= 37);
  }
  
  return false;
}

// 初始化机械臂
void initRobotArm() {
  // 连接舵机到引脚
  jointSystem.servos[0].attach(SERVO_PIN_A, 500, 2500);
  jointSystem.servos[1].attach(SERVO_PIN_B, 500, 2500);
  jointSystem.servos[2].attach(SERVO_PIN_C, 500, 2500);
  jointSystem.servos[3].attach(SERVO_PIN_G, 500, 2500);
  
  // 设置每个舵机的原点位置
  jointSystem.targetAngles[0] = DEFAULT_POS_A;      // 舵机a: 90度
  jointSystem.currentAngles[0] = DEFAULT_POS_A;
  jointSystem.servos[0].write(DEFAULT_POS_A);
  
  jointSystem.targetAngles[1] = DEFAULT_POS_B;  // 舵机b: 0度
  jointSystem.currentAngles[1] = DEFAULT_POS_B;
  jointSystem.servos[1].write(DEFAULT_POS_B);
  
  jointSystem.targetAngles[2] = DEFAULT_POS_C;     // 舵机c: 180度
  jointSystem.currentAngles[2] = DEFAULT_POS_C;
  jointSystem.servos[2].write(DEFAULT_POS_C);
  
  jointSystem.targetAngles[3] = DEFAULT_POS_G;   // 舵机g: 0度
  jointSystem.currentAngles[3] = DEFAULT_POS_G;
  jointSystem.servos[3].write(DEFAULT_POS_G);
  
  // 启动定时器，15ms每次移动
  moveTimer.attach_ms(15, moveTimerCallback);
  
  Serial.println("机械臂初始化完成，所有舵机已设置到原点位置");
}

// 设置单个关节角度
bool setJointPosition(int jointId, int angle) {
  if(jointId < 0 || jointId >= JOINT_COUNT) {
    return false;
  }
  
  if(!checkAngleValid(jointId, angle)) {
    Serial.print("关节");
    Serial.print(jointId);
    Serial.print("角度");
    Serial.print(angle);
    Serial.println("超出范围");
    return false;
  }
  
  jointSystem.targetAngles[jointId] = angle;
  return true;
}

// 设置所有关节角度
bool setAllJointPositions(int a_Angle, int b_Angle, int c_Angle, int g_Angle) {
  // 首先检查基座和肩部角度
  if(!checkAngleValid(0, a_Angle) || !checkAngleValid(1, b_Angle)) {
    return false;
  }
  
  // 临时更新肩部角度，以便正确计算肘部范围
  int oldb_Angle = jointSystem.currentAngles[1];
  jointSystem.currentAngles[1] = b_Angle;
  
  // 检查肘部和抓手角度
  bool c_Valid = checkAngleValid(2, c_Angle);
  bool g_Valid = checkAngleValid(3, g_Angle);
  
  // 恢复肩部角度
  jointSystem.currentAngles[1] = oldb_Angle;
  
  if(!c_Angle || !g_Valid) {
    return false;
  }
  
  // 都有效，设置目标角度
  jointSystem.targetAngles[0] = a_Angle;
  jointSystem.targetAngles[1] = b_Angle;
  jointSystem.targetAngles[2] = c_Angle;
  jointSystem.targetAngles[3] = g_Angle;
  
  return true;
}

// 设置多个关节角度（数组方式）
bool setJointPositions(int angles[]) {
  return setAllJointPositions(angles[0], angles[1], angles[2], angles[3]);
}

// 获取当前关节位置
void getCurrentPositions(int positions[]) {
  for(int i = 0; i < JOINT_COUNT; i++) {
    positions[i] = jointSystem.currentAngles[i];
  }
}

// 重置到各舵机的原点位置
void resetToDefaultPosition() {
  jointSystem.targetAngles[0] = DEFAULT_POS_A;      // 舵机a: 90度
  jointSystem.targetAngles[1] = DEFAULT_POS_B;  // 舵机b: 0度
  jointSystem.targetAngles[2] = DEFAULT_POS_C;     // 舵机c: 180度
  jointSystem.targetAngles[3] = DEFAULT_POS_G;   // 舵机g: 0度
  
  Serial.println("重置所有舵机到原点位置");
}

// 调试函数：打印关节角度限制和原点位置
void printJointLimits() {
  Serial.println("---------------------");
  Serial.println("关节角度限制和原点位置:");
  Serial.println("---------------------");
  Serial.print("基座(舵机a): 角度范围: 0-180度, 原点位置: ");
  Serial.print(DEFAULT_POS_A);
  Serial.println("度");
  
  Serial.print("肩部(舵机b): 角度范围: 0-77度, 原点位置: ");
  Serial.print(DEFAULT_POS_B);
  Serial.println("度");
  
  int b_Angle = jointSystem.currentAngles[1];
  int minc_Angle = 140 - b_Angle;
  int maxc_Angle = 180;
  if(196 - b_Angle < 180) {
    maxc_Angle = 196 - b_Angle;
  }
  
  Serial.print("肘部(舵机c): 角度范围: ");
  Serial.print(minc_Angle);
  Serial.print("-");
  Serial.print(maxc_Angle);
  Serial.print("度 (基于当前肩部角度), 原点位置: ");
  Serial.print(DEFAULT_POS_C);
  Serial.println("度");
  
  Serial.print("抓手(舵机g): 角度范围: 0-37度, 原点位置: ");
  Serial.print(DEFAULT_POS_G);
  Serial.println("度");
  Serial.println("---------------------");
  
  // 打印当前位置与原点的关系
  Serial.println("当前各舵机位置:");
  Serial.print("基座: 当前:");
  Serial.print(jointSystem.currentAngles[0]);
  Serial.print("度, 与原点相差: ");
  Serial.print(jointSystem.currentAngles[0] - DEFAULT_POS_A);
  Serial.println("度");
  
  Serial.print("肩部: 当前:");
  Serial.print(jointSystem.currentAngles[1]);
  Serial.print("度, 与原点相差: ");
  Serial.print(jointSystem.currentAngles[1] - DEFAULT_POS_B);
  Serial.println("度");
  
  Serial.print("肘部: 当前:");
  Serial.print(jointSystem.currentAngles[2]);
  Serial.print("度, 与原点相差: ");
  Serial.print(jointSystem.currentAngles[2] - DEFAULT_POS_C);
  Serial.println("度");
  
  Serial.print("抓手: 当前:");
  Serial.print(jointSystem.currentAngles[3]);
  Serial.print("度, 与原点相差: ");
  Serial.print(jointSystem.currentAngles[3] - DEFAULT_POS_G);
  Serial.println("度");
}