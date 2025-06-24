package com.example.miniarmcontroller;

import android.Manifest;
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
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import androidx.core.app.ActivityCompat;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class BluetoothManager {
    private static final String TAG = "BluetoothManager";

    // 单例实例
    private static BluetoothManager instance;

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
    private Handler handler = new Handler(Looper.getMainLooper());
    private Runnable scanStopRunnable;

    public interface BluetoothManagerListener {
        void onDeviceFound(BluetoothDeviceInfo device);
        void onScanFinished();
        void onConnectionStateChanged(boolean connected, String deviceAddress);
        void onDataReceived(String data);
        void onError(String error);
    }

    // 单例模式构造函数 - 私有
    private BluetoothManager(Context context) {
        this.context = context.getApplicationContext(); // 使用ApplicationContext避免内存泄漏
        this.discoveredDevices = new ArrayList<>();
        this.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (bluetoothAdapter != null && bluetoothAdapter.isEnabled()) {
            this.bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
        }
    }

    // 获取单例实例
    public static synchronized BluetoothManager getInstance(Context context) {
        if (instance == null) {
            instance = new BluetoothManager(context);
        }
        // 更新context以防Activity切换
        instance.context = context.getApplicationContext();
        return instance;
    }

    public void setListener(BluetoothManagerListener listener) {
        this.listener = listener;
        // 如果使用单例，同时更新单例的listener
        if (instance != null) {
            instance.listener = listener;
        }
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

    // 检查权限
    private boolean hasRequiredPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            return ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED &&
                    ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED;
        } else {
            return ActivityCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
        }
    }

    // 开始扫描BLE设备
    public void startScan() {
        if (!isBluetoothEnabled()) {
            if (listener != null) {
                listener.onError("Bluetooth not enabled");
            }
            return;
        }

        if (!hasRequiredPermissions()) {
            if (listener != null) {
                listener.onError("Missing required permissions");
            }
            return;
        }

        if (isScanning) {
            Log.d(TAG, "Already scanning");
            return;
        }

        // 获取 BluetoothLeScanner
        if (bluetoothLeScanner == null) {
            bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
            if (bluetoothLeScanner == null) {
                Log.e(TAG, "Cannot get BluetoothLeScanner");
                if (listener != null) {
                    listener.onError("Cannot initialize Bluetooth scanner");
                }
                return;
            }
        }

        discoveredDevices.clear();
        isScanning = true;

        try {
            Log.d(TAG, "Starting BLE device scan");
            bluetoothLeScanner.startScan(scanCallback);

            // 10秒后自动停止扫描
            scanStopRunnable = new Runnable() {
                @Override
                public void run() {
                    stopScan();
                }
            };
            handler.postDelayed(scanStopRunnable, 10000);
        } catch (Exception e) {
            Log.e(TAG, "Failed to start scan: " + e.getMessage());
            isScanning = false;
            if (listener != null) {
                listener.onError("Failed to start scan: " + e.getMessage());
            }
        }
    }

    // 停止扫描
    public void stopScan() {
        if (!isScanning) {
            return;
        }

        isScanning = false;

        // 取消定时停止
        if (scanStopRunnable != null) {
            handler.removeCallbacks(scanStopRunnable);
            scanStopRunnable = null;
        }

        if (bluetoothLeScanner != null) {
            try {
                bluetoothLeScanner.stopScan(scanCallback);
            } catch (Exception e) {
                Log.e(TAG, "Failed to stop scan: " + e.getMessage());
            }
        }

        Log.d(TAG, "Stopped scan, found " + discoveredDevices.size() + " devices");
        if (listener != null) {
            listener.onScanFinished();
        }
    }

    // 扫描回调
    private ScanCallback scanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            try {
                BluetoothDevice device = result.getDevice();
                int rssi = result.getRssi();

                // 获取设备名称（需要权限检查）
                String deviceName = null;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                        deviceName = device.getName();
                    }
                } else {
                    deviceName = device.getName();
                }

                // 如果无法获取名称，使用地址作为名称
                if (deviceName == null || deviceName.isEmpty()) {
                    deviceName = "Unknown Device";
                }

                BluetoothDeviceInfo deviceInfo = new BluetoothDeviceInfo(device, rssi);

                // 避免重复添加同一设备
                boolean exists = false;
                for (BluetoothDeviceInfo existingDevice : discoveredDevices) {
                    if (existingDevice.getAddress().equals(deviceInfo.getAddress())) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    discoveredDevices.add(deviceInfo);
                    Log.d(TAG, "Found device: " + deviceName + " (" + device.getAddress() + ")");

                    if (listener != null) {
                        listener.onDeviceFound(deviceInfo);
                    }
                }
            } catch (Exception e) {
                Log.e(TAG, "Error processing scan result: " + e.getMessage());
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            Log.e(TAG, "Scan failed, error code: " + errorCode);
            isScanning = false;
            if (listener != null) {
                String errorMsg = "Scan failed: ";
                switch (errorCode) {
                    case SCAN_FAILED_ALREADY_STARTED:
                        errorMsg += "Scan already started";
                        break;
                    case SCAN_FAILED_APPLICATION_REGISTRATION_FAILED:
                        errorMsg += "Application registration failed";
                        break;
                    case SCAN_FAILED_INTERNAL_ERROR:
                        errorMsg += "Internal error";
                        break;
                    case SCAN_FAILED_FEATURE_UNSUPPORTED:
                        errorMsg += "Feature not supported";
                        break;
                    default:
                        errorMsg += "Unknown error " + errorCode;
                }
                listener.onError(errorMsg);
            }
        }
    };

    // 连接到指定设备
    public void connectToDevice(BluetoothDevice device) {
        if (!hasRequiredPermissions()) {
            if (listener != null) {
                listener.onError("Connection permission missing");
            }
            return;
        }

        if (bluetoothGatt != null) {
            bluetoothGatt.close();
            bluetoothGatt = null;
        }

        try {
            Log.d(TAG, "Connecting to device: " + device.getAddress());
            bluetoothGatt = device.connectGatt(context, false, gattCallback);
        } catch (Exception e) {
            Log.e(TAG, "Connection failed: " + e.getMessage());
            if (listener != null) {
                listener.onError("Connection failed: " + e.getMessage());
            }
        }
    }

    // GATT回调
    private BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d(TAG, "Connected to GATT server");
                isConnected = true;

                handler.post(() -> {
                    if (listener != null) {
                        listener.onConnectionStateChanged(true, gatt.getDevice().getAddress());
                    }
                });

                // 发现服务
                try {
                    gatt.discoverServices();
                } catch (Exception e) {
                    Log.e(TAG, "Service discovery failed: " + e.getMessage());
                }

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.d(TAG, "Disconnected from GATT server");
                isConnected = false;

                handler.post(() -> {
                    if (listener != null) {
                        listener.onConnectionStateChanged(false, gatt.getDevice().getAddress());
                    }
                });
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d(TAG, "Service discovery successful");

                BluetoothGattService service = gatt.getService(UUID.fromString(SERVICE_UUID));
                if (service != null) {
                    rxCharacteristic = service.getCharacteristic(UUID.fromString(CHAR_UUID_RX));
                    txCharacteristic = service.getCharacteristic(UUID.fromString(CHAR_UUID_TX));

                    if (rxCharacteristic != null && txCharacteristic != null) {
                        Log.d(TAG, "Found required characteristics");

                        // 启用通知
                        try {
                            gatt.setCharacteristicNotification(txCharacteristic, true);
                        } catch (Exception e) {
                            Log.e(TAG, "Failed to set notification: " + e.getMessage());
                        }

                    } else {
                        Log.e(TAG, "Required characteristics not found");
                        handler.post(() -> {
                            if (listener != null) {
                                listener.onError("Device not compatible");
                            }
                        });
                    }
                } else {
                    Log.e(TAG, "Required service not found");
                    handler.post(() -> {
                        if (listener != null) {
                            listener.onError("Device not compatible");
                        }
                    });
                }
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            if (characteristic.getUuid().equals(UUID.fromString(CHAR_UUID_TX))) {
                byte[] data = characteristic.getValue();
                String receivedData = new String(data);
                Log.d(TAG, "Received data: " + receivedData);

                handler.post(() -> {
                    if (listener != null) {
                        listener.onDataReceived(receivedData);
                    }
                });
            }
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                Log.d(TAG, "Write successful");
            } else {
                Log.e(TAG, "Write failed, status: " + status);
            }
        }
    };

    // 发送数据
    public boolean sendData(String data) {
        if (!isConnected || bluetoothGatt == null || rxCharacteristic == null) {
            Log.e(TAG, "Cannot send data, not connected. isConnected=" + isConnected + ", bluetoothGatt=" + bluetoothGatt + ", rxCharacteristic=" + rxCharacteristic);
            return false;
        }

        if (!hasRequiredPermissions()) {
            Log.e(TAG, "Cannot send data, missing permissions");
            return false;
        }

        try {
            rxCharacteristic.setValue(data.getBytes());
            boolean result = bluetoothGatt.writeCharacteristic(rxCharacteristic);
            Log.d(TAG, "Send data: " + data + ", result: " + result);
            return result;
        } catch (Exception e) {
            Log.e(TAG, "Send data exception: " + e.getMessage());
            return false;
        }
    }

    // 断开连接
    public void disconnect() {
        if (bluetoothGatt != null) {
            try {
                bluetoothGatt.disconnect();
                bluetoothGatt.close();
            } catch (Exception e) {
                Log.e(TAG, "Disconnect failed: " + e.getMessage());
            }
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
        if (handler != null) {
            handler.removeCallbacksAndMessages(null);
        }
    }
}