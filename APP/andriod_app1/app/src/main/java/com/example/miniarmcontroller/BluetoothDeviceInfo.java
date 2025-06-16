package com.example.miniarmcontroller;

import android.bluetooth.BluetoothDevice;

public class BluetoothDeviceInfo {
    private BluetoothDevice device;
    private int rssi;
    private String name;
    private String address;

    public BluetoothDeviceInfo(BluetoothDevice device, int rssi) {
        this.device = device;
        this.rssi = rssi;
        this.name = device.getName() != null ? device.getName() : "未知设备";
        this.address = device.getAddress();
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