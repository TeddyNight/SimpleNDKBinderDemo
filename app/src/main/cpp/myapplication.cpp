#include <android/binder_ibinder.h>
#include <android/log.h>
#include <jni.h>
#include <dlfcn.h>
#include <string>
#include <unistd.h>

#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "binder_ndk", __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "binder_ndk", __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, "binder_ndk", __VA_ARGS__)

typedef AIBinder* (*AServiceManager_getService_func)(const char* instance);

//typedef binder_status_t (*AParcel_writeInt32_func)(AParcel* parcel, int32_t value);
//typedef binder_status_t (*AParcel_writeString_func)(AParcel* parcel, const char* string, int32_t length);
//typedef binder_status_t (*AParcel_writeStrongBinder_func)(AParcel* parcel, AIBinder* binder);
//typedef binder_status_t (*AIBinder_prepareTransaction_func)(AIBinder* binder, AParcel** in);
//typedef binder_status_t (*AIBinder_transact_func)(AIBinder* binder, transaction_code_t code, AParcel** in,
//                                          AParcel** out, binder_flags_t flags);
//typedef AIBinder_Class* (*AIBinder_Class_define_func)(
//        const char* interfaceDescriptor, AIBinder_Class_onCreate onCreate,
//        AIBinder_Class_onDestroy onDestroy, AIBinder_Class_onTransact onTransact);
//typedef AIBinder* (*AIBinder_new_func)(const AIBinder_Class* clazz, void* args);
//typedef bool (*AIBinder_associateClass_func)(AIBinder* binder, const AIBinder_Class* clazz);
//typedef binder_status_t (*AParcel_readStatusHeader_func)(const AParcel* parcel, AStatus** status);
//typedef bool (*AStatus_isOk_func)(const AStatus *status);
//typedef const char* (*AStatus_getMessage_func)(const AStatus* status);
//typedef int32_t (*AStatus_getServiceSpecificError_func)(const AStatus* status);
//typedef binder_exception_t (*AStatus_getExceptionCode_func)(const AStatus* status);
//typedef binder_status_t (*AStatus_getStatus_func)(const AStatus *status);

void* binder_ndk = dlopen("libbinder_ndk.so", RTLD_LAZY);

void* onCreate(void* args) {
    ALOGD("onCreate()");
    return NULL;
}
void onDestroy(void *userData) {
    ALOGD("onDestroy()");
}

binder_status_t onTransact(AIBinder* binder, transaction_code_t code,
                           const AParcel* in, AParcel* out) {
    // getPid()
    if (code == FIRST_CALL_TRANSACTION + 0) {
        ALOGD("getPid()");
        AStatus *status = AStatus_newOk();
        AParcel_writeStatusHeader(out, status);
        AParcel_writeInt32(out, 2333);
        AStatus_delete(status);
    }
    // onReceiveBinder
    if (code == FIRST_CALL_TRANSACTION + 1) {
        ALOGD("onReceiveBinder()");
        AStatus *status = AStatus_newOk();
        AIBinder *binder;
        // Interface token first?
        AParcel_readStrongBinder(in, &binder);
        if (binder == NULL) {
            ALOGE("receive null remote Binder");
        }
        else {
            ALOGD("call remote getPid()");
            AParcel *data;
            AParcel *data1;
            AIBinder_prepareTransaction(binder, &data);
            AIBinder_transact(binder, FIRST_CALL_TRANSACTION + 0, &data, &data1, 0);
            AParcel_delete(data);
            AParcel_delete(data1);
        }
        AParcel_writeStatusHeader(out, status);
        AStatus_delete(status);
    }
    ALOGD("onTransact() finish");
    return 0;
}

void AParcel_broadcastBundle(AParcel *data, const char *key, const char *serviceClz) {
    // place of length
    int prev = AParcel_getDataPosition(data);
    AParcel_writeInt32(data, 0);
    // magic number
    AParcel_writeInt32(data, 0x4C444E42);
    // element count
    AParcel_writeInt32(data, 1);
    // Map
    // first binder element
    // key: binder
    AParcel_writeString(data, key, strlen(key));
    AIBinder_Class *clazz = AIBinder_Class_define(serviceClz, onCreate, onDestroy, onTransact);
    AIBinder *binder = AIBinder_new(clazz, NULL);
    // write type code: VAL_IBINDER
    AParcel_writeInt32(data, 15);
    AParcel_writeStrongBinder(data, binder);
    int cur = AParcel_getDataPosition(data);
    // go back & write length
    AParcel_setDataPosition(data, prev);
    // write length here
    AParcel_writeInt32(data, cur - prev - 4);
    AParcel_setDataPosition(data, cur);
}

