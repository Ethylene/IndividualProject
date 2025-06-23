#include "movement.h"
#include <Arduino.h>

// 全局变量
JointSystem jointSystem;
Ticker moveTimer;

// 新增：力矩补偿参数
struct TorqueCompensation {
  bool enabled = true;                    // 启用力矩补偿
  int bAxisThreshold = 30;               // B轴重载阈值（角度大于30°时启用）
  int compensationAngle = 3;             // 预超调角度
  int compensationDelay = 150;           // 补偿延时（ms）
  int slowMoveThreshold = 40;            // 慢速移动阈值
  bool isCompensating = false;           // 当前是否在补偿中
  unsigned long compensationStartTime = 0; // 补偿开始时间
  int originalTarget = 0;                // 原始目标角度
} torqueComp;

// 新增：移动速度控制
struct MoveSpeed {
  int normalInterval = 15;    // 正常移动间隔（ms）
  int slowInterval = 25;      // 慢速移动间隔（ms）
  int fastInterval = 10;      // 快速移动间隔（ms）
  bool useSlowMode = false;   // 当前是否使用慢速模式
} moveSpeed;

// 新增：检查是否需要力矩补偿
bool needsTorqueCompensation(int jointId, int targetAngle) {
  if (!torqueComp.enabled || jointId != 1) {  // 只对B轴舵机进行补偿
    return false;
  }
  
  int currentAngle = jointSystem.currentAngles[1];
  
  // 判断条件：
  // 1. 向上移动（targetAngle < currentAngle）
  // 2. 目标角度在重载区域（>= threshold）
  // 3. 移动幅度较大（>= 5度）
  return (targetAngle < currentAngle) && 
         (targetAngle >= torqueComp.bAxisThreshold) && 
         (abs(targetAngle - currentAngle) >= 5);
}

// 新增：启动力矩补偿
void startTorqueCompensation(int originalTarget) {
  torqueComp.isCompensating = true;
  torqueComp.compensationStartTime = millis();
  torqueComp.originalTarget = originalTarget;
  
  // 设置预超调角度
  int compensationTarget = originalTarget - torqueComp.compensationAngle;
  if (compensationTarget < 0) compensationTarget = 0;
  
  jointSystem.targetAngles[1] = compensationTarget;
  
  Serial.printf("🚀 B轴力矩补偿启动: %d° -> %d° -> %d°\n", 
                jointSystem.currentAngles[1], compensationTarget, originalTarget);
}

// 新增：检查补偿完成
void checkCompensationComplete() {
  if (!torqueComp.isCompensating) return;
  
  unsigned long elapsed = millis() - torqueComp.compensationStartTime;
  
  // 如果补偿时间已到，切换到最终目标
  if (elapsed >= torqueComp.compensationDelay) {
    jointSystem.targetAngles[1] = torqueComp.originalTarget;
    torqueComp.isCompensating = false;
    
    Serial.printf("⚡ B轴补偿完成，切换到目标角度: %d°\n", torqueComp.originalTarget);
  }
}

// 增强的舵机移动控制函数
void moveJointHelper(int id, int targetAngle, int currentAngle) {
  // 特殊处理B轴移动
  if (id == 1) {
    // 检查是否在重载区域，使用慢速移动
    if (currentAngle >= torqueComp.slowMoveThreshold || targetAngle >= torqueComp.slowMoveThreshold) {
      moveSpeed.useSlowMode = true;
    } else {
      moveSpeed.useSlowMode = false;
    }
  }
  
  // 标准移动逻辑
  if (targetAngle > currentAngle) {
    currentAngle++;
    jointSystem.servos[id].write(currentAngle);
  } else if (targetAngle < currentAngle) {
    currentAngle--;
    jointSystem.servos[id].write(currentAngle);
  }
  
  jointSystem.currentAngles[id] = currentAngle;
  
  // B轴特殊处理：到达补偿角度时的调试信息
  if (id == 1 && torqueComp.isCompensating && currentAngle == jointSystem.targetAngles[1]) {
    Serial.printf("📍 B轴到达补偿位置: %d°\n", currentAngle);
  }
}

// 增强的定时器回调函数
void moveTimerCallback() {
  // 检查力矩补偿状态
  checkCompensationComplete();
  
  // 执行关节移动
  for (int i = 0; i < JOINT_COUNT; i++) {
    moveJointHelper(i, jointSystem.targetAngles[i], jointSystem.currentAngles[i]);
  }
}

