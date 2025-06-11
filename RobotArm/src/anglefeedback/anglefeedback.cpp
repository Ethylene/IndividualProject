#include "anglefeedback.h"
#include "../movement/movement.h"

// 初始化电位器
void initanglefeedback() {
    analogReadResolution(12);  // 设置ADC分辨率为12位
    Serial.println("电位器初始化完成");
}

// 获取B轴实际角度
float getAngleB() {
    int adcValue = analogRead(POT_PIN_B);
    
    // 限制在有效范围内
    if(adcValue < POT_B_MIN) adcValue = POT_B_MIN;
    if(adcValue > POT_B_MAX) adcValue = POT_B_MAX;
    
    // 将ADC值映射到0-77度范围（B轴舵机角度范围）
    float servoAngleB = (float)(adcValue - POT_B_MIN) * 77.0 / (POT_B_MAX - POT_B_MIN);
    
    // B轴角度（∠B）与B轴舵机角度（∠Bs）关系：∠B = ∠Bs
    return servoAngleB;
}

// 获取C轴实际角度
float getAngleC() {
    int adcValue = analogRead(POT_PIN_C);
    
    // 限制在有效范围内
    if(adcValue < POT_C_MIN) adcValue = POT_C_MIN;
    if(adcValue > POT_C_MAX) adcValue = POT_C_MAX;
    
    // 将ADC值映射到140-180度范围（C轴舵机角度范围）
    float servoAngleC = 140.0 + (float)(adcValue - POT_C_MIN) * 40.0 / (POT_C_MAX - POT_C_MIN);
    
    // C轴角度（∠C）与C轴舵机（∠Cs）角度的关系：∠C = ∠Bs + ∠Cs - 96.5°
    float angleB = getAngleB();
    return angleB + servoAngleC - 96.5;
}

// 打印当前角度
void printAngles() {
    float angleB = getAngleB();
    float angleC = getAngleC();
    
    Serial.print("B轴角度: ");
    Serial.print(angleB, 1);
    Serial.print("°, C轴角度: ");
    Serial.print(angleC, 1);
    Serial.println("°");
}