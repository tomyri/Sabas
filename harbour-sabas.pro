# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-sabas

QT += multimedia

CONFIG += sailfishapp

SOURCES += src/harbour-sabas.cpp \
    src/sabasplugin.cpp \
    src/sabaslibrary.cpp \
    src/sabasbook.cpp

OTHER_FILES += qml/harbour-sabas.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-sabas.spec \
    rpm/harbour-sabas.yaml \
    harbour-sabas.desktop \
    qml/pages/LibraryPage.qml \
    qml/pages/PlayerPage.qml \
    qml/pages/RenameDialog.qml \
    qml/pages/SleepTimeDialog.qml \
    qml/pages/HelpPage.qml \
    qml/pages/SelectTrackDialog.qml

HEADERS += \
    src/sabasplugin.h \
    src/sabaslibrary.h \
    src/sabasbook.h

CONFIG += c++11

RESOURCES += \
    resources.qrc

lupdate_only{
SOURCES +=  qml/*.qml\
            qml/pages/*.qml\
            qml/cover/*.qml
}

TRANSLATIONS = i18n/harbour-sabas_fi.ts

i18n.files = i18n
i18n.path = /usr/share/$$TARGET

INSTALLS += i18n