// 动态调整定时器间隔
void updateMoveTimer() {
  moveTimer.detach();
  
  int interval = moveSpeed.normalInterval;
  if (moveSpeed.useSlowMode) {
    interval = moveSpeed.slowInterval;
  }
  
  moveTimer.attach_ms(interval, moveTimerCallback);
}

// 检查角度是否在允许范围内（保持原有逻辑）
bool checkAngleValid(int jointId, int angle) {
  // 基座舵机a (0-180度)
  if (jointId == 0) {
    return (angle >= 0 && angle <= 180);
  }
  // 肩部舵机b (0-77度)
  else if (jointId == 1) {
    return (angle >= 0 && angle <= 77);
  }
  // 肘部舵机c (公式范围)
  else if (jointId == 2) {
    int b_Angle = jointSystem.currentAngles[1];
    int minc_Angle = 140 - b_Angle;
    int maxc_Angle = 180;
    if (196 - b_Angle < 180) {
      maxc_Angle = 196 - b_Angle;
    }
    return (angle >= minc_Angle && angle <= maxc_Angle);
  }
  // 抓手舵机g (0-37.5度)
  else if (jointId == 3) {
    return (angle >= 0 && angle <= 37);
  }
  
  return false;
}

// 初始化机械臂（保持原有逻辑，添加补偿初始化）
void initRobotArm() {
  // 连接舵机到引脚（为B轴使用更强的PWM设置）
  jointSystem.servos[0].attach(SERVO_PIN_A, 500, 2500);
  jointSystem.servos[1].attach(SERVO_PIN_B, 500, 2500);  // B轴可以考虑调整PWM参数
  jointSystem.servos[2].attach(SERVO_PIN_C, 500, 2500);
  jointSystem.servos[3].attach(SERVO_PIN_G, 500, 2500);
  
  // 设置每个舵机的原点位置
  jointSystem.targetAngles[0] = DEFAULT_POS_A;
  jointSystem.currentAngles[0] = DEFAULT_POS_A;
  jointSystem.servos[0].write(DEFAULT_POS_A);
  
  jointSystem.targetAngles[1] = DEFAULT_POS_B;
  jointSystem.currentAngles[1] = DEFAULT_POS_B;
  jointSystem.servos[1].write(DEFAULT_POS_B);
  
  jointSystem.targetAngles[2] = DEFAULT_POS_C;
  jointSystem.currentAngles[2] = DEFAULT_POS_C;
  jointSystem.servos[2].write(DEFAULT_POS_C);
  
  jointSystem.targetAngles[3] = DEFAULT_POS_G;
  jointSystem.currentAngles[3] = DEFAULT_POS_G;
  jointSystem.servos[3].write(DEFAULT_POS_G);
  
  // 启动定时器
  moveTimer.attach_ms(moveSpeed.normalInterval, moveTimerCallback);
  
  Serial.println("机械臂初始化完成，力矩补偿系统已启用");
  Serial.printf("力矩补偿参数: 阈值=%d°, 超调=%d°, 延时=%dms\n", 
                torqueComp.bAxisThreshold, torqueComp.compensationAngle, torqueComp.compensationDelay);
}

// 增强的设置单个关节角度
bool setJointPosition(int jointId, int angle) {
  if (jointId < 0 || jointId >= JOINT_COUNT) {
    return false;
  }
  
  if (!checkAngleValid(jointId, angle)) {
    Serial.printf("关节%d角度%d°超出范围\n", jointId, angle);
    return false;
  }
  
  // B轴特殊处理：检查是否需要力矩补偿
  if (needsTorqueCompensation(jointId, angle)) {
    startTorqueCompensation(angle);
    Serial.printf("🔧 B轴智能控制: %d° -> %d° (启用力矩补偿)\n", 
                  jointSystem.currentAngles[1], angle);
  } else {
    jointSystem.targetAngles[jointId] = angle;
    
    if (jointId == 1) {
      // B轴常规移动的调试信息
      if (angle > jointSystem.currentAngles[1]) {
        Serial.printf("📉 B轴下降: %d° -> %d° (重力协助)\n", 
                      jointSystem.currentAngles[1], angle);
      } else {
        Serial.printf("📈 B轴上升: %d° -> %d° (常规模式)\n", 
                      jointSystem.currentAngles[1], angle);
      }
    }
  }
  
  // 更新移动速度
  updateMoveTimer();
  
  return true;
}

