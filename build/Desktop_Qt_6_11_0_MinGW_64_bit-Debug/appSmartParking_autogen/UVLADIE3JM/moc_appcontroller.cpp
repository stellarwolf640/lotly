/****************************************************************************
** Meta object code from reading C++ file 'appcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/appcontroller.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appcontroller.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13AppControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto AppController::qt_create_metaobjectdata<qt_meta_tag_ZN13AppControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppController",
        "lotsChanged",
        "",
        "selectedLotChanged",
        "recommendationChanged",
        "lastSubmissionMessageChanged",
        "locationLoadingChanged",
        "locationStatusChanged",
        "searchLocationNameChanged",
        "searchLocationChanged",
        "autocompleteSuggestionsChanged",
        "autocompleteActiveChanged",
        "selectLot",
        "lotId",
        "submitReport",
        "statusLabel",
        "refreshRecommendations",
        "geocodeAddress",
        "address",
        "updateAutocompleteQuery",
        "text",
        "selectAutocompleteSuggestion",
        "placeId",
        "displayText",
        "clearAutocomplete",
        "setSearchLocation",
        "latitude",
        "longitude",
        "useDefaultLocation",
        "refreshLots",
        "lots",
        "QVariantList",
        "parkingLotModel",
        "ParkingLotModel*",
        "selectedLot",
        "QVariantMap",
        "recommendation",
        "lastSubmissionMessage",
        "simulationManager",
        "SimulationManager*",
        "locationLoading",
        "locationStatus",
        "searchLocationName",
        "searchLat",
        "searchLng",
        "hasSearchLocation",
        "autocompleteSuggestions",
        "autocompleteActive"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'lotsChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectedLotChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'recommendationChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastSubmissionMessageChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'locationLoadingChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'locationStatusChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchLocationNameChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'searchLocationChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'autocompleteSuggestionsChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'autocompleteActiveChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'selectLot'
        QtMocHelpers::MethodData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'submitReport'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 15 },
        }}),
        // Method 'refreshRecommendations'
        QtMocHelpers::MethodData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'geocodeAddress'
        QtMocHelpers::MethodData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'updateAutocompleteQuery'
        QtMocHelpers::MethodData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Method 'selectAutocompleteSuggestion'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 22 }, { QMetaType::QString, 23 },
        }}),
        // Method 'clearAutocomplete'
        QtMocHelpers::MethodData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'setSearchLocation'
        QtMocHelpers::MethodData<void(double, double)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 26 }, { QMetaType::Double, 27 },
        }}),
        // Method 'useDefaultLocation'
        QtMocHelpers::MethodData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'refreshLots'
        QtMocHelpers::MethodData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'lots'
        QtMocHelpers::PropertyData<QVariantList>(30, 0x80000000 | 31, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'parkingLotModel'
        QtMocHelpers::PropertyData<ParkingLotModel*>(32, 0x80000000 | 33, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'selectedLot'
        QtMocHelpers::PropertyData<QVariantMap>(34, 0x80000000 | 35, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 1),
        // property 'recommendation'
        QtMocHelpers::PropertyData<QVariantMap>(36, 0x80000000 | 35, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 2),
        // property 'lastSubmissionMessage'
        QtMocHelpers::PropertyData<QString>(37, QMetaType::QString, QMC::DefaultPropertyFlags, 3),
        // property 'simulationManager'
        QtMocHelpers::PropertyData<SimulationManager*>(38, 0x80000000 | 39, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'locationLoading'
        QtMocHelpers::PropertyData<bool>(40, QMetaType::Bool, QMC::DefaultPropertyFlags, 4),
        // property 'locationStatus'
        QtMocHelpers::PropertyData<QString>(41, QMetaType::QString, QMC::DefaultPropertyFlags, 5),
        // property 'searchLocationName'
        QtMocHelpers::PropertyData<QString>(42, QMetaType::QString, QMC::DefaultPropertyFlags, 6),
        // property 'searchLat'
        QtMocHelpers::PropertyData<double>(43, QMetaType::Double, QMC::DefaultPropertyFlags, 7),
        // property 'searchLng'
        QtMocHelpers::PropertyData<double>(44, QMetaType::Double, QMC::DefaultPropertyFlags, 7),
        // property 'hasSearchLocation'
        QtMocHelpers::PropertyData<bool>(45, QMetaType::Bool, QMC::DefaultPropertyFlags, 7),
        // property 'autocompleteSuggestions'
        QtMocHelpers::PropertyData<QVariantList>(46, 0x80000000 | 31, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 8),
        // property 'autocompleteActive'
        QtMocHelpers::PropertyData<bool>(47, QMetaType::Bool, QMC::DefaultPropertyFlags, 9),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AppController, qt_meta_tag_ZN13AppControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AppController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13AppControllerE_t>.metaTypes,
    nullptr
} };

void AppController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->lotsChanged(); break;
        case 1: _t->selectedLotChanged(); break;
        case 2: _t->recommendationChanged(); break;
        case 3: _t->lastSubmissionMessageChanged(); break;
        case 4: _t->locationLoadingChanged(); break;
        case 5: _t->locationStatusChanged(); break;
        case 6: _t->searchLocationNameChanged(); break;
        case 7: _t->searchLocationChanged(); break;
        case 8: _t->autocompleteSuggestionsChanged(); break;
        case 9: _t->autocompleteActiveChanged(); break;
        case 10: _t->selectLot((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->submitReport((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 12: _t->refreshRecommendations(); break;
        case 13: _t->geocodeAddress((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->updateAutocompleteQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->selectAutocompleteSuggestion((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 16: _t->clearAutocomplete(); break;
        case 17: _t->setSearchLocation((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 18: _t->useDefaultLocation(); break;
        case 19: _t->refreshLots(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::lotsChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::selectedLotChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::recommendationChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::lastSubmissionMessageChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::locationLoadingChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::locationStatusChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::searchLocationNameChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::searchLocationChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::autocompleteSuggestionsChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)()>(_a, &AppController::autocompleteActiveChanged, 9))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ParkingLotModel* >(); break;
        case 5:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< SimulationManager* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QVariantList*>(_v) = _t->lots(); break;
        case 1: *reinterpret_cast<ParkingLotModel**>(_v) = _t->parkingLotModel(); break;
        case 2: *reinterpret_cast<QVariantMap*>(_v) = _t->selectedLot(); break;
        case 3: *reinterpret_cast<QVariantMap*>(_v) = _t->recommendation(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->lastSubmissionMessage(); break;
        case 5: *reinterpret_cast<SimulationManager**>(_v) = _t->simulationManager(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->locationLoading(); break;
        case 7: *reinterpret_cast<QString*>(_v) = _t->locationStatus(); break;
        case 8: *reinterpret_cast<QString*>(_v) = _t->searchLocationName(); break;
        case 9: *reinterpret_cast<double*>(_v) = _t->searchLat(); break;
        case 10: *reinterpret_cast<double*>(_v) = _t->searchLng(); break;
        case 11: *reinterpret_cast<bool*>(_v) = _t->hasSearchLocation(); break;
        case 12: *reinterpret_cast<QVariantList*>(_v) = _t->autocompleteSuggestions(); break;
        case 13: *reinterpret_cast<bool*>(_v) = _t->autocompleteActive(); break;
        default: break;
        }
    }
}

const QMetaObject *AppController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void AppController::lotsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AppController::selectedLotChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppController::recommendationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppController::lastSubmissionMessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppController::locationLoadingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppController::locationStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void AppController::searchLocationNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void AppController::searchLocationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void AppController::autocompleteSuggestionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void AppController::autocompleteActiveChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
