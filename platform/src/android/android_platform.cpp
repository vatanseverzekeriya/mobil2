#ifdef M2_PLATFORM_ANDROID

#include "metin2/platform/platform.hpp"
#include "metin2/core/logger.hpp"
#include <jni.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <unistd.h>

namespace metin2::platform {

/**
 * Android Platform implementation
 */
class AndroidPlatform : public IPlatform {
public:
    AndroidPlatform() : m_javaVM(nullptr), m_activity(nullptr) {}

    void initialize(JavaVM* vm, jobject activity) {
        m_javaVM = vm;
        m_activity = activity;
    }

    PlatformInfo getPlatformInfo() const override {
        PlatformInfo info;
        info.platform = PlatformType::Android;

        #ifdef M2_ARCH_ARM64
            info.architecture = Architecture::ARM64;
        #else
            info.architecture = Architecture::ARM32;
        #endif

        // OS version
        char osVersion[PROP_VALUE_MAX];
        __system_property_get("ro.build.version.release", osVersion);
        info.osVersion = osVersion;

        // Device model
        char deviceModel[PROP_VALUE_MAX];
        __system_property_get("ro.product.model", deviceModel);
        info.deviceModel = deviceModel;

        // CPU cores
        info.cpuCores = sysconf(_SC_NPROCESSORS_ONLN);

        // Memory
        long pages = sysconf(_SC_PHYS_PAGES);
        long pageSize = sysconf(_SC_PAGE_SIZE);
        info.totalMemory = pages * pageSize;

        long availPages = sysconf(_SC_AVPHYS_PAGES);
        info.availableMemory = availPages * pageSize;

        return info;
    }

    std::string getPlatformName() const override {
        return "Android";
    }

    std::string getDataPath() const override {
        // JNI call to get internal storage path
        return callJavaMethod<std::string>("getDataPath", "()Ljava/lang/String;");
    }

    std::string getCachePath() const override {
        return callJavaMethod<std::string>("getCachePath", "()Ljava/lang/String;");
    }

    std::string getTempPath() const override {
        return "/data/local/tmp";
    }

    std::string getDocumentsPath() const override {
        return getDataPath() + "/documents";
    }

    void vibrate(uint32_t durationMs) override {
        callJavaMethod<void>("vibrate", "(I)V", static_cast<jint>(durationMs));
    }

    void openURL(const std::string& url) override {
        JNIEnv* env = getJNIEnv();
        if (!env) return;

        jstring jUrl = env->NewStringUTF(url.c_str());
        callJavaMethod<void>("openURL", "(Ljava/lang/String;)V", jUrl);
        env->DeleteLocalRef(jUrl);
    }

    void showKeyboard(bool show) override {
        callJavaMethod<void>("showKeyboard", "(Z)V", static_cast<jboolean>(show));
    }

    float getBatteryLevel() const override {
        return callJavaMethod<float>("getBatteryLevel", "()F");
    }

    bool isBatteryCharging() const override {
        return callJavaMethod<bool>("isBatteryCharging", "()Z");
    }

    bool isNetworkAvailable() const override {
        return callJavaMethod<bool>("isNetworkAvailable", "()Z");
    }

    bool isWiFiConnected() const override {
        return callJavaMethod<bool>("isWiFiConnected", "()Z");
    }

    void setLowPowerMode(bool enable) override {
        callJavaMethod<void>("setLowPowerMode", "(Z)V", static_cast<jboolean>(enable));
    }

    bool isLowPowerMode() const override {
        return callJavaMethod<bool>("isLowPowerMode", "()Z");
    }

    void getScreenSize(uint32_t& width, uint32_t& height) const override {
        JNIEnv* env = getJNIEnv();
        if (!env) return;

        jclass cls = env->FindClass("com/metin2/Platform");
        jmethodID method = env->GetStaticMethodID(cls, "getScreenWidth", "()I");
        width = env->CallStaticIntMethod(cls, method);

        method = env->GetStaticMethodID(cls, "getScreenHeight", "()I");
        height = env->CallStaticIntMethod(cls, method);

        env->DeleteLocalRef(cls);
    }

    float getScreenDPI() const override {
        return callJavaMethod<float>("getScreenDPI", "()F");
    }

    void setScreenOrientation(bool landscape) override {
        callJavaMethod<void>("setScreenOrientation", "(Z)V", static_cast<jboolean>(landscape));
    }

private:
    JNIEnv* getJNIEnv() const {
        if (!m_javaVM) return nullptr;

        JNIEnv* env = nullptr;
        jint result = m_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

        if (result == JNI_EDETACHED) {
            // Attach current thread
            m_javaVM->AttachCurrentThread(&env, nullptr);
        }

        return env;
    }

    template<typename ReturnType, typename... Args>
    ReturnType callJavaMethod(const char* methodName, const char* signature, Args... args) const {
        JNIEnv* env = getJNIEnv();
        if (!env || !m_activity) {
            if constexpr (std::is_same_v<ReturnType, void>) {
                return;
            } else {
                return ReturnType{};
            }
        }

        jclass cls = env->GetObjectClass(m_activity);
        jmethodID method = env->GetMethodID(cls, methodName, signature);

        ReturnType result;
        if constexpr (std::is_same_v<ReturnType, void>) {
            env->CallVoidMethod(m_activity, method, args...);
        } else if constexpr (std::is_same_v<ReturnType, bool>) {
            result = env->CallBooleanMethod(m_activity, method, args...);
        } else if constexpr (std::is_same_v<ReturnType, int>) {
            result = env->CallIntMethod(m_activity, method, args...);
        } else if constexpr (std::is_same_v<ReturnType, float>) {
            result = env->CallFloatMethod(m_activity, method, args...);
        } else if constexpr (std::is_same_v<ReturnType, std::string>) {
            jstring jstr = static_cast<jstring>(env->CallObjectMethod(m_activity, method, args...));
            const char* str = env->GetStringUTFChars(jstr, nullptr);
            result = std::string(str);
            env->ReleaseStringUTFChars(jstr, str);
            env->DeleteLocalRef(jstr);
        }

        env->DeleteLocalRef(cls);
        return result;
    }

    JavaVM* m_javaVM;
    jobject m_activity;
};

// Global instance
static AndroidPlatform* g_androidPlatform = nullptr;

IPlatform* PlatformFactory::create() {
    if (!g_androidPlatform) {
        g_androidPlatform = new AndroidPlatform();
    }
    return g_androidPlatform;
}

void PlatformFactory::destroy(IPlatform* platform) {
    if (platform == g_androidPlatform) {
        delete g_androidPlatform;
        g_androidPlatform = nullptr;
    }
}

IPlatform& getPlatform() {
    if (!g_androidPlatform) {
        g_androidPlatform = static_cast<AndroidPlatform*>(PlatformFactory::create());
    }
    return *g_androidPlatform;
}

// JNI exports
extern "C" {

JNIEXPORT void JNICALL
Java_com_metin2_Metin2Activity_nativeInitPlatform(JNIEnv* env, jobject activity) {
    JavaVM* vm;
    env->GetJavaVM(&vm);

    auto* platform = static_cast<AndroidPlatform*>(PlatformFactory::create());
    platform->initialize(vm, env->NewGlobalRef(activity));

    M2_LOG_INFO("Android platform initialized");
}

} // extern "C"

} // namespace metin2::platform

#endif // M2_PLATFORM_ANDROID
