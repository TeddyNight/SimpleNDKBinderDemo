// ILocalService.aidl
package com.example.myapplication;
import android.os.IBinder;

// Declare any non-default types here with import statements

interface ILocalService {
     // same getPid() goes here only for convenience
     int getPid();
     void onReceiveBinder(IBinder binder);
}