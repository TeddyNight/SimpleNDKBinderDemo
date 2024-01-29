package com.example.myapplication

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.IBinder
import android.os.Parcel
import android.util.Log

class BinderReceiver : BroadcastReceiver() {
    companion object {
        var remoteBinder: IBinder? = null
    }
    private val tag = "BinderReceiver"

    override fun onReceive(context: Context, intent: Intent) {
        Log.d(tag, "onReceive()")
        val binder = intent.extras?.getBinder("binder")
        if (binder != null) {
            Log.d(tag, "receive remoteBinder")
            remoteBinder = binder
            Log.d(tag, "getPid()" + IRemoteService.Stub.asInterface(binder).pid)
            return
        }
        val localBinder = intent.extras?.getBinder("local")
        if (localBinder != null) {
            Log.d(tag, "receive localBinder")
            // `spinlock` ensures that remoteBinder isn't NULL
            while (remoteBinder == null) {
                Log.d(tag, "NULL remoteBinder")
            }
            ILocalService.Stub.asInterface(localBinder).onReceiveBinder(remoteBinder)
        }
    }
}