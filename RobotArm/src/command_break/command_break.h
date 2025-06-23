#ifndef COMMAND_BREAK_H
#define COMMAND_BREAK_H

#include <Arduino.h>

// 命令类型枚举（新增力矩补偿命令）
typedef enum {
  CMD_UNKNOWN = 0,
  CMD_SET_JOINT,        // set 0 90
  CMD_SET_ALL_JOINTS,   // setall 90 45 60 20
  CMD_RESET,           // reset
  CMD_STATUS,          // status
  CMD_LIMITS,          // limits/info
  CMD_HELP,            // help
  CMD_TORQUE_STATUS,   // torque - 查看力矩补偿状态
  CMD_TORQUE_ENABLE,   // torque on/off - 启用/禁用力矩补偿
  CMD_TORQUE_SET       // torque set <threshold> <compensation> <delay>
} CommandType;

// 解析后的命令结构体
typedef struct {
  CommandType type;
  int jointId;           // 关节ID (仅用于单关节命令)
  int angles[4];         // 角度数组 (用于多关节命令，也用于存储torque参数)
  int angleCount;        // 有效角度数量
  String originalCommand; // 原始命令字符串
  bool isValid;          // 命令是否有效
  String errorMessage;   // 错误信息
} ParsedCommand;

// 函数声明
void executeCommand(String command, bool fromBluetooth = false);
ParsedCommand parseCommand(String rawCommand);
void executeParsedCommand(ParsedCommand cmd, bool fromBluetooth = false);
void sendResponse(String response, bool toBluetooth = false);

#endif // COMMAND_BREAK_H