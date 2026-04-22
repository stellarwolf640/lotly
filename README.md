# Smart Parking Availability & Prediction System

Qt Quick + QML starter application intended for Qt Creator with an Android kit.

## What is included

- QML presentation layer with four main screens
- C++ business-logic layer with placeholder service classes
- In-memory mock data behind a `DatabaseManager` abstraction
- QML-facing `AppController` and `ParkingLotModel`

## Open in Qt Creator

1. Open [CMakeLists.txt](C:/Users/Ethan Oscarson/Documents/New%20project/CMakeLists.txt).
2. Configure the project with a Qt 6.5+ Android kit.
3. Build and run on an Android emulator or device.

## Planned extension points

- Replace mock prediction logic inside `PredictionEngine`
- Add reliability weighting and anomaly detection in `DataProcessor`
- Swap `DatabaseManager` from in-memory storage to Firebase or another backend
- Replace `NotificationManager` placeholder prompts with real Android notifications
