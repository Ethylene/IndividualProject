package com.example.miniarmcontroller;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
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
    private TextView tvDeviceAddress;
    private Button btnScan;
    private Button btnDisconnect;
    private Button btnSliderControl;
    private Button btnDirectionControl;
    private ListView lvDevices;

    private String connectedDeviceAddress = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initViews();
        initBluetooth();
        checkPermissions();
    }

    private void initViews() {
        tvBluetoothStatus = findViewById(R.id.tv_bluetooth_status);
        tvDeviceAddress = findViewById(R.id.tv_device_address);
        btnScan = findViewById(R.id.btn_scan);
        btnDisconnect = findViewById(R.id.btn_disconnect);
        btnSliderControl = findViewById(R.id.btn_slider_control);
        btnDirectionControl = findViewById(R.id.btn_direction_control);
        lvDevices = findViewById(R.id.lv_devices);

        // 设置按钮点击事件
        btnScan.setOnClickListener(v -> startScan());
        btnDisconnect.setOnClickListener(v -> disconnect());
        btnSliderControl.setOnClickListener(v -> openSliderControl());
        btnDirectionControl.setOnClickListener(v -> openDirectionControl());

        // 设置设备列表点击事件
        deviceList = new ArrayList<>();
        deviceAdapter = new DeviceListAdapter(this, deviceList);
        lvDevices.setAdapter(deviceAdapter);

        lvDevices.setOnItemClickListener((parent, view, position, id) -> {
            if (position < deviceList.size()) {
                BluetoothDeviceInfo deviceInfo = deviceList.get(position);
                Log.d(TAG, "点击设备: " + deviceInfo.getName());
                connectToDevice(deviceInfo);
            }
        });

        updateUIState(false);
    }

    private void initBluetooth() {
        bluetoothManager = new BluetoothManager(this);
        bluetoothManager.setListener(this);
    }

    private void checkPermissions() {
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
    }

    private void checkBluetoothEnabled() {
        if (!bluetoothManager.isBluetoothEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else {
            Log.d(TAG, "蓝牙已启用");
        }
    }

    private void startScan() {
        if (!bluetoothManager.isBluetoothEnabled()) {
            Toast.makeText(this, "请先启用蓝牙", Toast.LENGTH_SHORT).show();
            checkBluetoothEnabled();
            return;
        }

        // 清空设备列表
        deviceList.clear();
        deviceAdapter.notifyDataSetChanged();

        btnScan.setText("扫描中...");
        btnScan.setEnabled(false);

        Log.d(TAG, "开始扫描设备");
        bluetoothManager.startScan();
    }

    private void connectToDevice(BluetoothDeviceInfo deviceInfo) {
        if (bluetoothManager.isScanning()) {
            bluetoothManager.stopScan();
        }

        Toast.makeText(this, "正在连接到 " + deviceInfo.getName(), Toast.LENGTH_SHORT).show();
        bluetoothManager.connectToDevice(deviceInfo.getDevice());
    }

    private void disconnect() {
        bluetoothManager.disconnect();
        Toast.makeText(this, "已断开连接", Toast.LENGTH_SHORT).show();
    }

    private void openSliderControl() {
        if (!bluetoothManager.isConnected()) {
            Toast.makeText(this, "请先连接设备", Toast.LENGTH_SHORT).show();
            return;
        }

        Intent intent = new Intent(this, SliderControlActivity.class);
        intent.putExtra("device_address", connectedDeviceAddress);
        startActivity(intent);
    }

    private void openDirectionControl() {
        if (!bluetoothManager.isConnected()) {
            Toast.makeText(this, "请先连接设备", Toast.LENGTH_SHORT).show();
            return;
        }

        Intent intent = new Intent(this, DirectionControlActivity.class);
        intent.putExtra("device_address", connectedDeviceAddress);
        startActivity(intent);
    }

    private void updateUIState(boolean connected) {
        runOnUiThread(() -> {
            if (connected) {
                tvBluetoothStatus.setText("已连接");
                tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
                tvDeviceAddress.setText("设备地址: " + connectedDeviceAddress);
                btnDisconnect.setEnabled(true);
                btnSliderControl.setEnabled(true);
                btnDirectionControl.setEnabled(true);
            } else {
                tvBluetoothStatus.setText("未连接");
                tvBluetoothStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
                tvDeviceAddress.setText("设备地址: 无");
                btnDisconnect.setEnabled(false);
                btnSliderControl.setEnabled(false);
                btnDirectionControl.setEnabled(false);
                connectedDeviceAddress = "";
            }
        });
    }

    // BluetoothManagerListener 接口实现
    @Override
    public void onDeviceFound(BluetoothDeviceInfo device) {
        runOnUiThread(() -> {
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
                deviceAdapter.notifyDataSetChanged();
                Log.d(TAG, "添加设备到列表: " + device.getName() + " (" + device.getAddress() + ")");
                Log.d(TAG, "当前设备列表大小: " + deviceList.size());
            }
        });
    }

    @Override
    public void onScanFinished() {
        runOnUiThread(() -> {
            btnScan.setText("扫描设备");
            btnScan.setEnabled(true);

            String message = "扫描完成，发现 " + deviceList.size() + " 个设备";
            Toast.makeText(this, message, Toast.LENGTH_SHORT).show();

            // 调试信息
            Log.d(TAG, "扫描完成，设备列表:");
            for (int i = 0; i < deviceList.size(); i++) {
                BluetoothDeviceInfo device = deviceList.get(i);
                Log.d(TAG, i + ": " + device.getName() + " - " + device.getAddress());
            }
        });
    }

    @Override
    public void onConnectionStateChanged(boolean connected, String deviceAddress) {
        runOnUiThread(() -> {
            if (connected) {
                connectedDeviceAddress = deviceAddress;
                Toast.makeText(this, "连接成功", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, "连接断开", Toast.LENGTH_SHORT).show();
            }
            updateUIState(connected);
        });
    }

    @Override
    public void onDataReceived(String data) {
        Log.d(TAG, "收到数据: " + data);
    }

    @Override
    public void onError(String error) {
        runOnUiThread(() -> {
            Toast.makeText(this, "错误: " + error, Toast.LENGTH_LONG).show();
            Log.e(TAG, "蓝牙错误: " + error);
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
            } else {
                Toast.makeText(this, "需要启用蓝牙才能使用", Toast.LENGTH_LONG).show();
                Log.e(TAG, "用户拒绝启用蓝牙");
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        // 添加蓝牙诊断日志
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        Log.d(TAG, "蓝牙适配器状态: " + (adapter != null ? "存在" : "不存在"));
        Log.d(TAG, "蓝牙是否启用: " + (adapter != null && adapter.isEnabled()));

        updateUIState(bluetoothManager.isConnected());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (bluetoothManager != null) {
            bluetoothManager.cleanup();
        }
    }
}