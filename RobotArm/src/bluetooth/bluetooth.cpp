#include "bluetooth.h"

// 全局变量
bool bluetoothConnected = false;
String receivedCommand = "";
bool hasNewCommand = false;

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
class MyCallbacks: public BLECharacteristicCallbacks {
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