void AParcel_writeIntent(AParcel *data, const char *mPackage, const char *mClass, bool hasBundle) {
//    auto AParcel_writeInt32 = (AParcel_writeInt32_func) dlsym(binder_ndk, "AParcel_writeInt32");
//    auto AParcel_writeString = (AParcel_writeString_func) dlsym(binder_ndk, "AParcel_writeString");
    // indicate that there is an Intent?
    AParcel_writeInt32(data, 1);
    // mAction
    AParcel_writeString(data, NULL, -1);
    // mData
    AParcel_writeInt32(data, 0);
    // mType
    AParcel_writeString(data, NULL, -1);
    // mIdentifier
    AParcel_writeString(data, NULL, -1);
    // mFlags
    AParcel_writeInt32(data, 0);
    // mPackage
    AParcel_writeString(data, NULL, -1);
//    AParcel_writeString(data, mPackage, strlen(mPackage));
    // mComponent
    AParcel_writeString(data, mPackage, strlen(mPackage));
    AParcel_writeString(data, mClass, strlen(mClass));
    // mSourceBounds
    AParcel_writeInt32(data, 0);
    // mCategories
    AParcel_writeInt32(data, 0);
    // mSelector
    AParcel_writeInt32(data, 0);
    // mClipData
    AParcel_writeInt32(data, 0);
    // mContentUserHint
    AParcel_writeInt32(data, 0);
    // mExtras
    if (!hasBundle)
        AParcel_writeInt32(data, -1);
//    AParcel_putExtra(data, mClass, mPackage);
}

AIBinder* getActivityManager() {
    AServiceManager_getService_func AServiceManager_getService;
//    auto AIBinder_Class_define = (AIBinder_Class_define_func) dlsym(binder_ndk, "AIBinder_Class_define");
//    auto AIBinder_associateClass = (AIBinder_associateClass_func) dlsym(binder_ndk, "AIBinder_associateClass");
    AServiceManager_getService = (AServiceManager_getService_func) dlsym(binder_ndk, "AServiceManager_getService");
    if(AServiceManager_getService == nullptr) {
        ALOGD("Failed to look up AServiceManager_getService");
        return nullptr;
    }
    ALOGD("AServiceManager_getService symbol found at: %p", AServiceManager_getService);

    AIBinder* binder = AServiceManager_getService("activity");
    if (binder == nullptr) {
        ALOGD("Unable to obtain Activity Manager service");
        return nullptr;
    }

    ALOGD("We got the binder to the Activity Manager!: %p", binder);
    AIBinder_Class *AMSClass = AIBinder_Class_define("android.app.IActivityManager", onCreate, onDestroy,
                                          onTransact);
    if (AMSClass == nullptr) {
        ALOGE("clazz define failed");
        return NULL;
    }
    if (!AIBinder_associateClass(binder, AMSClass)) {
        ALOGE("associateClass failed");
//        return NULL;
    }

    return binder;
}

void broadcastIntent(AIBinder* binder, const char* key, const char *serviceClz) {
//    auto AParcel_writeInt32 = (AParcel_writeInt32_func) dlsym(binder_ndk, "AParcel_writeInt32");
//    auto AParcel_writeString = (AParcel_writeString_func) dlsym(binder_ndk, "AParcel_writeString");
//    auto AParcel_writeStrongBinder = (AParcel_writeStrongBinder_func) dlsym(binder_ndk, "AParcel_writeStrongBinder");
//    auto AIBinder_prepareTransaction = (AIBinder_prepareTransaction_func) dlsym(binder_ndk, "AIBinder_prepareTransaction");
//    auto AIBinder_transact = (AIBinder_transact_func) dlsym(binder_ndk, "AIBinder_transact");
//    auto AParcel_readStatusHeader = (AParcel_readStatusHeader_func) dlsym(binder_ndk, "AParcel_readStatusHeader");
//    auto AStatus_isOk = (AStatus_isOk_func) dlsym(binder_ndk, "AStatus_isOk");

//    AIBinder* binder = getActivityManager();
    if (binder == nullptr) {
        ALOGE("null activityManager handle");
        return;
    }
    AParcel *in, *out;
    AIBinder_prepareTransaction(binder, &in);
    AParcel_writeStrongBinder(in, NULL);
    const char *mPackage = "com.example.myapplication";
    const char *mClass = "com.example.myapplication.BinderReceiver";
    AParcel_writeIntent(in, mPackage, mClass, true);
    AParcel_broadcastBundle(in, key, serviceClz);
    AParcel_writeString(in, NULL, -1);
    AParcel_writeStrongBinder(in, NULL);
    AParcel_writeInt32(in, 0);
    AParcel_writeString(in, NULL, -1);
    // null bundle
    AParcel_writeInt32(in, 0);
    AParcel_writeStringArray(in, NULL, 0, NULL);
    AParcel_writeInt32(in, -1);
    // null bundle
    AParcel_writeInt32(in, 0);
    AParcel_writeBool(in, true);
    AParcel_writeBool(in, false);
    AParcel_writeInt32(in, 0);
    AIBinder_transact(binder, FIRST_CALL_TRANSACTION + 13 /* broadcastIntent */, &in, &out, 0);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(JNIEnv *env, jobject clazz) {
    std::string hello = "Hello from C++";
    AIBinder* binder = getActivityManager();
    broadcastIntent(binder, "binder", "com.example.myapplication.IRemoteService");
    broadcastIntent(binder, "local", "com.example.myapplication.ILocalService");
    return env -> NewStringUTF(hello . c_str());
}