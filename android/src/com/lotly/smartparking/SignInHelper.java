package com.lotly.smartparking;

import android.content.Context;
import android.net.Uri;
import android.os.CancellationSignal;
import androidx.credentials.CredentialManager;
import androidx.credentials.CredentialManagerCallback;
import androidx.credentials.GetCredentialRequest;
import androidx.credentials.GetCredentialResponse;
import androidx.credentials.exceptions.GetCredentialException;
import com.google.android.libraries.identity.googleid.GetGoogleIdOption;
import com.google.android.libraries.identity.googleid.GoogleIdTokenCredential;
import java.util.concurrent.Executors;

/**
 * Wraps the Android Credential Manager API so C++ (via JNI) can trigger
 * Google Sign-In without touching Android SDK types directly.
 *
 * Flow:
 *   C++ calls startSignIn(activity).
 *   Credential Manager shows the system account picker.
 *   On success/failure, the static native onSignInResult() is called back
 *   into C++ (registered in JNI_OnLoad via RegisterNatives).
 */
public class SignInHelper {

    // Web (not Android) client ID — Credential Manager requires the Web
    // client ID to produce a server-verifiable ID token.
    private static final String WEB_CLIENT_ID =
        "235080414842-9vpq14ds4mf598l05ppmipca10cp7nbq.apps.googleusercontent.com";

    /**
     * Called from C++ (Qt main thread) to start the Credential Manager flow.
     * The result is delivered asynchronously via onSignInResult().
     */
    public static void startSignIn(Context context) {
        GetGoogleIdOption googleIdOption = new GetGoogleIdOption.Builder()
            .setFilterByAuthorizedAccounts(false)   // show all Google accounts, not just previously used ones
            .setServerClientId(WEB_CLIENT_ID)
            .build();

        GetCredentialRequest request = new GetCredentialRequest.Builder()
            .addCredentialOption(googleIdOption)
            .build();

        CredentialManager credentialManager = CredentialManager.create(context);
        credentialManager.getCredentialAsync(
            context,
            request,
            new CancellationSignal(),
            Executors.newSingleThreadExecutor(),
            new CredentialManagerCallback<GetCredentialResponse, GetCredentialException>() {
                @Override
                public void onResult(GetCredentialResponse result) {
                    try {
                        GoogleIdTokenCredential credential =
                            GoogleIdTokenCredential.createFrom(
                                result.getCredential().getData());

                        String idToken     = credential.getIdToken();
                        String userId      = credential.getId();   // Google account email
                        String displayName = credential.getDisplayName() != null
                                            ? credential.getDisplayName() : userId;
                        String email       = credential.getId();
                        Uri    photoUri    = credential.getProfilePictureUri();
                        String photoUrl    = (photoUri != null) ? photoUri.toString() : "";

                        onSignInResult(idToken, userId, displayName, email, photoUrl, "");
                    } catch (Exception e) {
                        onSignInResult("", "", "", "", "",
                            e.getMessage() != null ? e.getMessage() : "Unknown error");
                    }
                }

                @Override
                public void onError(GetCredentialException e) {
                    onSignInResult("", "", "", "", "",
                        e.getMessage() != null ? e.getMessage() : "Credential Manager error");
                }
            }
        );
    }

    /**
     * Native callback — implemented in googleauthservice.cpp via RegisterNatives.
     * Called on a background thread; C++ side uses QMetaObject::invokeMethod
     * (Qt::QueuedConnection) to marshal the result to the Qt main thread.
     *
     * On success: idToken, userId, displayName, email, photoUrl are non-empty; error is "".
     * On failure: error is non-empty; all other fields are "".
     */
    private static native void onSignInResult(
        String idToken, String userId, String displayName,
        String email,   String photoUrl, String error);
}
