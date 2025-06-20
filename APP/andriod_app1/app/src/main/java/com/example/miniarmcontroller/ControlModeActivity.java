package com.example.miniarmcontroller;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class ControlModeActivity extends AppCompatActivity {
    private static final String TAG = "ControlModeActivity";

    private String deviceAddress;
    private BluetoothManager bluetoothManager;

    // UI组件
    private TextView tvTitle;
    private TextView tvDeviceInfo;
    private Button btnSliderControl;
    private Button btnDirectionControl;
    private Button btnDisconnect;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            Log.d(TAG, "=== ControlModeActivity onCreate started ===");
            setContentView(R.layout.activity_control_mode);
            Log.d(TAG, "setContentView completed");

            deviceAddress = getIntent().getStringExtra("device_address");
            Log.d(TAG, "获取设备地址: " + deviceAddress);

            initViews();
            Log.d(TAG, "initViews completed");

            initBluetooth();
            Log.d(TAG, "initBluetooth completed");

            Log.d(TAG, "=== ControlModeActivity onCreate finished successfully ===");

        } catch (Exception e) {
            Log.e(TAG, "=== CRITICAL ERROR in ControlModeActivity onCreate ===", e);
            Toast.makeText(this, "控制模式界面初始化失败: " + e.getMessage(), Toast.LENGTH_LONG).show();
            finish();
        }
    }

    private void initViews() {
        try {
            Log.d(TAG, "Starting initViews");

            tvTitle = findViewById(R.id.tv_title);
            tvDeviceInfo = findViewById(R.id.tv_device_info);
            btnSliderControl = findViewById(R.id.btn_slider_control);
            btnDirectionControl = findViewById(R.id.btn_direction_control);
            btnDisconnect = findViewById(R.id.btn_disconnect);

            // 检查UI组件是否找到
            if (tvTitle == null) Log.e(TAG, "tv_title not found");
            if (tvDeviceInfo == null) Log.e(TAG, "tv_device_info not found");
            if (btnSliderControl == null) Log.e(TAG, "btn_slider_control not found");
            if (btnDirectionControl == null) Log.e(TAG, "btn_direction_control not found");
            if (btnDisconnect == null) Log.e(TAG, "btn_disconnect not found");

            // 显示设备信息
            if (tvDeviceInfo != null && deviceAddress != null) {
                tvDeviceInfo.setText("已连接设备: " + deviceAddress);
            }

            // 设置按钮点击事件
            if (btnSliderControl != null) {
                btnSliderControl.setOnClickListener(v -> {
                    Log.d(TAG, "用户选择滑杆控制模式");
                    openSliderControl();
                });
            }

            if (btnDirectionControl != null) {
                btnDirectionControl.setOnClickListener(v -> {
                    Log.d(TAG, "用户选择方向键控制模式");
                    openDirectionControl();
                });
            }

            if (btnDisconnect != null) {
                btnDisconnect.setOnClickListener(v -> {
                    Log.d(TAG, "用户点击断开连接");
                    disconnect();
                });
            }

            Log.d(TAG, "initViews completed successfully");

        } catch (Exception e) {
            Log.e(TAG, "Error in initViews", e);
            Toast.makeText(this, "界面初始化失败", Toast.LENGTH_SHORT).show();
        }
    }

    private void initBluetooth() {
        try {
            Log.d(TAG, "Starting initBluetooth");
            bluetoothManager = BluetoothManager.getInstance(this);
            bluetoothManager.setListener(new BluetoothManager.BluetoothManagerListener() {
                @Override
                public void onDeviceFound(BluetoothDeviceInfo device) {
                    // 不需要处理
                }

                @Override
                public void onScanFinished() {
                    // 不需要处理
                }

                @Override
                public void onConnectionStateChanged(boolean connected, String deviceAddress) {
                    runOnUiThread(() -> {
                        Log.d(TAG, "连接状态变化: " + connected);
                        if (!connected) {
                            Toast.makeText(ControlModeActivity.this, "连接断开，返回主界面", Toast.LENGTH_SHORT).show();
                            finish(); // 连接断开时返回主界面
                        }
                    });
                }

                @Override
                public void onDataReceived(String data) {
                    Log.d(TAG, "收到数据: " + data);
                }

                @Override
                public void onError(String error) {
                    runOnUiThread(() -> {
                        Log.e(TAG, "蓝牙错误: " + error);
                        Toast.makeText(ControlModeActivity.this, "错误: " + error, Toast.LENGTH_SHORT).show();
                    });
                }
            });
            Log.d(TAG, "initBluetooth completed successfully");
        } catch (Exception e) {
            Log.e(TAG, "Error in initBluetooth", e);
            Toast.makeText(this, "蓝牙初始化失败", Toast.LENGTH_SHORT).show();
        }
    }

    private void openSliderControl() {
        try {
            Log.d(TAG, "打开滑杆控制界面");
            Intent intent = new Intent(this, SliderControlActivity.class);
            intent.putExtra("device_address", deviceAddress);
            startActivity(intent);
        } catch (Exception e) {
            Log.e(TAG, "Error opening SliderControlActivity", e);
            Toast.makeText(this, "无法打开滑杆控制界面: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void openDirectionControl() {
        try {
            Log.d(TAG, "打开方向键控制界面");
            Intent intent = new Intent(this, DirectionControlActivity.class);
            intent.putExtra("device_address", deviceAddress);
            startActivity(intent);
        } catch (Exception e) {
            Log.e(TAG, "Error opening DirectionControlActivity", e);
            Toast.makeText(this, "无法打开方向键控制界面: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void disconnect() {
        try {
            Log.d(TAG, "用户主动断开蓝牙连接");
            if (bluetoothManager != null) {
                bluetoothManager.disconnect(); // 使用现有的断开方法
            }
            Toast.makeText(this, "已断开连接", Toast.LENGTH_SHORT).show();
            finish(); // 返回主界面
        } catch (Exception e) {
            Log.e(TAG, "Error disconnecting", e);
            Toast.makeText(this, "断开连接失败: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            finish(); // 即使失败也返回主界面
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            Log.d(TAG, "onDestroy called - 保持蓝牙连接");
            if (bluetoothManager != null) {
                // 不调用cleanup()，保持连接状态
                bluetoothManager.setListener(null); // 只清理监听器
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in onDestroy", e);
        }
    }
}