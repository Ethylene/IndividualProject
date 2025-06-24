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
            Log.d(TAG, "Device address: " + deviceAddress);

            initViews();
            Log.d(TAG, "initViews completed");

            initBluetooth();
            Log.d(TAG, "initBluetooth completed");

            Log.d(TAG, "=== ControlModeActivity onCreate finished successfully ===");

        } catch (Exception e) {
            Log.e(TAG, "=== CRITICAL ERROR in ControlModeActivity onCreate ===", e);
            Toast.makeText(this, "Control mode interface initialization failed: " + e.getMessage(), Toast.LENGTH_LONG).show();
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
                tvDeviceInfo.setText("Connected Device: " + deviceAddress);
            }

            // 设置按钮点击事件
            if (btnSliderControl != null) {
                btnSliderControl.setOnClickListener(v -> {
                    Log.d(TAG, "User selected slider control mode");
                    openSliderControl();
                });
            }

            if (btnDirectionControl != null) {
                btnDirectionControl.setOnClickListener(v -> {
                    Log.d(TAG, "User selected direction control mode");
                    openDirectionControl();
                });
            }

            if (btnDisconnect != null) {
                btnDisconnect.setOnClickListener(v -> {
                    Log.d(TAG, "User clicked disconnect");
                    disconnect();
                });
            }

            Log.d(TAG, "initViews completed successfully");

        } catch (Exception e) {
            Log.e(TAG, "Error in initViews", e);
            Toast.makeText(this, "Interface initialization failed", Toast.LENGTH_SHORT).show();
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
                        Log.d(TAG, "Connection state changed: " + connected);
                        if (!connected) {
                            Toast.makeText(ControlModeActivity.this, "Connection lost, returning to main screen", Toast.LENGTH_SHORT).show();
                            finish(); // 连接断开时返回主界面
                        }
                    });
                }

                @Override
                public void onDataReceived(String data) {
                    Log.d(TAG, "Received data: " + data);
                }

                @Override
                public void onError(String error) {
                    runOnUiThread(() -> {
                        Log.e(TAG, "Bluetooth error: " + error);
                        Toast.makeText(ControlModeActivity.this, "Error: " + error, Toast.LENGTH_SHORT).show();
                    });
                }
            });
            Log.d(TAG, "initBluetooth completed successfully");
        } catch (Exception e) {
            Log.e(TAG, "Error in initBluetooth", e);
            Toast.makeText(this, "Bluetooth initialization failed", Toast.LENGTH_SHORT).show();
        }
    }

    private void openSliderControl() {
        try {
            Log.d(TAG, "Opening slider control interface");
            Intent intent = new Intent(this, SliderControlActivity.class);
            intent.putExtra("device_address", deviceAddress);
            startActivity(intent);
        } catch (Exception e) {
            Log.e(TAG, "Error opening SliderControlActivity", e);
            Toast.makeText(this, "Cannot open slider control interface: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void openDirectionControl() {
        try {
            Log.d(TAG, "Opening direction control interface");
            Intent intent = new Intent(this, DirectionControlActivity.class);
            intent.putExtra("device_address", deviceAddress);
            startActivity(intent);
        } catch (Exception e) {
            Log.e(TAG, "Error opening DirectionControlActivity", e);
            Toast.makeText(this, "Cannot open direction control interface: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void disconnect() {
        try {
            Log.d(TAG, "User manually disconnecting Bluetooth");
            if (bluetoothManager != null) {
                bluetoothManager.disconnect(); // 使用现有的断开方法
            }
            Toast.makeText(this, "Disconnected", Toast.LENGTH_SHORT).show();
            finish(); // 返回主界面
        } catch (Exception e) {
            Log.e(TAG, "Error disconnecting", e);
            Toast.makeText(this, "Disconnect failed: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            finish(); // 即使失败也返回主界面
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            Log.d(TAG, "onDestroy called - maintaining Bluetooth connection");
            if (bluetoothManager != null) {
                // 不调用cleanup()，保持连接状态
                bluetoothManager.setListener(null); // 只清理监听器
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in onDestroy", e);
        }
    }
}