#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <ESP32Servo.h>
#include <Ticker.h>

// 舵机数量和引脚定义
#define JOINT_COUNT 4

// 各舵机的原点位置
#define DEFAULT_POS_A 90     // 舵机a原点为90度
#define DEFAULT_POS_B 0  // 舵机b原点为0度
#define DEFAULT_POS_C 180   // 舵机c原点为180度
#define DEFAULT_POS_G 0   // 舵机g原点为0度

// 舵机引脚
#define SERVO_PIN_A 33
#define SERVO_PIN_B 25 
#define SERVO_PIN_C 26
#define SERVO_PIN_G 27

// 机械臂关节系统结构体
typedef struct {
  Servo servos[JOINT_COUNT];       // 舵机对象
  int currentAngles[JOINT_COUNT];  // 当前角度
  int targetAngles[JOINT_COUNT];   // 目标角度
} JointSystem;

// 外部变量声明
extern JointSystem jointSystem;

// 函数声明
void initRobotArm();  // 初始化机械臂
bool setJointPosition(int jointId, int angle);  // 设置单个关节位置
bool setAllJointPositions(int a_Angle, int b_Angle, int c_Angle, int g_Angle);  // 设置所有关节位置
bool setJointPositions(int angles[]);  // 数组方式设置关节位置
void getCurrentPositions(int positions[]);  // 获取当前位置
void resetToDefaultPosition();  // 重置到默认位置
void printJointLimits();  // 调试用：打印关节限制

#endif // MOVEMENT_H