/****************************************************************************
** Meta object code from reading C++ file 'settingsmanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/settingsmanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingsmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN15SettingsManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SettingsManager::qt_create_metaobjectdata<qt_meta_tag_ZN15SettingsManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SettingsManager",
        "darkModeChanged",
        "",
        "largeTextChanged",
        "highContrastChanged",
        "notificationsEnabledChanged",
        "reportRemindersChanged",
        "nearbyLotUpdatesChanged",
        "locationEnabledChanged",
        "useCurrentLocationChanged",
        "largerTouchTargetsChanged",
        "simplifiedDisplayChanged",
        "setDarkMode",
        "v",
        "setLargeText",
        "setHighContrast",
        "setNotificationsEnabled",
        "setReportReminders",
        "setNearbyLotUpdates",
        "setLocationEnabled",
        "setUseCurrentLocation",
        "setLargerTouchTargets",
        "setSimplifiedDisplay",
        "darkMode",
        "largeText",
        "highContrast",
        "notificationsEnabled",
        "reportReminders",
        "nearbyLotUpdates",
        "locationEnabled",
        "useCurrentLocation",
        "largerTouchTargets",
        "simplifiedDisplay"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'darkModeChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'largeTextChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'highContrastChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'notificationsEnabledChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'reportRemindersChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'nearbyLotUpdatesChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'locationEnabledChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'useCurrentLocationChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'largerTouchTargetsChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'simplifiedDisplayChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setDarkMode'
        QtMocHelpers::SlotData<void(bool)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setLargeText'
        QtMocHelpers::SlotData<void(bool)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setHighContrast'
        QtMocHelpers::SlotData<void(bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setNotificationsEnabled'
        QtMocHelpers::SlotData<void(bool)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setReportReminders'
        QtMocHelpers::SlotData<void(bool)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setNearbyLotUpdates'
        QtMocHelpers::SlotData<void(bool)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setLocationEnabled'
        QtMocHelpers::SlotData<void(bool)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setUseCurrentLocation'
        QtMocHelpers::SlotData<void(bool)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setLargerTouchTargets'
        QtMocHelpers::SlotData<void(bool)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
        // Slot 'setSimplifiedDisplay'
        QtMocHelpers::SlotData<void(bool)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'darkMode'
        QtMocHelpers::PropertyData<bool>(23, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'largeText'
        QtMocHelpers::PropertyData<bool>(24, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'highContrast'
        QtMocHelpers::PropertyData<bool>(25, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'notificationsEnabled'
        QtMocHelpers::PropertyData<bool>(26, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'reportReminders'
        QtMocHelpers::PropertyData<bool>(27, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'nearbyLotUpdates'
        QtMocHelpers::PropertyData<bool>(28, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'locationEnabled'
        QtMocHelpers::PropertyData<bool>(29, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'useCurrentLocation'
        QtMocHelpers::PropertyData<bool>(30, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'largerTouchTargets'
        QtMocHelpers::PropertyData<bool>(31, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 8),
        // property 'simplifiedDisplay'
        QtMocHelpers::PropertyData<bool>(32, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 9),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SettingsManager, qt_meta_tag_ZN15SettingsManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SettingsManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15SettingsManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15SettingsManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15SettingsManagerE_t>.metaTypes,
    nullptr
} };

void SettingsManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SettingsManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->darkModeChanged(); break;
        case 1: _t->largeTextChanged(); break;
        case 2: _t->highContrastChanged(); break;
        case 3: _t->notificationsEnabledChanged(); break;
        case 4: _t->reportRemindersChanged(); break;
        case 5: _t->nearbyLotUpdatesChanged(); break;
        case 6: _t->locationEnabledChanged(); break;
        case 7: _t->useCurrentLocationChanged(); break;
        case 8: _t->largerTouchTargetsChanged(); break;
        case 9: _t->simplifiedDisplayChanged(); break;
        case 10: _t->setDarkMode((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->setLargeText((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->setHighContrast((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->setNotificationsEnabled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->setReportReminders((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->setNearbyLotUpdates((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->setLocationEnabled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 17: _t->setUseCurrentLocation((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 18: _t->setLargerTouchTargets((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->setSimplifiedDisplay((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::darkModeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::largeTextChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::highContrastChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::notificationsEnabledChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::reportRemindersChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::nearbyLotUpdatesChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::locationEnabledChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::useCurrentLocationChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::largerTouchTargetsChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsManager::*)()>(_a, &SettingsManager::simplifiedDisplayChanged, 9))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->darkMode(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->largeText(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->highContrast(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->notificationsEnabled(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->reportReminders(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->nearbyLotUpdates(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->locationEnabled(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->useCurrentLocation(); break;
        case 8: *reinterpret_cast<bool*>(_v) = _t->largerTouchTargets(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->simplifiedDisplay(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDarkMode(*reinterpret_cast<bool*>(_v)); break;
        case 1: _t->setLargeText(*reinterpret_cast<bool*>(_v)); break;
        case 2: _t->setHighContrast(*reinterpret_cast<bool*>(_v)); break;
        case 3: _t->setNotificationsEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 4: _t->setReportReminders(*reinterpret_cast<bool*>(_v)); break;
        case 5: _t->setNearbyLotUpdates(*reinterpret_cast<bool*>(_v)); break;
        case 6: _t->setLocationEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 7: _t->setUseCurrentLocation(*reinterpret_cast<bool*>(_v)); break;
        case 8: _t->setLargerTouchTargets(*reinterpret_cast<bool*>(_v)); break;
        case 9: _t->setSimplifiedDisplay(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *SettingsManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SettingsManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15SettingsManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SettingsManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 20;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SettingsManager::darkModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SettingsManager::largeTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SettingsManager::highContrastChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SettingsManager::notificationsEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SettingsManager::reportRemindersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SettingsManager::nearbyLotUpdatesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SettingsManager::locationEnabledChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void SettingsManager::useCurrentLocationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void SettingsManager::largerTouchTargetsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void SettingsManager::simplifiedDisplayChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
