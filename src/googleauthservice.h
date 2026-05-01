#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariantMap>

#include "user.h"

// ─────────────────────────────────────────────────────────────────────────────
// GoogleAuthService — Google Sign-In via Android Credential Manager API
//
// Flow (Android)
// ──────────────
//  1. QML calls appController.signIn().
//  2. signIn() calls SignInHelper.startSignIn(activity) via QJniObject.
//  3. Credential Manager shows the system Google account picker (no browser).
//  4. Java calls the native onSignInResult() registered in JNI_OnLoad.
//  5. jni_onSignInResult() (C++) invokes deliverSignInResult() from the
//     background thread used by Credential Manager.
//  6. deliverSignInResult() posts onSignInResult() to the Qt main thread
//     via QMetaObject::invokeMethod (Qt::QueuedConnection).
//  7. onSignInResult() builds the User, persists the session, emits signInSuccess.
//
// Flow (Windows dev-mode)
// ────────────────────────
//  signIn() immediately creates a hardcoded dev user and emits signInSuccess.
//  No JNI, no network.
//
// ─────────────────────────────────────────────────────────────────────────────
class GoogleAuthService : public QObject
{
    Q_OBJECT

public:
    explicit GoogleAuthService(QObject *parent = nullptr);
    ~GoogleAuthService() override;

    bool        isSignedIn()     const;
    User        currentUser()    const;
    QVariantMap currentUserMap() const;

    // Android : triggers the system Google account picker via Credential Manager.
    // Windows : instantly signs in as the hardcoded dev user.
    // If already signed in, re-emits signInSuccess immediately.
    void signIn();
    void signOut();

    // Called from jni_onSignInResult (background JNI thread).
    // Marshals the result to the Qt main thread via QMetaObject::invokeMethod.
    static void deliverSignInResult(const QString &idToken,
                                    const QString &userId,
                                    const QString &displayName,
                                    const QString &email,
                                    const QString &photoUrl,
                                    const QString &error);

signals:
    void signInSuccess(const User &user);
    void signInFailed(const QString &error);
    void signedOut();

private:
    // Invoked on the Qt main thread by deliverSignInResult().
    void onSignInResult(const QString &idToken,
                        const QString &userId,
                        const QString &displayName,
                        const QString &email,
                        const QString &photoUrl,
                        const QString &error);

    void loadSession();
    void saveSession(const User &user);
    void clearSession();

    QSettings m_settings;
    User      m_currentUser;
    bool      m_isSignedIn = false;

    // Singleton pointer used by the static JNI callback to reach the live instance.
    static GoogleAuthService *s_instance;
};
