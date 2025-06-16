package com.example.miniarmcontroller;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.pm.PackageManager;
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
                Log.d("MainActivity", "点击设备: " + deviceInfo.getName());
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
        String[] permissions = {
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.ACCESS_COARSE_LOCATION,
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN
        };

        List<String> permissionsToRequest = new ArrayList<>();
        for (String permission : permissions) {
            if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                permissionsToRequest.add(permission);
            }
        }

        if (!permissionsToRequest.isEmpty()) {
            ActivityCompat.requestPermissions(this,
                    permissionsToRequest.toArray(new String[0]), REQUEST_PERMISSIONS);
        } else {
            checkBluetoothEnabled();
        }
    }

    private void checkBluetoothEnabled() {
        if (!bluetoothManager.isBluetoothEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    private void startScan() {
        if (!bluetoothManager.isBluetoothEnabled()) {
            Toast.makeText(this, "请先启用蓝牙", Toast.LENGTH_SHORT).show();
            return;
        }

        deviceList.clear();
        deviceAdapter.notifyDataSetChanged();

        btnScan.setText("扫描中...");
        btnScan.setEnabled(false);

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
    }

    // BluetoothManagerListener 接口实现

    @Override
    public void onDeviceFound(BluetoothDeviceInfo device) {
        runOnUiThread(() -> {
            if (!deviceList.contains(device)) {
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

            // 如果没有设备，添加测试数据
            if (deviceList.isEmpty()) {
                Log.d(TAG, "没有发现设备，添加测试数据");
                // 注释掉下面的测试代码，仅用于调试
                // addTestDevice();
            }
        });
    }

    // 测试方法（仅用于调试）
    private void addTestDevice() {
        // 这只是为了测试ListView是否正常显示
        // 实际使用时请删除这个方法
    /*
    BluetoothDeviceInfo testDevice = new BluetoothDeviceInfo(null, -50) {
        @Override
        public String getName() { return "测试设备"; }
        @Override
        public String getAddress() { return "00:11:22:33:44:55"; }
    };
    deviceList.add(testDevice);
    deviceAdapter.notifyDataSetChanged();
    */
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
        // 主界面不需要处理接收的数据
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
            for (int result : grantResults) {
                if (result != PackageManager.PERMISSION_GRANTED) {
                    allPermissionsGranted = false;
                    break;
                }
            }

            if (allPermissionsGranted) {
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
            } else {
                Toast.makeText(this, "需要启用蓝牙才能使用", Toast.LENGTH_LONG).show();
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        // 检查连接状态并更新UI
        updateUIState(bluetoothManager.isConnected());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (bluetoothManager != null) {
            bluetoothManager.cleanup();
        }
    }

    // 静态方法，供其他Activity获取蓝牙管理器实例
    public static BluetoothManager getBluetoothManager() {
        // 这里我们需要通过Application或单例模式来管理
        // 为简化起见，我们在每个Activity中重新创建连接
        return null;
    }
}