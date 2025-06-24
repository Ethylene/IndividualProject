package com.example.miniarmcontroller;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.List;

public class DeviceListAdapter extends BaseAdapter {
    private Context context;
    private List<BluetoothDeviceInfo> devices;
    private LayoutInflater inflater;

    public DeviceListAdapter(Context context, List<BluetoothDeviceInfo> devices) {
        this.context = context;
        this.devices = devices;
        this.inflater = LayoutInflater.from(context);
    }

    @Override
    public int getCount() {
        return devices.size();
    }

    @Override
    public Object getItem(int position) {
        return devices.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;

        if (convertView == null) {
            convertView = inflater.inflate(R.layout.item_device, parent, false);
            holder = new ViewHolder();
            holder.tvDeviceName = convertView.findViewById(R.id.tv_device_name);
            holder.tvDeviceAddress = convertView.findViewById(R.id.tv_device_address);
            holder.tvDeviceRssi = convertView.findViewById(R.id.tv_device_rssi);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        BluetoothDeviceInfo deviceInfo = devices.get(position);
        holder.tvDeviceName.setText(deviceInfo.getName());
        holder.tvDeviceAddress.setText(deviceInfo.getAddress());
        holder.tvDeviceRssi.setText("Signal Strength: " + deviceInfo.getRssiString());

        return convertView;
    }

    static class ViewHolder {
        TextView tvDeviceName;
        TextView tvDeviceAddress;
        TextView tvDeviceRssi;
    }

    public void updateDevices(List<BluetoothDeviceInfo> newDevices) {
        this.devices = newDevices;
        notifyDataSetChanged();
    }
}