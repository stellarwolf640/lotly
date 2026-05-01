#include "googleauthservice.h"

#include <QDebug>
#include <QMetaObject>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniObject>
#include <jni.h>
#endif

// ── QSettings keys ────────────────────────────────────────────────────────────
static constexpr const char *SETTINGS_GROUP   = "GoogleAuth";
static constexpr const char *KEY_USER_ID      = "userId";
static constexpr const char *KEY_DISPLAY_NAME = "displayName";
static constexpr const char *KEY_EMAIL        = "email";
static constexpr const char *KEY_PHOTO_URL    = "photoUrl";
static constexpr const char *KEY_RELIABILITY  = "reliabilityScore";

// ── Static instance ───────────────────────────────────────────────────────────
GoogleAuthService *GoogleAuthService::s_instance = nullptr;

// ── JNI bridge (Android only) ─────────────────────────────────────────────────
#ifdef Q_OS_ANDROID

extern "C" {

static void jni_onSignInResult(JNIEnv *env, jclass /*clazz*/,
                               jstring idToken,     jstring userId,
                               jstring displayName, jstring email,
                               jstring photoUrl,    jstring error)
{
    auto toQString = [env](jstring s) -> QString {
        if (!s) return {};
        const char *chars = env->GetStringUTFChars(s, nullptr);
        QString result = QString::fromUtf8(chars);
        env->ReleaseStringUTFChars(s, chars);
        return result;
    };

    GoogleAuthService::deliverSignInResult(
        toQString(idToken),
        toQString(userId),
        toQString(displayName),
        toQString(email),
        toQString(photoUrl),
        toQString(error));
}

} // extern "C"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void * /*reserved*/)
{
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    jclass clazz = env->FindClass("com/lotly/smartparking/SignInHelper");
    if (!clazz) {
        qWarning() << "[Auth] JNI_OnLoad: SignInHelper class not found";
        return JNI_ERR;
    }

    static const JNINativeMethod methods[] = {
        { const_cast<char *>("onSignInResult"),
          const_cast<char *>("(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;"
                             "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),
          reinterpret_cast<void *>(jni_onSignInResult) }
    };

    if (env->RegisterNatives(clazz, methods,
                             static_cast<jint>(sizeof(methods) / sizeof(methods[0]))) < 0) {
        qWarning() << "[Auth] JNI_OnLoad: RegisterNatives failed";
        return JNI_ERR;
    }

    qDebug() << "[Auth] JNI_OnLoad: SignInHelper native registered";
    return JNI_VERSION_1_6;
}

#endif // Q_OS_ANDROID

// ── Construction ──────────────────────────────────────────────────────────────

GoogleAuthService::GoogleAuthService(QObject *parent)
    : QObject(parent)
    , m_settings(QStringLiteral("Lotly"), QStringLiteral("SmartParking"))
{
    Q_ASSERT_X(!s_instance, "GoogleAuthService",
               "Only one GoogleAuthService instance may exist at a time");
    s_instance = this;
    loadSession();
}

GoogleAuthService::~GoogleAuthService()
{
    if (s_instance == this)
        s_instance = nullptr;
}

// ── Public API ────────────────────────────────────────────────────────────────

bool GoogleAuthService::isSignedIn() const { return m_isSignedIn; }
User GoogleAuthService::currentUser() const { return m_currentUser; }

QVariantMap GoogleAuthService::currentUserMap() const
{
    return {
        {QStringLiteral("displayName"), m_currentUser.displayName()},
        {QStringLiteral("email"),       m_currentUser.email()},
        {QStringLiteral("photoUrl"),    m_currentUser.photoUrl()},
        {QStringLiteral("userId"),      m_currentUser.id()},
    };
}

void GoogleAuthService::signIn()
{
    if (m_isSignedIn) {
        qDebug() << "[Auth] Already signed in — re-emitting signInSuccess";
        emit signInSuccess(m_currentUser);
        return;
    }

#ifdef Q_OS_WINDOWS
    // ── Windows: instant dev bypass (no JNI, no network) ─────────────────────
    qDebug() << "[Auth] Windows dev bypass — signing in as Dev User";
    m_currentUser = User(
        QStringLiteral("dev-user-001"),
        QStringLiteral("Dev User"),
        QStringLiteral("dev@localhost"),
        QString(),
        1.0);
    m_isSignedIn = true;
    saveSession(m_currentUser);
    emit signInSuccess(m_currentUser);
#else
    // ── Android: trigger Credential Manager via JNI ───────────────────────────
    qDebug() << "[Auth] Launching Credential Manager via JNI";
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative",
        "activity",
        "()Landroid/app/Activity;");
    if (!activity.isValid()) {
        qWarning() << "[Auth] Could not obtain Android activity context";
        emit signInFailed(QStringLiteral("Internal error: no Android context."));
        return;
    }
    QJniObject::callStaticMethod<void>(
        "com/lotly/smartparking/SignInHelper",
        "startSignIn",
        "(Landroid/content/Context;)V",
        activity.object());
#endif
}

