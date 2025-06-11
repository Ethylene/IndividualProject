#include "command_break.h"
#include "../movement/movement.h"
#include "../bluetooth/bluetooth.h"


// 主要的命令执行函数
void executeCommand(String command, bool fromBluetooth) {
  // 解析命令
  ParsedCommand parsedCmd = parseCommand(command);
  
  // 执行解析后的命令
  executeParsedCommand(parsedCmd, fromBluetooth);
}

// 解析原始命令字符串
ParsedCommand parseCommand(String rawCommand) {
  ParsedCommand result;
  
  // 初始化结果结构体
  result.type = CMD_UNKNOWN;
  result.jointId = -1;
  result.angleCount = 0;
  result.originalCommand = rawCommand;
  result.isValid = false;
  result.errorMessage = "";
  
  // 清空角度数组
  for(int i = 0; i < 4; i++) {
    result.angles[i] = 0;
  }
  
  // 预处理：去掉首尾空格
  rawCommand.trim();
  
  if(rawCommand.length() == 0) {
    result.errorMessage = "空命令";
    return result;
  }
  
  // 分析命令类型和参数
  if(rawCommand.startsWith("set ")) {
    result.type = CMD_SET_JOINT;
    
    // 解析 "set 0 90" 格式
    String params = rawCommand.substring(4);
    params.trim();
    
    int spaceIndex = params.indexOf(' ');
    if(spaceIndex == -1) {
      result.errorMessage = "set命令格式错误，应为: set <关节ID> <角度>";
      return result;
    }
    
    result.jointId = params.substring(0, spaceIndex).toInt();
    result.angles[0] = params.substring(spaceIndex + 1).toInt();
    result.angleCount = 1;
    
    // 验证关节ID范围
    if(result.jointId < 0 || result.jointId >= 4) {
      result.errorMessage = "关节ID必须在0-3之间";
      return result;
    }
    
    result.isValid = true;
  }
  else if(rawCommand.startsWith("setall ")) {
    result.type = CMD_SET_ALL_JOINTS;
    
    // 解析 "setall 90 45 60 20" 格式
    String params = rawCommand.substring(7);
    params.trim();
    
    // 分割参数
    result.angleCount = 0;
    int lastIndex = 0;
    
    for(int i = 0; i <= params.length(); i++) {
      if(params.charAt(i) == ' ' || i == params.length()) {
        if(i > lastIndex) {
          String angleStr = params.substring(lastIndex, i);
          angleStr.trim();
          
          if(angleStr.length() > 0 && result.angleCount < 4) {
            result.angles[result.angleCount] = angleStr.toInt();
            result.angleCount++;
          }
        }
        lastIndex = i + 1;
      }
    }
    
    // 验证角度数量
    if(result.angleCount != 4) {
      result.errorMessage = "setall命令需要4个角度值";
      return result;
    }
    
    result.isValid = true;
  }
  else if(rawCommand == "reset") {
    result.type = CMD_RESET;
    result.isValid = true;
  }
  else if(rawCommand == "status") {
    result.type = CMD_STATUS;
    result.isValid = true;
  }
  else if(rawCommand == "limits" || rawCommand == "info") {
    result.type = CMD_LIMITS;
    result.isValid = true;
  }
  else if(rawCommand == "help") {
    result.type = CMD_HELP;
    result.isValid = true;
  }
  else {
    result.errorMessage = "未知命令: " + rawCommand;
  }
  
  return result;
}

// 执行解析后的命令
void executeParsedCommand(ParsedCommand cmd, bool fromBluetooth) {
  // 检查命令是否有效
  if(!cmd.isValid) {
    sendResponse("ERROR: " + cmd.errorMessage, fromBluetooth);
    return;
  }
  
  // 根据命令类型执行相应操作
  switch(cmd.type) {
    case CMD_SET_JOINT: {
      if(setJointPosition(cmd.jointId, cmd.angles[0])) {
        String response = "设置关节" + String(cmd.jointId) + "到" + String(cmd.angles[0]) + "度";
        sendResponse(response, fromBluetooth);
        
        // 蓝牙发送简单确认
        if(fromBluetooth) {
          sendBluetooth("OK");
        }
      } else {
        sendResponse("设置失败，角度可能超出限制", fromBluetooth);
        
        if(fromBluetooth) {
          sendBluetooth("ERROR");
        }
      }
      break;
    }
    
    case CMD_SET_ALL_JOINTS: {
      if(setAllJointPositions(cmd.angles[0], cmd.angles[1], cmd.angles[2], cmd.angles[3])) {
        sendResponse("成功设置所有关节角度", fromBluetooth);
        
        if(fromBluetooth) {
          sendBluetooth("OK");
        }
      } else {
        sendResponse("设置失败，某些角度超出限制", fromBluetooth);
        
        if(fromBluetooth) {
          sendBluetooth("ERROR");
        }
      }
      break;
    }
    
    case CMD_RESET: {
      resetToDefaultPosition();
      sendResponse("重置所有关节到默认位置", fromBluetooth);
      
      if(fromBluetooth) {
        sendBluetooth("RESET_OK");
      }
      break;
    }
    
    case CMD_STATUS: {
      int positions[JOINT_COUNT];
      getCurrentPositions(positions);
      
      if(fromBluetooth) {
        // 蓝牙发送简单格式
        String status = String(positions[0]) + "," + String(positions[1]) + "," + 
                       String(positions[2]) + "," + String(positions[3]);
        sendBluetooth(status);
      } else {
        // 串口发送详细格式
        Serial.println("当前关节角度:");
        for(int i = 0; i < JOINT_COUNT; i++) {
          Serial.print("关节");
          Serial.print(i);
          Serial.print(": ");
          Serial.print(positions[i]);
          Serial.println("度");
        }
      }
      break;
    }
    
    case CMD_LIMITS: {
      if(fromBluetooth) {
        sendBluetooth("LIMITS: 基座0-180°,肩部0-77°,肘部动态,抓手0-37°");
      } else {
        printJointLimits();
      }
      break;
    }
    
    case CMD_HELP: {
      if(fromBluetooth) {
        sendBluetooth("HELP: set/setall/reset/status/limits/help");
      } else {
        Serial.println("可用命令:");
        Serial.println("set <关节ID> <角度> - 设置单个关节角度");
        Serial.println("  关节ID: 0=基座(舵机a), 1=肩部(舵机b), 2=肘部(舵机c), 3=抓手(舵机g)");
        Serial.println("setall <基座> <肩部> <肘部> <抓手> - 设置所有关节角度");
        Serial.println("reset - 重置到各舵机原点位置");
        Serial.println("status - 显示当前角度");
        Serial.println("limits 或 info - 显示角度限制和原点信息");
        Serial.println("help - 显示帮助");
      }
      break;
    }
    
    default: {
      sendResponse("系统错误，无法处理命令", fromBluetooth);
      break;
    }
  }
}

// 统一的响应发送函数
void sendResponse(String response, bool toBluetooth) {
  Serial.println(response);
  
  if(toBluetooth && isBluetoothConnected()) {
    // 这里不用sendBluetooth，避免重复发送
    // sendBluetooth(response);
  }
}