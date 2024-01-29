package com.example.myapplication

import android.app.Service
import android.content.Intent
import android.os.IBinder
import android.os.Process
import android.util.Log

class RemoteService: Service() {

    private val binder = object : IRemoteService.Stub() {
        override fun getPid(): Int = Process.myPid()
    }

    override fun onBind(intent: Intent?): IBinder? {
        Log.d("remoteService", "onBind()");
        return binder;
    }

}