// 增强的设置所有关节角度
bool setAllJointPositions(int a_Angle, int b_Angle, int c_Angle, int g_Angle) {
  // 首先检查基座和肩部角度
  if (!checkAngleValid(0, a_Angle) || !checkAngleValid(1, b_Angle)) {
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
  
  if (!c_Valid || !g_Valid) {
    return false;
  }
  
  // 设置非B轴关节
  jointSystem.targetAngles[0] = a_Angle;
  jointSystem.targetAngles[2] = c_Angle;
  jointSystem.targetAngles[3] = g_Angle;
  
  // B轴特殊处理
  if (needsTorqueCompensation(1, b_Angle)) {
    startTorqueCompensation(b_Angle);
    Serial.printf("🔧 setall命令 - B轴启用力矩补偿: %d°\n", b_Angle);
  } else {
    jointSystem.targetAngles[1] = b_Angle;
  }
  
  updateMoveTimer();
  
  return true;
}

// 新增：调整力矩补偿参数的函数
void setTorqueCompensationParams(int threshold, int compensation, int delay) {
  torqueComp.bAxisThreshold = threshold;
  torqueComp.compensationAngle = compensation;
  torqueComp.compensationDelay = delay;
  
  Serial.printf("力矩补偿参数已更新: 阈值=%d°, 超调=%d°, 延时=%dms\n", 
                threshold, compensation, delay);
}

// 新增：启用/禁用力矩补偿
void enableTorqueCompensation(bool enable) {
  torqueComp.enabled = enable;
  Serial.printf("力矩补偿已%s\n", enable ? "启用" : "禁用");
}

// 新增：获取力矩补偿状态
void printTorqueCompensationStatus() {
  Serial.println("===== 力矩补偿状态 =====");
  Serial.printf("启用状态: %s\n", torqueComp.enabled ? "是" : "否");
  Serial.printf("补偿阈值: %d°\n", torqueComp.bAxisThreshold);
  Serial.printf("超调角度: %d°\n", torqueComp.compensationAngle);
  Serial.printf("补偿延时: %dms\n", torqueComp.compensationDelay);
  Serial.printf("当前补偿中: %s\n", torqueComp.isCompensating ? "是" : "否");
  Serial.printf("慢速模式: %s\n", moveSpeed.useSlowMode ? "是" : "否");
  Serial.println("========================");
}

// 保持原有的其他函数
bool setJointPositions(int angles[]) {
  return setAllJointPositions(angles[0], angles[1], angles[2], angles[3]);
}

void getCurrentPositions(int positions[]) {
  for (int i = 0; i < JOINT_COUNT; i++) {
    positions[i] = jointSystem.currentAngles[i];
  }
}

void resetToDefaultPosition() {
  jointSystem.targetAngles[0] = DEFAULT_POS_A;
  jointSystem.targetAngles[1] = DEFAULT_POS_B;
  jointSystem.targetAngles[2] = DEFAULT_POS_C;
  jointSystem.targetAngles[3] = DEFAULT_POS_G;
  
  Serial.println("重置所有舵机到原点位置");
}

void printJointLimits() {
  Serial.println("---------------------");
  Serial.println("关节角度限制和原点位置:");
  Serial.println("---------------------");
  Serial.printf("基座(舵机a): 角度范围: 0-180度, 原点位置: %d度\n", DEFAULT_POS_A);
  Serial.printf("肩部(舵机b): 角度范围: 0-77度, 原点位置: %d度\n", DEFAULT_POS_B);
  
  int b_Angle = jointSystem.currentAngles[1];
  int minc_Angle = 140 - b_Angle;
  int maxc_Angle = 180;
  if (196 - b_Angle < 180) {
    maxc_Angle = 196 - b_Angle;
  }
  
  Serial.printf("肘部(舵机c): 角度范围: %d-%d度 (基于当前肩部角度), 原点位置: %d度\n", 
                minc_Angle, maxc_Angle, DEFAULT_POS_C);
  Serial.printf("抓手(舵机g): 角度范围: 0-37度, 原点位置: %d度\n", DEFAULT_POS_G);
  Serial.println("---------------------");
  
  // 打印当前位置
  Serial.println("当前各舵机位置:");
  for (int i = 0; i < JOINT_COUNT; i++) {
    int defaults[] = {DEFAULT_POS_A, DEFAULT_POS_B, DEFAULT_POS_C, DEFAULT_POS_G};
    const char* names[] = {"基座", "肩部", "肘部", "抓手"};
    
    Serial.printf("%s: 当前:%d度, 与原点相差: %d度\n", 
                  names[i], jointSystem.currentAngles[i], 
                  jointSystem.currentAngles[i] - defaults[i]);
  }
}