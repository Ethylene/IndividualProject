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
            Toast.makeText(this, "应用初始化失败: " + e.getMessage(), Toast.LENGTH_LONG).show();
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
                    Log.d(TAG, "用户点击设备: " + deviceInfo.getName());
                    connectToDevice(deviceInfo);
                }
            });

            updateBluetoothStatus();
            updateEmptyListVisibility();

            Log.d(TAG, "initViews completed successfully");

        } catch (Exception e) {
            Log.e(TAG, "CRITICAL ERROR in initViews", e);
            Toast.makeText(this, "界面初始化失败: " + e.getMessage(), Toast.LENGTH_SHORT).show();
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
            Toast.makeText(this, "蓝牙初始化失败: " + e.getMessage(), Toast.LENGTH_SHORT).show();
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
                Log.d(TAG, "请求权限: " + permissionsToRequest);
                ActivityCompat.requestPermissions(this,
                        permissionsToRequest.toArray(new String[0]), REQUEST_PERMISSIONS);
            } else {
                Log.d(TAG, "所有权限已获取");
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
                Log.d(TAG, "蓝牙已启用");
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
                        tvBluetoothStatus.setText("蓝牙已启用，点击扫描查找设备");
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                    }
                    if (btnScan != null) {
                        btnScan.setEnabled(true);
                    }
                } else {
                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("蓝牙未启用");
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
                Toast.makeText(this, "请先启用蓝牙", Toast.LENGTH_SHORT).show();
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
                btnScan.setText("扫描中...");
                btnScan.setEnabled(false);
            }

            if (tvBluetoothStatus != null) {
                tvBluetoothStatus.setText("正在扫描蓝牙设备...");
            }

            Log.d(TAG, "开始扫描设备");
            bluetoothManager.startScan();
        } catch (Exception e) {
            Log.e(TAG, "Error in startScan", e);
            Toast.makeText(this, "扫描启动失败: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void connectToDevice(BluetoothDeviceInfo deviceInfo) {
        try {
            Log.d(TAG, "开始连接设备: " + deviceInfo.getName());
            if (bluetoothManager != null && !isConnecting) {
                isConnecting = true;

                if (bluetoothManager.isScanning()) {
                    bluetoothManager.stopScan();
                }

                Toast.makeText(this, "正在连接到 " + deviceInfo.getName() + "...", Toast.LENGTH_SHORT).show();

                // 更新状态显示
                if (tvBluetoothStatus != null) {
                    tvBluetoothStatus.setText("正在连接到 " + deviceInfo.getName() + "...");
                    tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_orange_dark));
                }

                if (btnScan != null) {
                    btnScan.setText("连接中...");
                    btnScan.setEnabled(false);
                }

                bluetoothManager.connectToDevice(deviceInfo.getDevice());
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in connectToDevice", e);
            Toast.makeText(this, "连接失败: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            isConnecting = false;
        }
    }

    // BluetoothManagerListener 接口实现
    @Override
    public void onDeviceFound(BluetoothDeviceInfo device) {
        Log.d(TAG, "发现设备: " + device.getName());
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
                    Log.d(TAG, "添加设备到列表: " + device.getName() + " (" + device.getAddress() + ")");
                    Log.d(TAG, "当前设备列表大小: " + deviceList.size());
                }
            } catch (Exception e) {
                Log.e(TAG, "Error in onDeviceFound", e);
            }
        });
    }

    @Override
    public void onScanFinished() {
        Log.d(TAG, "扫描结束");
        runOnUiThread(() -> {
            try {
                if (btnScan != null) {
                    btnScan.setText("扫描设备");
                    btnScan.setEnabled(true);
                }

                String message = "扫描完成，发现 " + (deviceList != null ? deviceList.size() : 0) + " 个设备";
                if (tvBluetoothStatus != null) {
                    tvBluetoothStatus.setText(message);
                    tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                }

                Toast.makeText(this, message, Toast.LENGTH_SHORT).show();

                // 调试信息
                Log.d(TAG, "扫描完成，设备列表:");
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
        Log.d(TAG, "连接状态变化: connected=" + connected + ", address=" + deviceAddress);
        runOnUiThread(() -> {
            try {
                isConnecting = false;

                if (connected) {
                    Toast.makeText(this, "连接成功！", Toast.LENGTH_SHORT).show();

                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("已连接到设备: " + deviceAddress);
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                    }

                    if (btnScan != null) {
                        btnScan.setText("已连接");
                        btnScan.setEnabled(false);
                    }

                    Log.d(TAG, "准备跳转到控制模式选择界面");

                    // 连接成功后跳转到控制模式选择界面
                    Intent intent = new Intent(MainActivity.this, ControlModeActivity.class);
                    intent.putExtra("device_address", deviceAddress);
                    startActivity(intent);

                } else {
                    Toast.makeText(this, "连接断开", Toast.LENGTH_SHORT).show();

                    if (tvBluetoothStatus != null) {
                        tvBluetoothStatus.setText("连接断开，请重新扫描");
                        tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
                    }

                    if (btnScan != null) {
                        btnScan.setText("扫描设备");
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
        Log.d(TAG, "收到数据: " + data);
    }

    @Override
    public void onError(String error) {
        Log.e(TAG, "蓝牙错误: " + error);
        runOnUiThread(() -> {
            isConnecting = false;
            Toast.makeText(this, "错误: " + error, Toast.LENGTH_LONG).show();

            if (tvBluetoothStatus != null) {
                tvBluetoothStatus.setText("发生错误: " + error);
                tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
            }

            if (btnScan != null) {
                btnScan.setText("扫描设备");
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
                    Log.e(TAG, "权限被拒绝: " + permissions[i]);
                }
            }

            if (allPermissionsGranted) {
                Log.d(TAG, "所有权限已授予");
                checkBluetoothEnabled();
            } else {
                Toast.makeText(this, "需要权限才能使用蓝牙功能", Toast.LENGTH_LONG).show();
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == RESULT_OK) {
                Toast.makeText(this, "蓝牙已启用", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "蓝牙启用成功");
                updateBluetoothStatus();
            } else {
                Toast.makeText(this, "需要启用蓝牙才能使用", Toast.LENGTH_LONG).show();
                Log.e(TAG, "用户拒绝启用蓝牙");
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
            Log.d(TAG, "蓝牙适配器状态: " + (adapter != null ? "存在" : "不存在"));
            Log.d(TAG, "蓝牙是否启用: " + (adapter != null && adapter.isEnabled()));

            updateBluetoothStatus();
        } catch (Exception e) {
            Log.e(TAG, "Error in onResume", e);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            Log.d(TAG, "MainActivity onDestroy - 检查是否需要断开蓝牙");

            // 只有在应用真正退出时才断开连接
            if (isFinishing()) {
                Log.d(TAG, "应用退出，断开蓝牙连接");
                if (bluetoothManager != null) {
                    bluetoothManager.disconnect(); // 使用现有的断开方法
                }
            } else {
                Log.d(TAG, "Activity切换，保持蓝牙连接");
                if (bluetoothManager != null) {
                    bluetoothManager.setListener(null); // 只清理监听器
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in onDestroy", e);
        }
    }
}