void GoogleAuthService::signOut()
{
    clearSession();
    emit signedOut();
}

// ── JNI result delivery ───────────────────────────────────────────────────────

/*static*/
void GoogleAuthService::deliverSignInResult(const QString &idToken,
                                            const QString &userId,
                                            const QString &displayName,
                                            const QString &email,
                                            const QString &photoUrl,
                                            const QString &error)
{
    // Called from a background JNI thread — must not touch Qt objects here.
    // Capture by value and post to the Qt main thread.
    if (s_instance) {
        QMetaObject::invokeMethod(
            s_instance,
            [idToken, userId, displayName, email, photoUrl, error]() {
                if (s_instance)
                    s_instance->onSignInResult(
                        idToken, userId, displayName, email, photoUrl, error);
            },
            Qt::QueuedConnection);
    }
}

void GoogleAuthService::onSignInResult(const QString &idToken,
                                       const QString &userId,
                                       const QString &displayName,
                                       const QString &email,
                                       const QString &photoUrl,
                                       const QString &error)
{
    // Runs on the Qt main thread.
    if (!error.isEmpty()) {
        qWarning() << "[Auth] Credential Manager sign-in failed:" << error;
        emit signInFailed(error);
        return;
    }

    if (userId.isEmpty()) {
        qWarning() << "[Auth] Sign-in succeeded but userId is empty";
        emit signInFailed(QStringLiteral("Could not retrieve user ID from Google."));
        return;
    }

    qDebug() << "[Auth] Signed in as" << displayName
             << "(" << email << ")"
             << "| idToken length:" << idToken.length();

    m_currentUser = User(userId, displayName, email, photoUrl, 1.0);
    m_isSignedIn  = true;
    saveSession(m_currentUser);
    emit signInSuccess(m_currentUser);
}

// ── Session persistence ───────────────────────────────────────────────────────

void GoogleAuthService::loadSession()
{
    m_settings.beginGroup(QLatin1StringView(SETTINGS_GROUP));
    const QString userId      = m_settings.value(QLatin1StringView(KEY_USER_ID)).toString();
    const QString displayName = m_settings.value(QLatin1StringView(KEY_DISPLAY_NAME)).toString();
    const QString email       = m_settings.value(QLatin1StringView(KEY_EMAIL)).toString();
    const QString photoUrl    = m_settings.value(QLatin1StringView(KEY_PHOTO_URL)).toString();
    const double  reliability =
        m_settings.value(QLatin1StringView(KEY_RELIABILITY), 1.0).toDouble();
    m_settings.endGroup();

    if (!userId.isEmpty()) {
        m_currentUser = User(userId, displayName, email, photoUrl, reliability);
        m_isSignedIn  = true;
        qDebug() << "[Auth] Restored session for" << displayName;
    }
}

void GoogleAuthService::saveSession(const User &user)
{
    m_settings.beginGroup(QLatin1StringView(SETTINGS_GROUP));
    m_settings.setValue(QLatin1StringView(KEY_USER_ID),      user.id());
    m_settings.setValue(QLatin1StringView(KEY_DISPLAY_NAME), user.displayName());
    m_settings.setValue(QLatin1StringView(KEY_EMAIL),        user.email());
    m_settings.setValue(QLatin1StringView(KEY_PHOTO_URL),    user.photoUrl());
    m_settings.setValue(QLatin1StringView(KEY_RELIABILITY),  user.reliabilityScore());
    m_settings.endGroup();
    m_settings.sync();
}

void GoogleAuthService::clearSession()
{
    m_settings.beginGroup(QLatin1StringView(SETTINGS_GROUP));
    m_settings.remove(QString());   // removes all keys in this group
    m_settings.endGroup();
    m_settings.sync();

    m_currentUser = User{};
    m_isSignedIn  = false;
}
