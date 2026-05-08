cmake_minimum_required(VERSION 3.21)

# ── Android: must be set before project() so the NDK toolchain picks it up.
# Links libc++_shared.so which provides operator new/delete, __cxa_* guards,
# vtable symbols and the rest of the C++ ABI that ld.lld otherwise can't find.
if(DEFINED ANDROID_ABI)
    set(ANDROID_STL c++_shared)
endif()

project(Lotly VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Network Positioning)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appSmartParking
    main.cpp
    src/appcontroller.cpp
    src/confidencecalculator.cpp
    src/databasemanager.cpp
    src/dataprocessor.cpp
    src/firestoreservice.cpp
    src/googleauthservice.cpp
    src/lotreport.cpp
    src/notificationmanager.cpp
    src/parkinglot.cpp
    src/parkinglotmodel.cpp
    src/predictionengine.cpp
    src/recommendationengine.cpp
    src/reportmanager.cpp
    src/user.cpp
    src/autocompleteservice.cpp
    src/geocodingservice.cpp
    src/mapsservice.cpp
    src/settingsmanager.cpp
    src/simulationmanager.cpp
)

qt_add_qml_module(appSmartParking
    URI SmartParking
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/AvailabilityBar.qml
        qml/components/BottomNav.qml
        qml/components/ConfidenceBadge.qml
        qml/components/DetailRow.qml
        qml/components/LotCard.qml
        qml/components/ProgressBar.qml
        qml/components/SectionCard.qml
        qml/screens/HomeScreen.qml
        qml/screens/LotDetailScreen.qml
        qml/screens/PlaceholderScreen.qml
        qml/screens/ProfileScreen.qml
        qml/screens/RecommendationScreen.qml
        qml/screens/RecommendationView.qml
        qml/screens/ReportScreen.qml
        qml/screens/SettingsScreen.qml
        qml/screens/DevToolsScreen.qml
        qml/screens/SignInScreen.qml
    RESOURCES
        qtquickcontrols2.conf
)

target_include_directories(appSmartParking PRIVATE src)

# ── Pin the Controls style at compile time.
# This stops the QML analyser from probing every platform style
# (Windows / macOS / iOS) and producing "could not be resolved" warnings.
target_compile_definitions(appSmartParking PRIVATE
    QT_QUICK_CONTROLS_STYLE_MATERIAL
)

# ── QML import paths for Qt Creator's legacy code model.
#
# IMPORTANT — do NOT add ${CMAKE_BINARY_DIR} or ${CMAKE_BINARY_DIR}/SmartParking
# here.  Those paths flow directly into the Android deployment JSON's
# "qml-import-paths" field.  When androiddeployqt calls qmlimportscanner with
# the build directory as an import path, it finds the SmartParking module as a
# physical filesystem module and then checks whether that path lies inside Qt's
# own Android kit directories (extraPrefixDirs).  It does not — the build
# directory is a project directory — so androiddeployqt exits 4 with:
#   "Import found outside of import paths: .../SmartParking"
#
# SmartParking is compiled into the app .so by qt_add_qml_module.  At runtime
# it is loaded from the Qt resource system (prefer :/qt/qml/SmartParking/).
# androiddeployqt does not need to find it as a filesystem module at all.
# When the build directory is absent from qml-import-paths, qmlimportscanner
# returns SmartParking with no "path" field, and androiddeployqt skips it.
#
# Qt Creator's qmlls language server reads .qmlls.ini (source root) for its
# import paths — that file contains the build directory and the Desktop kit's
# qml path, so code-model analysis is unaffected by this variable.
set(QML_IMPORT_PATH
    "${CMAKE_SOURCE_DIR}/qml_stubs"
    CACHE STRING "Qt Creator QML import paths" FORCE
)

target_sources(appSmartParking PRIVATE
    src/appcontroller.h
    src/confidencecalculator.h
    src/databasemanager.h
    src/dataprocessor.h
    src/firestoreservice.h
    src/googleauthservice.h
    src/lotreport.h
    src/notificationmanager.h
    src/parkinglot.h
    src/parkinglotmodel.h
    src/predictionengine.h
    src/recommendationengine.h
    src/reportmanager.h
    src/user.h
    src/autocompleteservice.h
    src/geocodingservice.h
    src/mapsservice.h
    src/settingsmanager.h
    src/simulationmanager.h
)

target_link_libraries(appSmartParking
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Network
        Qt6::Positioning
)

# ── Android: explicitly link the shared C++ STL so all ABI symbols resolve.
# ANDROID_STL=c++_shared (set above) tells the toolchain which STL to use;
# this target_link_libraries call ensures the linker actually pulls it in.
if(ANDROID)
    target_link_libraries(appSmartParking PRIVATE c++)
endif()

set_target_properties(appSmartParking PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    # Qt 6.5+ — generates .qmlls.ini in the source tree from CMake data so
    # qmlls always finds the correct build dir and import paths automatically.
    QT_QML_GENERATE_QMLLS_INI ON
)

# ── Android: bundle OpenSSL 3.x so TLS works at runtime.
# libplugins_tls_qopensslbackend_arm64-v8a.so requires libssl_3.so and
# libcrypto_3.so — these are not part of Android's system image and must be
# shipped inside the APK.  The android_openssl package installs them via the
# Qt SDK's sdkmanager integration.
if(ANDROID)
    if(DEFINED ENV{ANDROID_SDK_ROOT})
        set(ANDROID_SDK_ROOT_PATH $ENV{ANDROID_SDK_ROOT})
    elseif(WIN32)
        set(ANDROID_SDK_ROOT_PATH "$ENV{LOCALAPPDATA}/Android/Sdk")
    endif()

    set(OPENSSL_ROOT "${ANDROID_SDK_ROOT_PATH}/android_openssl/ssl_3/${ANDROID_ABI}")

    set_target_properties(appSmartParking PROPERTIES
        QT_ANDROID_EXTRA_LIBS
        "${OPENSSL_ROOT}/libssl_3.so;${OPENSSL_ROOT}/libcrypto_3.so"
    )
endif()

qt_finalize_executable(appSmartParking)

include(GNUInstallDirs)
install(TARGETS appSmartParking
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
