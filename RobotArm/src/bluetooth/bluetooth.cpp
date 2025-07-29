#include "bluetooth.h"

// 全局变量
bool bluetoothConnected = false;
String receivedCommand = "";
bool hasNewCommand = false;

/******测试加入：毫秒级精度******/
// 测试相关全局变量定义
unsigned long testStartTime = 0;  // Android发送时间戳（ms）
bool isTestCommand = false;       // 是否为测试命令
String actualCommand = "";        // 实际要执行的命令
/*******测试加入：毫秒级精度******/

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic = NULL;
BLECharacteristic* pRxCharacteristic = NULL;

// 服务器回调类
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        bluetoothConnected = true;
        Serial.println("蓝牙客户端已连接");
    };

    void onDisconnect(BLEServer* pServer) {
        bluetoothConnected = false;
        Serial.println("蓝牙客户端已断开连接");
        // 重新开始广播
        BLEDevice::startAdvertising();
    }
};

// 特征值回调类
/*class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            // 将 std::string 转换为 Arduino String
            receivedCommand = String(rxValue.c_str());
            
            // 移除换行符
            receivedCommand.trim();
            hasNewCommand = true;
            
            Serial.print("蓝牙接收到命令: ");
            Serial.println(receivedCommand);
        }
    }
};*/
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        unsigned long esp32ReceiveTime = millis(); // 立即记录ESP32接收时间
        
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            String receivedData = String(rxValue.c_str());
            receivedData.trim();
            
            // 检查是否为时间测试命令
            if (receivedData.startsWith("TIMING_TEST:")) {
                // 解析: "TIMING_TEST:12345,set 0 90"
                int colonIndex = receivedData.indexOf(':');
                int commaIndex = receivedData.indexOf(',');
                
                if (colonIndex > 0 && commaIndex > colonIndex) {
                    // 提取Android发送时间（相对时间）
                    String androidTimeStr = receivedData.substring(colonIndex + 1, commaIndex);
                    unsigned long androidSendTime = androidTimeStr.toInt();
                    
                    // 提取实际命令
                    actualCommand = receivedData.substring(commaIndex + 1);
                    receivedCommand = actualCommand;
                    isTestCommand = true;
                    
                    // 保存时间信息用于后续计算
                    testStartTime = esp32ReceiveTime;
                    
                    Serial.printf("=== 完整响应时间测试 ===\n");
                    Serial.printf("Android发送时间: %lu ms\n", androidSendTime);
                    Serial.printf("ESP32接收时间: %lu ms\n", esp32ReceiveTime);
                    
                    // 注意：这里暂时不计算BLE延迟，因为时间基准不同
                    // 我们主要关注ESP32内部的处理时间
                    
                    Serial.printf("测试命令: %s\n", actualCommand.c_str());
                } else {
                    receivedCommand = receivedData;
                    isTestCommand = false;
                }
            } else {
                receivedCommand = receivedData;
                isTestCommand = false;
            }
            
            hasNewCommand = true;
            
            Serial.print("蓝牙接收到命令: ");
            Serial.println(receivedCommand);
        }
    }
};

// 初始化蓝牙
void initBluetooth() {
    Serial.println("初始化蓝牙BLE服务...");
    
    // 创建BLE设备
    BLEDevice::init(BT_DEVICE_NAME);
    
    // 创建BLE服务器
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // 创建BLE服务
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // 创建TX特征值(服务器发送数据)
    pTxCharacteristic = pService->createCharacteristic(
                        CHAR_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                    );
    pTxCharacteristic->addDescriptor(new BLE2902());
    
    // 创建RX特征值(服务器接收数据)
    pRxCharacteristic = pService->createCharacteristic(
                        CHAR_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                    );
    pRxCharacteristic->setCallbacks(new MyCallbacks());
    
    // 启动服务
    pService->start();
    
    // 启动广播
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    
    Serial.println("蓝牙BLE服务已启动，等待客户端连接...");
    Serial.print("设备名称: ");
    Serial.println(BT_DEVICE_NAME);
}

// 发送蓝牙消息
void sendBluetooth(String message) {
    if (bluetoothConnected && pTxCharacteristic != NULL) {
        pTxCharacteristic->setValue(message.c_str());
        pTxCharacteristic->notify();
        Serial.print("蓝牙发送: ");
        Serial.println(message);
    }
}

// 获取蓝牙接收的命令
String getBluetooth() {
    if (hasNewCommand) {
        hasNewCommand = false;
        return receivedCommand;
    }
    return "";
}

// 检查蓝牙连接状态
bool isBluetoothConnected() {
    return bluetoothConnected;
}