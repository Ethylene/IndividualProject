package com.example.miniarmcontroller;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements BluetoothManager.BluetoothManagerListener {
    private static final String TAG = "MainActivity";
    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_PERMISSIONS = 2;

    private BluetoothManager bluetoothManager;
    private DeviceListAdapter deviceAdapter;
    private List<BluetoothDeviceInfo> deviceList;

    // UI组件
    private TextView tvBluetoothStatus;
    private TextView tvEmptyList;
    private Button btnScan;
    private ListView lvDevices;

    // 连接状态
    private boolean isConnecting = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            Log.d(TAG, "=== MainActivity onCreate started ===");
            setContentView(R.layout.activity_main);
            Log.d(TAG, "setContentView completed");

            initViews();
            Log.d(TAG, "initViews completed");

            initBluetooth();
            Log.d(TAG, "initBluetooth completed");

            checkPermissions();
            Log.d(TAG, "checkPermissions completed");
            Log.d(TAG, "=== MainActivity onCreate finished successfully ===");

        } catch (Exception e) {
            Log.e(TAG, "=== CRITICAL ERROR in onCreate ===", e);
            Toast.makeText(this, "Application initialization failed: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void initViews() {
        try {
            Log.d(TAG, "Starting initViews");

            tvBluetoothStatus = findViewById(R.id.tv_bluetooth_status);
            if (tvBluetoothStatus == null) {
                Log.e(TAG, "ERROR: tv_bluetooth_status not found in layout");
                return;
            }
            Log.d(TAG, "tv_bluetooth_status found");

            tvEmptyList = findViewById(R.id.tv_empty_list);
            if (tvEmptyList == null) {
                Log.e(TAG, "ERROR: tv_empty_list not found in layout");
                return;
            }
            Log.d(TAG, "tv_empty_list found");

            btnScan = findViewById(R.id.btn_scan);
            if (btnScan == null) {
                Log.e(TAG, "ERROR: btn_scan not found in layout");
                return;
            }
            Log.d(TAG, "btn_scan found");

            lvDevices = findViewById(R.id.lv_devices);
            if (lvDevices == null) {
                Log.e(TAG, "ERROR: lv_devices not found in layout");
                return;
            }
            Log.d(TAG, "lv_devices found");

            // 设置按钮点击事件
            btnScan.setOnClickListener(v -> {
                Log.d(TAG, "Scan button clicked");
                startScan();
            });

            // 设置设备列表点击事件 - 点击直接连接
            deviceList = new ArrayList<>();
            deviceAdapter = new DeviceListAdapter(this, deviceList);
            lvDevices.setAdapter(deviceAdapter);

            lvDevices.setOnItemClickListener((parent, view, position, id) -> {
                if (position < deviceList.size() && !isConnecting) {
                    BluetoothDeviceInfo deviceInfo = deviceList.get(position);
                    Log.d(TAG, "User clicked device: " + deviceInfo.getName());
                    connectToDevice(deviceInfo);
                }
            });

            updateBluetoothStatus();
            updateEmptyListVisibility();

            Log.d(TAG, "initViews completed successfully");

        } catch (Exception e) {
            Log.e(TAG, "CRITICAL ERROR in initViews", e);
            Toast.makeText(this, "Interface initialization failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void initBluetooth() {
        try {
            Log.d(TAG, "Starting initBluetooth");
            // 明确使用getInstance方法
            bluetoothManager = BluetoothManager.getInstance(this);
            bluetoothManager.setListener(this);
            Log.d(TAG, "initBluetooth completed successfully");
        } catch (Exception e) {
            Log.e(TAG, "ERROR in initBluetooth", e);
            Toast.makeText(this, "Bluetooth initialization failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void checkPermissions() {
        try {
            Log.d(TAG, "Starting checkPermissions");
            List<String> permissionsToRequest = new ArrayList<>();

            // Android 12+ 需要新的蓝牙权限
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN)
                        != PackageManager.PERMISSION_GRANTED) {
                    permissionsToRequest.add(Manifest.permission.BLUETOOTH_SCAN);
                }
                if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
                        != PackageManager.PERMISSION_GRANTED) {
                    permissionsToRequest.add(Manifest.permission.BLUETOOTH_CONNECT);
                }
            } else {
                // Android 11 及以下版本
                if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH)
                        != PackageManager.PERMISSION_GRANTED) {
                    permissionsToRequest.add(Manifest.permission.BLUETOOTH);
                }
                if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADMIN)
                        != PackageManager.PERMISSION_GRANTED) {
                    permissionsToRequest.add(Manifest.permission.BLUETOOTH_ADMIN);
                }
            }

            // 位置权限（所有版本都需要）
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                    != PackageManager.PERMISSION_GRANTED) {
                permissionsToRequest.add(Manifest.permission.ACCESS_FINE_LOCATION);
            }
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                    != PackageManager.PERMISSION_GRANTED) {
                permissionsToRequest.add(Manifest.permission.ACCESS_COARSE_LOCATION);
            }

            if (!permissionsToRequest.isEmpty()) {
                Log.d(TAG, "Requesting permissions: " + permissionsToRequest);
                ActivityCompat.requestPermissions(this,
                        permissionsToRequest.toArray(new String[0]), REQUEST_PERMISSIONS);
            } else {
                Log.d(TAG, "All permissions granted");
                checkBluetoothEnabled();
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in checkPermissions", e);
        }
    }

    private void checkBluetoothEnabled() {
        try {
            if (bluetoothManager != null && !bluetoothManager.isBluetoothEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            } else {
                Log.d(TAG, "Bluetooth enabled");
                updateBluetoothStatus();
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in checkBluetoothEnabled", e);
        }
    }

    private void updateBluetoothStatus() {
        try {
            runOnUiThread(() -> {
                if (bluetoothManager != null && bluetoothManager.isBluetoothEnabled()) {
                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("Bluetooth enabled, click scan to find devices");
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                    }
                    if (btnScan != null) {
                        btnScan.setEnabled(true);
                    }
                } else {
                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("Bluetooth not enabled");
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
                    }
                    if (btnScan != null) {
                        btnScan.setEnabled(false);
                    }
                }
            });
        } catch (Exception e) {
            Log.e(TAG, "Error in updateBluetoothStatus", e);
        }
    }

    private void updateEmptyListVisibility() {
        try {
            if (deviceList != null && tvEmptyList != null && lvDevices != null) {
                if (deviceList.isEmpty()) {
                    tvEmptyList.setVisibility(View.VISIBLE);
                    lvDevices.setVisibility(View.GONE);
                } else {
                    tvEmptyList.setVisibility(View.GONE);
                    lvDevices.setVisibility(View.VISIBLE);
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in updateEmptyListVisibility", e);
        }
    }

    private void startScan() {
        try {
            Log.d(TAG, "startScan called");
            if (bluetoothManager == null || !bluetoothManager.isBluetoothEnabled()) {
                Toast.makeText(this, "Please enable Bluetooth first", Toast.LENGTH_SHORT).show();
                checkBluetoothEnabled();
                return;
            }

            // 清空设备列表
            if (deviceList != null) {
                deviceList.clear();
                if (deviceAdapter != null) {
                    deviceAdapter.notifyDataSetChanged();
                }
                updateEmptyListVisibility();
            }

            if (btnScan != null) {
                btnScan.setText("Scanning...");
                btnScan.setEnabled(false);
            }

            if (tvBluetoothStatus != null) {
                tvBluetoothStatus.setText("Scanning for Bluetooth devices...");
            }

            Log.d(TAG, "Starting device scan");
            bluetoothManager.startScan();
        } catch (Exception e) {
            Log.e(TAG, "Error in startScan", e);
            Toast.makeText(this, "Scan failed to start: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void connectToDevice(BluetoothDeviceInfo deviceInfo) {
        try {
            Log.d(TAG, "Starting connection to device: " + deviceInfo.getName());
            if (bluetoothManager != null && !isConnecting) {
                isConnecting = true;

                if (bluetoothManager.isScanning()) {
                    bluetoothManager.stopScan();
                }

                Toast.makeText(this, "Connecting to " + deviceInfo.getName() + "...", Toast.LENGTH_SHORT).show();

                // 更新状态显示
                if (tvBluetoothStatus != null) {
                    tvBluetoothStatus.setText("Connecting to " + deviceInfo.getName() + "...");
                    tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_orange_dark));
                }

                if (btnScan != null) {
                    btnScan.setText("Connecting...");
                    btnScan.setEnabled(false);
                }

                bluetoothManager.connectToDevice(deviceInfo.getDevice());
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in connectToDevice", e);
            Toast.makeText(this, "Connection failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            isConnecting = false;
        }
    }

    // BluetoothManagerListener 接口实现
    @Override
    public void onDeviceFound(BluetoothDeviceInfo device) {
        Log.d(TAG, "Found device: " + device.getName());
        runOnUiThread(() -> {
            try {
                if (deviceList == null) return;

                // 检查设备是否已存在
                boolean exists = false;
                for (BluetoothDeviceInfo existingDevice : deviceList) {
                    if (existingDevice.getAddress().equals(device.getAddress())) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    deviceList.add(device);
                    if (deviceAdapter != null) {
                        deviceAdapter.notifyDataSetChanged();
                    }
                    updateEmptyListVisibility();
                    Log.d(TAG, "Added device to list: " + device.getName() + " (" + device.getAddress() + ")");
                    Log.d(TAG, "Current device list size: " + deviceList.size());
                }
            } catch (Exception e) {
                Log.e(TAG, "Error in onDeviceFound", e);
            }
        });
    }

    @Override
    public void onScanFinished() {
        Log.d(TAG, "Scan finished");
        runOnUiThread(() -> {
            try {
                if (btnScan != null) {
                    btnScan.setText("Scan Devices");
                    btnScan.setEnabled(true);
                }

                String message = "Scan complete, found " + (deviceList != null ? deviceList.size() : 0) + " devices";
                if (tvBluetoothStatus != null) {
                    tvBluetoothStatus.setText(message);
                    tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                }

                Toast.makeText(this, message, Toast.LENGTH_SHORT).show();

                // 调试信息
                Log.d(TAG, "Scan complete, device list:");
                if (deviceList != null) {
                    for (int i = 0; i < deviceList.size(); i++) {
                        BluetoothDeviceInfo device = deviceList.get(i);
                        Log.d(TAG, i + ": " + device.getName() + " - " + device.getAddress());
                    }
                }
            } catch (Exception e) {
                Log.e(TAG, "Error in onScanFinished", e);
            }
        });
    }

    @Override
    public void onConnectionStateChanged(boolean connected, String deviceAddress) {
        Log.d(TAG, "Connection state changed: connected=" + connected + ", address=" + deviceAddress);
        runOnUiThread(() -> {
            try {
                isConnecting = false;

                if (connected) {
                    Toast.makeText(this, "Connected successfully!", Toast.LENGTH_SHORT).show();

                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("Connected to device: " + deviceAddress);
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                    }

                    if (btnScan != null) {
                        btnScan.setText("Connected");
                        btnScan.setEnabled(false);
                    }

                    Log.d(TAG, "Preparing to navigate to control mode selection screen");

                    // 连接成功后跳转到控制模式选择界面
                    Intent intent = new Intent(MainActivity.this, ControlModeActivity.class);
                    intent.putExtra("device_address", deviceAddress);
                    startActivity(intent);

                } else {
                    Toast.makeText(this, "Connection lost", Toast.LENGTH_SHORT).show();

                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("Connection lost, please scan again");
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
                    }

                    if (btnScan != null) {
                        btnScan.setText("Scan Devices");
                        btnScan.setEnabled(true);
                    }
                }
            } catch (Exception e) {
                Log.e(TAG, "Error in onConnectionStateChanged", e);
            }
        });
    }

    @Override
    public void onDataReceived(String data) {
        Log.d(TAG, "Received data: " + data);
    }

    @Override
    public void onError(String error) {
        Log.e(TAG, "Bluetooth error: " + error);
        runOnUiThread(() -> {
            isConnecting = false;
            Toast.makeText(this, "Error: " + error, Toast.LENGTH_LONG).show();

            if (tvBluetoothStatus != null) {
                tvBluetoothStatus.setText("Error occurred: " + error);
                tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
            }

            if (btnScan != null) {
                btnScan.setText("Scan Devices");
                btnScan.setEnabled(true);
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == REQUEST_PERMISSIONS) {
            boolean allPermissionsGranted = true;
            for (int i = 0; i < grantResults.length; i++) {
                if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                    allPermissionsGranted = false;
                    Log.e(TAG, "Permission denied: " + permissions[i]);
                }
            }

            if (allPermissionsGranted) {
                Log.d(TAG, "All permissions granted");
                checkBluetoothEnabled();
            } else {
                Toast.makeText(this, "Bluetooth permissions required", Toast.LENGTH_LONG).show();
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == RESULT_OK) {
                Toast.makeText(this, "Bluetooth enabled", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "Bluetooth enabled successfully");
                updateBluetoothStatus();
            } else {
                Toast.makeText(this, "Bluetooth required to use this app", Toast.LENGTH_LONG).show();
                Log.e(TAG, "User denied Bluetooth enable");
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        try {
            Log.d(TAG, "onResume called");
            // 重置连接状态
            isConnecting = false;

            // 添加蓝牙诊断日志
            BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
            Log.d(TAG, "Bluetooth adapter status: " + (adapter != null ? "exists" : "not exists"));
            Log.d(TAG, "Bluetooth enabled: " + (adapter != null && adapter.isEnabled()));

            updateBluetoothStatus();
        } catch (Exception e) {
            Log.e(TAG, "Error in onResume", e);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            Log.d(TAG, "MainActivity onDestroy - checking if Bluetooth disconnection needed");

            // 只有在应用真正退出时才断开连接
            if (isFinishing()) {
                Log.d(TAG, "App exiting, disconnecting Bluetooth");
                if (bluetoothManager != null) {
                    bluetoothManager.disconnect(); // 使用现有的断开方法
                }
            } else {
                Log.d(TAG, "Activity switching, maintaining Bluetooth connection");
                if (bluetoothManager != null) {
                    bluetoothManager.setListener(null); // 只清理监听器
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in onDestroy", e);
        }
    }
}