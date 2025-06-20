package com.example.miniarmcontroller;

import android.os.Bundle;
import android.os.Handler;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class DirectionControlActivity extends AppCompatActivity implements BluetoothManager.BluetoothManagerListener {
    private static final String TAG = "DirectionControl";

    private BluetoothManager bluetoothManager;
    private String deviceAddress;

    // 舵机角度范围定义
    private static final int SERVO_A_MIN = 0;
    private static final int SERVO_A_MAX = 180;
    private static final int SERVO_A_DEFAULT = 90;

    private static final int SERVO_B_MIN = 0;
    private static final int SERVO_B_MAX = 77;
    private static final int SERVO_B_DEFAULT = 0;

    private static final int SERVO_C_MIN = 140;
    private static final int SERVO_C_MAX = 180;
    private static final int SERVO_C_DEFAULT = 180;

    private static final int SERVO_G_MIN = 0;
    private static final int SERVO_G_MAX = 37;
    private static final int SERVO_G_DEFAULT = 0;

    // UI组件
    private Button btnReset;
    private Button btnUp, btnDown, btnLeft, btnRight, btnStop;
    private Button btnGripperOpen, btnGripperClose;
    private TextView tvCurrentA, tvCurrentB, tvCurrentC, tvCurrentG;
    private TextView tvStepValue, tvLog;
    private SeekBar seekbarStep;

    // 当前角度值
    private int currentServoA = SERVO_A_DEFAULT;
    private int currentServoB = SERVO_B_DEFAULT;
    private int currentServoC = SERVO_C_DEFAULT;
    private int currentServoG = SERVO_G_DEFAULT;

    // 步进角度
    private int stepAngle = 5;

    // 动态肘部舵机范围
    private int servoCMin = SERVO_C_MIN;
    private int servoCMax = SERVO_C_MAX;

    // 连续移动控制
    private Handler moveHandler = new Handler();
    private Runnable moveRunnable;
    private boolean isMoving = false;
    private int moveDirection = 0; // 0=停止, 1=上, 2=下, 3=左, 4=右

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_direction_control);

        deviceAddress = getIntent().getStringExtra("device_address");

        initViews();
        initBluetooth();
        setupButtonListeners();
        updateCurrentDisplay();
        updateServoCRange();
    }

    private void initViews() {
        btnReset = findViewById(R.id.btn_reset);
        btnUp = findViewById(R.id.btn_up);
        btnDown = findViewById(R.id.btn_down);
        btnLeft = findViewById(R.id.btn_left);
        btnRight = findViewById(R.id.btn_right);
        btnStop = findViewById(R.id.btn_stop);
        btnGripperOpen = findViewById(R.id.btn_gripper_open);
        btnGripperClose = findViewById(R.id.btn_gripper_close);

        tvCurrentA = findViewById(R.id.tv_current_a);
        tvCurrentB = findViewById(R.id.tv_current_b);
        tvCurrentC = findViewById(R.id.tv_current_c);
        tvCurrentG = findViewById(R.id.tv_current_g);
        tvStepValue = findViewById(R.id.tv_step_value);
        tvLog = findViewById(R.id.tv_log);

        seekbarStep = findViewById(R.id.seekbar_step);

        // 设置日志可滚动
        tvLog.setMovementMethod(new ScrollingMovementMethod());
        tvLog.setText("");

        // 步进角度控制
        seekbarStep.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                stepAngle = progress + 1; // 1-21度
                tvStepValue.setText(stepAngle + "°");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void initBluetooth() {
        // 使用单例BluetoothManager，保持连接状态
        bluetoothManager = BluetoothManager.getInstance(this);
        bluetoothManager.setListener(this);

        // 检查连接状态
        if (bluetoothManager.isConnected()) {
            addLog("蓝牙已连接，可以发送命令");
        } else {
            addLog("警告: 蓝牙未连接");
        }
    }

    private void setupButtonListeners() {
        btnReset.setOnClickListener(v -> resetToDefault());
        btnStop.setOnClickListener(v -> stopMovement());
        btnGripperOpen.setOnClickListener(v -> openGripper());
        btnGripperClose.setOnClickListener(v -> closeGripper());

        // 设置方向按钮的长按监听
        setupDirectionButton(btnUp, 1);
        setupDirectionButton(btnDown, 2);
        setupDirectionButton(btnLeft, 3);
        setupDirectionButton(btnRight, 4);
    }

    private void setupDirectionButton(Button button, final int direction) {
        button.setOnTouchListener((v, event) -> {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    startContinuousMovement(direction);
                    break;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    stopMovement();
                    break;
                default:
                    // 其他触摸事件不处理
                    break;
            }
            return true;
        });
    }

    private void startContinuousMovement(int direction) {
        if (isMoving && moveDirection == direction) {
            return; // 已在此方向移动
        }

        stopMovement(); // 停止之前的移动

        moveDirection = direction;
        isMoving = true;

        moveRunnable = new Runnable() {
            @Override
            public void run() {
                if (isMoving) {
                    performMovement(moveDirection);
                    moveHandler.postDelayed(this, 200); // 200ms间隔
                }
            }
        };

        moveHandler.post(moveRunnable);

        String directionName = getDirectionName(direction);
        addLog("开始" + directionName + "移动");
    }

    private void stopMovement() {
        if (!isMoving) {
            return;
        }

        isMoving = false;
        moveDirection = 0;

        if (moveRunnable != null) {
            moveHandler.removeCallbacks(moveRunnable);
            moveRunnable = null;
        }

        addLog("停止移动");
    }

    private void performMovement(int direction) {
        int oldA = currentServoA;
        int oldB = currentServoB;
        int oldC = currentServoC;

        switch (direction) {
            case 1: // 上 - 增加肩部角度
                currentServoB = Math.min(currentServoB + stepAngle, SERVO_B_MAX);
                break;
            case 2: // 下 - 减少肩部角度
                currentServoB = Math.max(currentServoB - stepAngle, SERVO_B_MIN);
                break;
            case 3: // 左 - 减少基座角度
                currentServoA = Math.max(currentServoA - stepAngle, SERVO_A_MIN);
                break;
            case 4: // 右 - 增加基座角度
                currentServoA = Math.min(currentServoA + stepAngle, SERVO_A_MAX);
                break;
            default:
                // 无效方向，不执行任何操作
                Log.w(TAG, "无效的移动方向: " + direction);
                return;
        }

        // 检查是否需要更新肘部范围
        if (currentServoB != oldB) {
            updateServoCRange();
        }

        // 发送命令
        if (currentServoA != oldA) {
            sendSingleServoCommand(0, currentServoA);
        }
        if (currentServoB != oldB) {
            sendSingleServoCommand(1, currentServoB);
        }
        if (currentServoC != oldC) {
            sendSingleServoCommand(2, currentServoC);
        }

        updateCurrentDisplay();
    }

    private String getDirectionName(int direction) {
        switch (direction) {
            case 1:
                return "向上";
            case 2:
                return "向下";
            case 3:
                return "向左";
            case 4:
                return "向右";
            default:
                return "未知方向";
        }
    }

    private void openGripper() {
        currentServoG = SERVO_G_MAX;
        sendSingleServoCommand(3, currentServoG);
        updateCurrentDisplay();
        addLog("张开抓手");
    }

    private void closeGripper() {
        currentServoG = SERVO_G_MIN;
        sendSingleServoCommand(3, currentServoG);
        updateCurrentDisplay();
        addLog("闭合抓手");
    }

    private void resetToDefault() {
        stopMovement();

        currentServoA = SERVO_A_DEFAULT;
        currentServoB = SERVO_B_DEFAULT;
        currentServoC = SERVO_C_DEFAULT;
        currentServoG = SERVO_G_DEFAULT;

        updateServoCRange();
        updateCurrentDisplay();
        sendAllServoCommand();
        addLog("重置到默认位置");
    }

    private void updateServoCRange() {
        // 根据肩部角度计算肘部舵机的动态范围
        servoCMin = 140 - currentServoB;
        servoCMax = Math.min(196 - currentServoB, 180);

        // 确保范围有效
        if (servoCMin < 0) servoCMin = 0;
        if (servoCMax > 180) servoCMax = 180;
        if (servoCMin > servoCMax) servoCMin = servoCMax;

        // 调整当前肘部角度以适应新范围
        if (currentServoC < servoCMin) {
            currentServoC = servoCMin;
        } else if (currentServoC > servoCMax) {
            currentServoC = servoCMax;
        }
    }

    private void updateCurrentDisplay() {
        tvCurrentA.setText(currentServoA + "°");
        tvCurrentB.setText(currentServoB + "°");
        tvCurrentC.setText(currentServoC + "°");
        tvCurrentG.setText(currentServoG + "°");
    }

    private void sendSingleServoCommand(int servoId, int angle) {
        if (bluetoothManager == null || !bluetoothManager.isConnected()) {
            addLog("错误: 未连接到设备");
            return;
        }

        String command = "set " + servoId + " " + angle;
        boolean success = bluetoothManager.sendData(command);

        if (success) {
            addLog("发送: " + command);
        } else {
            addLog("发送失败: " + command);
        }
    }

    private void sendAllServoCommand() {
        if (bluetoothManager == null || !bluetoothManager.isConnected()) {
            addLog("错误: 未连接到设备");
            return;
        }

        String command = "setall " + currentServoA + " " + currentServoB + " " + currentServoC + " " + currentServoG;
        boolean success = bluetoothManager.sendData(command);

        if (success) {
            addLog("发送: " + command);
        } else {
            addLog("发送失败: " + command);
        }
    }

    private void addLog(String message) {
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
        String timestamp = sdf.format(new Date());
        String logEntry = "[" + timestamp + "] " + message + "\n";

        tvLog.append(logEntry);

        // 自动滚动到底部
        tvLog.post(() -> {
            if (tvLog.getLayout() != null) {
                int scrollAmount = tvLog.getLayout().getLineTop(tvLog.getLineCount()) - tvLog.getHeight();
                if (scrollAmount > 0) {
                    tvLog.scrollTo(0, scrollAmount);
                } else {
                    tvLog.scrollTo(0, 0);
                }
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
                addLog("蓝牙连接成功: " + deviceAddress);
                Toast.makeText(DirectionControlActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
            } else {
                addLog("蓝牙连接断开");
                Toast.makeText(DirectionControlActivity.this, "连接断开", Toast.LENGTH_SHORT).show();
                finish(); // 连接断开时返回上一界面
            }
        });
    }

    @Override
    public void onDataReceived(String data) {
        runOnUiThread(() -> {
            addLog("收到: " + data);
        });
    }

    @Override
    public void onError(String error) {
        runOnUiThread(() -> {
            addLog("错误: " + error);
            Toast.makeText(DirectionControlActivity.this, "错误: " + error, Toast.LENGTH_SHORT).show();
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopMovement(); // 确保停止所有移动
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

    @Override
    protected void onPause() {
        super.onPause();
        stopMovement(); // 暂停时停止移动
        Log.d(TAG, "onPause called - 保持蓝牙连接");
        // 不做任何蓝牙相关的清理
    }
}