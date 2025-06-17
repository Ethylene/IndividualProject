package com.example.miniarmcontroller;

import android.Manifest;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.core.app.ActivityCompat;

public class BluetoothDeviceInfo {
    private BluetoothDevice device;
    private int rssi;
    private String name;
    private String address;
    private Context context;

    public BluetoothDeviceInfo(BluetoothDevice device, int rssi) {
        this.device = device;
        this.rssi = rssi;
        this.address = device.getAddress();

        // 尝试获取设备名称
        try {
            String deviceName = device.getName();
            this.name = (deviceName != null && !deviceName.isEmpty()) ? deviceName : "未知设备";
        } catch (SecurityException e) {
            this.name = "未知设备";
        }
    }

    // 带Context的构造函数，用于权限检查
    public BluetoothDeviceInfo(BluetoothDevice device, int rssi, Context context) {
        this.device = device;
        this.rssi = rssi;
        this.context = context;
        this.address = device.getAddress();

        // 根据Android版本检查权限
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (context != null &&
                    ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT)
                            == PackageManager.PERMISSION_GRANTED) {
                try {
                    String deviceName = device.getName();
                    this.name = (deviceName != null && !deviceName.isEmpty()) ? deviceName : "未知设备";
                } catch (Exception e) {
                    this.name = "未知设备";
                }
            } else {
                this.name = "未知设备";
            }
        } else {
            try {
                String deviceName = device.getName();
                this.name = (deviceName != null && !deviceName.isEmpty()) ? deviceName : "未知设备";
            } catch (Exception e) {
                this.name = "未知设备";
            }
        }
    }

    public BluetoothDevice getDevice() {
        return device;
    }

    public int getRssi() {
        return rssi;
    }

    public String getName() {
        return name;
    }

    public String getAddress() {
        return address;
    }

    public String getRssiString() {
        return rssi + " dBm";
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        BluetoothDeviceInfo that = (BluetoothDeviceInfo) obj;
        return address.equals(that.address);
    }

    @Override
    public int hashCode() {
        return address.hashCode();
    }
}