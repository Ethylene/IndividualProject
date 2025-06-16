package com.example.miniarmcontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class BluetoothManager {
    private static final String TAG = "BluetoothManager";

    // ESP32 BLE服务和特征值UUID (与ESP32代码中的保持一致)
    private static final String SERVICE_UUID = "b9c7eaa3-f8de-4f40-9ad3-4e7e49bd431a";
    private static final String CHAR_UUID_RX = "d05182d7-e26f-4bfc-8a69-6e3efb177358";
    private static final String CHAR_UUID_TX = "a0c4ca34-247c-4651-86a9-bf61b28b7bff";

    private Context context;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattCharacteristic rxCharacteristic;
    private BluetoothGattCharacteristic txCharacteristic;

    private boolean isScanning = false;
    private boolean isConnected = false;

    private BluetoothManagerListener listener;
    private List<BluetoothDeviceInfo> discoveredDevices;

    public interface BluetoothManagerListener {
        void onDeviceFound(BluetoothDeviceInfo device);
        void onScanFinished();
        void onConnectionStateChanged(boolean connected, String deviceAddress);
        void onDataReceived(String data);
        void onError(String error);
    }

    public BluetoothManager(Context context) {
        this.context = context;
        this.discoveredDevices = new ArrayList<>();
        this.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (bluetoothAdapter != null) {
            this.bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
        }
    }

    public void setListener(BluetoothManagerListener listener) {
        this.listener = listener;
    }

    public boolean isBluetoothEnabled() {
        return bluetoothAdapter != null && bluetoothAdapter.isEnabled();
    }

    public boolean isConnected() {
        return isConnected;
    }

    public boolean isScanning() {
        return isScanning;
    }

    public List<BluetoothDeviceInfo> getDiscoveredDevices() {
        return new ArrayList<>(discoveredDevices);
    }

    // 开始扫描BLE设备
    public void startScan() {
        if (!isBluetoothEnabled()) {
            if (listener != null) {
                listener.onError("蓝牙未启用");
            }
            return;
        }

        if (isScanning) {
            return;
        }

        discoveredDevices.clear();
        isScanning = true;

        Log.d(TAG, "开始扫描BLE设备");
        bluetoothLeScanner.startScan(scanCallback);

        // 10秒后自动停止扫描
        new android.os.Handler().postDelayed(() -> {
            stopScan();
        }, 10000);
    }

    // 停止扫描
    public void stopScan() {
        if (!isScanning) {
            return;
        }

        isScanning = false;
        if (bluetoothLeScanner != null) {
            bluetoothLeScanner.stopScan(scanCallback);
        }

        Log.d(TAG, "停止扫描，发现 " + discoveredDevices.size() + " 个设备");
        if (listener != null) {
            listener.onScanFinished();
        }
    }

    // 扫描回调
    private ScanCallback scanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            int rssi = result.getRssi();

            // 过滤掉没有名称的设备，优先显示RobotArm设备
            if (device.getName() != null) {
                BluetoothDeviceInfo deviceInfo = new BluetoothDeviceInfo(device, rssi);

                // 避免重复添加同一设备
                if (!discoveredDevices.contains(deviceInfo)) {
                    discoveredDevices.add(deviceInfo);
                    Log.d(TAG, "发现设备: " + device.getName() + " (" + device.getAddress() + ")");

                    if (listener != null) {
                        listener.onDeviceFound(deviceInfo);
                    }
                }
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            Log.e(TAG, "扫描失败，错误代码: " + errorCode);
            isScanning = false;
            if (listener != null) {
                listener.onError("扫描失败: " + errorCode);
            }
        }
    };

    // 连接到指定设备
    public void connectToDevice(BluetoothDevice device) {
        if (bluetoothGatt != null) {
            bluetoothGatt.close();
            bluetoothGatt = null;
        }

        Log.d(TAG, "正在连接到设备: " + device.getAddress());
        bluetoothGatt = device.connectGatt(context, false, gattCallback);
    }

    // GATT回调
    private BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d(TAG, "已连接到GATT服务器");
                isConnected = true;

                if (listener != null) {
                    listener.onConnectionStateChanged(true, gatt.getDevice().getAddress());
                }

                // 发现服务
                gatt.discoverServices();

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.d(TAG, "已断开GATT服务器连接");
                isConnected = false;

                if (listener != null) {
                    listener.onConnectionStateChanged(false, gatt.getDevice().getAddress());
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d(TAG, "服务发现成功");

                BluetoothGattService service = gatt.getService(UUID.fromString(SERVICE_UUID));
                if (service != null) {
                    rxCharacteristic = service.getCharacteristic(UUID.fromString(CHAR_UUID_RX));
                    txCharacteristic = service.getCharacteristic(UUID.fromString(CHAR_UUID_TX));

                    if (rxCharacteristic != null && txCharacteristic != null) {
                        Log.d(TAG, "找到所需的特征值");

                        // 启用通知
                        gatt.setCharacteristicNotification(txCharacteristic, true);

                    } else {
                        Log.e(TAG, "未找到所需的特征值");
                        if (listener != null) {
                            listener.onError("设备不兼容");
                        }
                    }
                } else {
                    Log.e(TAG, "未找到所需的服务");
                    if (listener != null) {
                        listener.onError("设备不兼容");
                    }
                }
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            if (characteristic.getUuid().equals(UUID.fromString(CHAR_UUID_TX))) {
                byte[] data = characteristic.getValue();
                String receivedData = new String(data);
                Log.d(TAG, "收到数据: " + receivedData);

                if (listener != null) {
                    listener.onDataReceived(receivedData);
                }
            }
        }
    };

    // 发送数据
    public boolean sendData(String data) {
        if (!isConnected || bluetoothGatt == null || rxCharacteristic == null) {
            Log.e(TAG, "无法发送数据，未连接");
            return false;
        }

        try {
            rxCharacteristic.setValue(data.getBytes());
            boolean result = bluetoothGatt.writeCharacteristic(rxCharacteristic);
            Log.d(TAG, "发送数据: " + data + ", 结果: " + result);
            return result;
        } catch (Exception e) {
            Log.e(TAG, "发送数据异常: " + e.getMessage());
            return false;
        }
    }

    // 断开连接
    public void disconnect() {
        if (bluetoothGatt != null) {
            bluetoothGatt.disconnect();
            bluetoothGatt.close();
            bluetoothGatt = null;
        }
        isConnected = false;
        rxCharacteristic = null;
        txCharacteristic = null;
    }

    // 清理资源
    public void cleanup() {
        stopScan();
        disconnect();
    }
}