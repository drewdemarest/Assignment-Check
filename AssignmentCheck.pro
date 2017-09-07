#-------------------------------------------------
#
# Project created by QtCreator 2017-09-04T15:37:09
#
#-------------------------------------------------

QT       += core gui network widgets networkauth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AssignmentCheck
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gmTypes/gm_i18nlocale.cpp \
    gmTypes/gm_organization.cpp \
    gmTypes/gmorganization.cpp \
    netcode/netconnect.cpp \
    netcode/oauthnetconnect.cpp \
    masterroute/masterroute.cpp

HEADERS += \
    mainwindow.h \
    gmTypes/gm_i18nlocale.h \
    gmTypes/gm_organization.h \
    jsonTypes/jsonType.hpp \
    netcode/netconnect.h \
    netcode/oauthnetconnect.h \
    masterroute/masterroute.h \
    masterroute/route.h

FORMS += \
        mainwindow.ui
