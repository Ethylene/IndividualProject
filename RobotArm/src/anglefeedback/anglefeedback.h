#ifndef ANGLEFEEDBACK_H
#define ANGLEFEEDBACK_H

#include <Arduino.h>

// 电位器引脚定义
#define POT_PIN_B 34    // B轴电位器引脚
#define POT_PIN_C 35    // C轴电位器引脚

// 电位器校准参数（根据实际情况调整这些值）
#define POT_B_MIN 100     // B轴电位器最小ADC值
#define POT_B_MAX 3900    // B轴电位器最大ADC值
#define POT_C_MIN 100     // C轴电位器最小ADC值  
#define POT_C_MAX 3900    // C轴电位器最大ADC值

// 函数声明
void initanglefeedback();           // 初始化电位器
float getAngleB();                   // 获取B轴实际角度
float getAngleC();                   // 获取C轴实际角度
void printAngles();                  // 打印当前角度

#endif // ANGLEFEEDBACK_H