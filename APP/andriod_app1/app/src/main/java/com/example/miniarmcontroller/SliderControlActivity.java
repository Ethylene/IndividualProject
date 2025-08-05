package com.example.miniarmcontroller;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class SliderControlActivity extends AppCompatActivity implements BluetoothManager.BluetoothManagerListener {
    private static final String TAG = "SliderControlActivity";

    private BluetoothManager bluetoothManager;
    private String deviceAddress;

    // 舵机角度范围定义
    private static final int SERVO_A_MIN = 0;    // 基座舵机
    private static final int SERVO_A_MAX = 180;
    private static final int SERVO_A_DEFAULT = 90;

    private static final int SERVO_B_MIN = 0;    // 肩部舵机
    private static final int SERVO_B_MAX = 77;
    private static final int SERVO_B_DEFAULT = 0;

    private static final int SERVO_C_MIN = 140;  // 肘部舵机 (初始范围)
    private static final int SERVO_C_MAX = 180;
    private static final int SERVO_C_DEFAULT = 180;

    private static final int SERVO_G_MIN = 0;    // 抓手舵机
    private static final int SERVO_G_MAX = 37;
    private static final int SERVO_G_DEFAULT = 0;

    // UI组件 - 移除btnBack
    private SeekBar seekbarServoA, seekbarServoB, seekbarServoC, seekbarServoG;
    private TextView tvServoAValue, tvServoBValue, tvServoCValue, tvServoGValue;
    private TextView tvServoCRange, tvLog;
    private Button btnReset, btnPreset1, btnPreset2, btnPreset3;

    // 当前角度值
    private int currentServoA = SERVO_A_DEFAULT;
    private int currentServoB = SERVO_B_DEFAULT;
    private int currentServoC = SERVO_C_DEFAULT;
    private int currentServoG = SERVO_G_DEFAULT;

    // 动态肘部舵机范围
    private int servoCMin = SERVO_C_MIN;
    private int servoCMax = SERVO_C_MAX;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_slider_control);

        deviceAddress = getIntent().getStringExtra("device_address");

        initViews();
        initBluetooth();
        initSeekBars();
        resetToDefault();
    }

    private void initViews() {
        // 查找UI组件 - 移除btnBack查找
        seekbarServoA = findViewById(R.id.seekbar_servo_a);
        seekbarServoB = findViewById(R.id.seekbar_servo_b);
        seekbarServoC = findViewById(R.id.seekbar_servo_c);
        seekbarServoG = findViewById(R.id.seekbar_servo_g);

        tvServoAValue = findViewById(R.id.tv_servo_a_value);
        tvServoBValue = findViewById(R.id.tv_servo_b_value);
        tvServoCValue = findViewById(R.id.tv_servo_c_value);
        tvServoGValue = findViewById(R.id.tv_servo_g_value);
        tvServoCRange = findViewById(R.id.tv_servo_c_range);
        tvLog = findViewById(R.id.tv_log);

        btnReset = findViewById(R.id.btn_reset);
        btnPreset1 = findViewById(R.id.btn_preset1);
        btnPreset2 = findViewById(R.id.btn_preset2);
        btnPreset3 = findViewById(R.id.btn_preset3);

        // 设置日志可滚动
        tvLog.setMovementMethod(new ScrollingMovementMethod());
        tvLog.setText("");

        // 设置按钮点击事件 - 移除btnBack点击事件，使用系统返回键
        btnReset.setOnClickListener(v -> resetToDefault());
        btnPreset1.setOnClickListener(v -> setPresetPosition1());
        btnPreset2.setOnClickListener(v -> setPresetPosition2());
        btnPreset3.setOnClickListener(v -> setPresetPosition3());
    }

    private void initBluetooth() {
        // 使用单例BluetoothManager，保持连接状态
        bluetoothManager = BluetoothManager.getInstance(this);
        bluetoothManager.setListener(this);

        // 检查连接状态
        if (bluetoothManager.isConnected()) {
            addLog("Bluetooth connected, ready to send commands");
        } else {
            addLog("Warning: Bluetooth not connected");
        }
    }

    private void initSeekBars() {
        // 基座舵机
        seekbarServoA.setMax(SERVO_A_MAX - SERVO_A_MIN);
        seekbarServoA.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser) {
                    currentServoA = progress + SERVO_A_MIN;
                    updateServoADisplay();
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                sendSingleServoCommand(0, currentServoA);
            }
        });

        // 肩部舵机
        seekbarServoB.setMax(SERVO_B_MAX - SERVO_B_MIN);
        seekbarServoB.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser) {
                    currentServoB = progress + SERVO_B_MIN;
                    updateServoBDisplay();
                    updateServoCRange(); // 肩部角度变化时更新肘部范围
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                sendSingleServoCommand(1, currentServoB);
            }
        });

        // 肘部舵机
        seekbarServoC.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser) {
                    currentServoC = progress + servoCMin;
                    updateServoCDisplay();
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                sendSingleServoCommand(2, currentServoC);
            }
        });

        // 抓手舵机
        seekbarServoG.setMax(SERVO_G_MAX - SERVO_G_MIN);
        seekbarServoG.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (fromUser) {
                    currentServoG = progress + SERVO_G_MIN;
                    updateServoGDisplay();
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                sendSingleServoCommand(3, currentServoG);
            }
        });
    }

    private void updateServoCRange() {
        // 根据肩部角度计算肘部舵机的动态范围
        // 根据ESP32代码中的公式: minc_Angle = 140 - b_Angle, maxc_Angle = min(196 - b_Angle, 180)
        servoCMin = 140 - currentServoB;
        servoCMax = Math.min(196 - currentServoB, 180);

        // 确保范围有效
        if (servoCMin < 0) servoCMin = 0;
        if (servoCMax > 180) servoCMax = 180;
        if (servoCMin > servoCMax) servoCMin = servoCMax;

        // 更新SeekBar的最大值
        seekbarServoC.setMax(servoCMax - servoCMin);

        // 调整当前肘部角度以适应新范围
        if (currentServoC < servoCMin) {
            currentServoC = servoCMin;
        } else if (currentServoC > servoCMax) {
            currentServoC = servoCMax;
        }

        // 更新SeekBar位置
        seekbarServoC.setProgress(currentServoC - servoCMin);

        // 更新显示
        tvServoCRange.setText("Servo C (" + servoCMin + "°-" + servoCMax + "°)");
        updateServoCDisplay();
    }

    private void updateServoADisplay() {
        tvServoAValue.setText(currentServoA + "°");
    }

    private void updateServoBDisplay() {
        tvServoBValue.setText(currentServoB + "°");
    }

    private void updateServoCDisplay() {
        tvServoCValue.setText(currentServoC + "°");
    }

    private void updateServoGDisplay() {
        tvServoGValue.setText(currentServoG + "°");
    }

    private void resetToDefault() {
        currentServoA = SERVO_A_DEFAULT;
        currentServoB = SERVO_B_DEFAULT;
        currentServoC = SERVO_C_DEFAULT;
        currentServoG = SERVO_G_DEFAULT;

        updateServoCRange();

        seekbarServoA.setProgress(currentServoA - SERVO_A_MIN);
        seekbarServoB.setProgress(currentServoB - SERVO_B_MIN);
        seekbarServoG.setProgress(currentServoG - SERVO_G_MIN);

        updateServoADisplay();
        updateServoBDisplay();
        updateServoCDisplay();
        updateServoGDisplay();

        sendAllServoCommand();
        addLog("Reset to default position");
    }

    // 预设位置1 (参考Android模板代码)
    private void setPresetPosition1() {
        currentServoA = 138;
        currentServoB = 62;
        currentServoC = 124;
        currentServoG = 37;

        updatePositionAndSend();
        addLog("Set to preset position 1");
    }

    // 预设位置2
    private void setPresetPosition2() {
        currentServoA = 33;
        currentServoB = 62;
        currentServoC = 124;
        currentServoG = 0;

        updatePositionAndSend();
        addLog("Set to preset position 2");
    }

    // 预设位置3
    private void setPresetPosition3() {
        currentServoA = 50;
        currentServoB = 50;
        currentServoC = 125;
        currentServoG = 35;

        updatePositionAndSend();
        addLog("Set to preset position 3");
    }

    private void updatePositionAndSend() {
        updateServoCRange();

        // 确保角度在有效范围内
        if (currentServoA < SERVO_A_MIN) currentServoA = SERVO_A_MIN;
        if (currentServoA > SERVO_A_MAX) currentServoA = SERVO_A_MAX;
        if (currentServoB < SERVO_B_MIN) currentServoB = SERVO_B_MIN;
        if (currentServoB > SERVO_B_MAX) currentServoB = SERVO_B_MAX;
        if (currentServoC < servoCMin) currentServoC = servoCMin;
        if (currentServoC > servoCMax) currentServoC = servoCMax;
        if (currentServoG < SERVO_G_MIN) currentServoG = SERVO_G_MIN;
        if (currentServoG > SERVO_G_MAX) currentServoG = SERVO_G_MAX;

        seekbarServoA.setProgress(currentServoA - SERVO_A_MIN);
        seekbarServoB.setProgress(currentServoB - SERVO_B_MIN);
        seekbarServoC.setProgress(currentServoC - servoCMin);
        seekbarServoG.setProgress(currentServoG - SERVO_G_MIN);

        updateServoADisplay();
        updateServoBDisplay();
        updateServoCDisplay();
        updateServoGDisplay();

        sendAllServoCommand();
    }

    private void sendSingleServoCommand(int servoId, int angle) {
        if (bluetoothManager == null || !bluetoothManager.isConnected()) {
            addLog("Error: Not connected to device");
            return;
        }

        String command = "set " + servoId + " " + angle;
        boolean success = bluetoothManager.sendData(command);

        if (success) {
            addLog("Sent: " + command);
        } else {
            addLog("Send failed: " + command);
        }
    }

    private void sendAllServoCommand() {
        if (bluetoothManager == null || !bluetoothManager.isConnected()) {
            addLog("Error: Not connected to device");
            return;
        }

        String command = "setall " + currentServoA + " " + currentServoB + " " + currentServoC + " " + currentServoG;
        boolean success = bluetoothManager.sendData(command);

        if (success) {
            addLog("Sent: " + command);
        } else {
            addLog("Send failed: " + command);
        }
    }

    private void addLog(String message) {
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
        String timestamp = sdf.format(new Date());
        String logEntry = "[" + timestamp + "] " + message + "\n";

        tvLog.append(logEntry);

        // 自动滚动到底部
        tvLog.post(() -> {
            int scrollAmount = tvLog.getLayout().getLineTop(tvLog.getLineCount()) - tvLog.getHeight();
            if (scrollAmount > 0) {
                tvLog.scrollTo(0, scrollAmount);
            } else {
                tvLog.scrollTo(0, 0);
            }
        });
    }

    // BluetoothManagerListener 接口实现
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
            if (connected) {
                addLog("Bluetooth connected successfully: " + deviceAddress);
                Toast.makeText(this, "Connected successfully", Toast.LENGTH_SHORT).show();
            } else {
                addLog("Bluetooth connection lost");
                Toast.makeText(this, "Connection lost", Toast.LENGTH_SHORT).show();
                finish(); // 连接断开时返回上一界面
            }
        });
    }

    @Override
    public void onDataReceived(String data) {
        runOnUiThread(() -> {
            addLog("Received: " + data);
        });
    }

    @Override
    public void onError(String error) {
        runOnUiThread(() -> {
            addLog("Error: " + error);
            Toast.makeText(this, "Error: " + error, Toast.LENGTH_SHORT).show();
        });
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

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause called - maintaining Bluetooth connection");
        // 不做任何蓝牙相关的清理
    }
}