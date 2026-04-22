cmake_minimum_required(VERSION 3.21)

project(SmartParkingAvailabilityPredictionSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Network Sql)

# Qt WebView is optional — enables the embedded map tab.
# Install via Qt Maintenance Tool: Qt 6.x → Additional Libraries → Qt WebView
find_package(Qt6 OPTIONAL_COMPONENTS WebView)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appSmartParking
    main.cpp
    src/appcontroller.cpp
    src/confidencecalculator.cpp
    src/databasemanager.cpp
    src/dataprocessor.cpp
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
        qml/data/MockParkingData.qml
        qml/screens/HomeScreen.qml
        qml/screens/LotDetailScreen.qml
        qml/screens/MapScreen.qml
        qml/screens/WebMapView.qml
        qml/screens/PlaceholderScreen.qml
        qml/screens/RecommendationScreen.qml
        qml/screens/RecommendationView.qml
        qml/screens/ReportScreen.qml
        qml/screens/SettingsScreen.qml
        qml/screens/DevToolsScreen.qml
    RESOURCES
        qml/data/MockParkingData.js
        qtquickcontrols2.conf
)

target_include_directories(appSmartParking PRIVATE src)

target_sources(appSmartParking PRIVATE
    src/appcontroller.h
    src/confidencecalculator.h
    src/databasemanager.h
    src/dataprocessor.h
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
        Qt6::Sql
)

# Link WebView and expose a compile definition so C++ / QML know it's available
if(Qt6WebView_FOUND)
    target_link_libraries(appSmartParking PRIVATE Qt6::WebView)
    target_compile_definitions(appSmartParking PRIVATE HAS_QTWEBVIEW)
    message(STATUS "Qt WebView found — embedded map enabled")
else()
    message(STATUS "Qt WebView NOT found — map tab will show placeholder (install via Qt Maintenance Tool)")
endif()

set_target_properties(appSmartParking PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
)

qt_finalize_executable(appSmartParking)

include(GNUInstallDirs)
install(TARGETS appSmartParking
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
