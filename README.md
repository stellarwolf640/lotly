# Lotly

A crowdsourced smart parking availability and prediction system for university campuses, built with Qt 6 and QML, with Firebase Firestore for cloud data sync and Google Sign-In for user authentication.

## Description

Lotly addresses the inefficiency of campus parking by combining real-time crowdsourced reports with historical patterns to generate predictive availability data. Rather than just answering *"where are parking spaces?"*, Lotly answers *"where should I park right now, and how reliable is that choice?"*

Users sign in with their Google account, view nearby parking lots ranked by predicted availability, see confidence-weighted recommendations, and contribute reports that improve predictions for everyone. The system uses time-decayed weighting to prioritize recent reports while preserving the value of historical data.

## Team Members

- Ethan Oscarson

## Technologies Used

**Frontend**
- Qt 6.11.0
- QML / Qt Quick Controls 2
- C++17

**Backend / Cloud**
- Google Cloud Firestore (NoSQL document database)
- Firebase Authentication (Google Sign-In via Credential Manager)
- Google Maps Platform — Places API (New), Geocoding API, Maps SDK for Android

**Mobile / Native**
- Android SDK (API 36)
- Android NDK 27.2.12479018
- AndroidX Credential Manager
- JNI (C++ ↔ Java interop)

**Build Tools**
- CMake 3.30+
- Gradle 9.3.1
- Android Gradle Plugin 9.0.0

**Development Environment**
- Qt Creator 16.0+
- Android Studio (for SDK/NDK management)

## Setup & Build

For complete setup instructions including required tools, Firebase configuration, API keys, and deployment steps, see [SETUP.md](SETUP.md).
