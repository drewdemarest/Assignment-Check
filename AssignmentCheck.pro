#-------------------------------------------------
#
# Project created by QtCreator 2017-09-04T15:37:09
#
#-------------------------------------------------

QT       += core gui network widgets networkauth sql

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
    netcode/netconnect.cpp \
    masterroute/masterroute.cpp \
    masterroute/route.cpp \
    routewidget.cpp \
    greenmilewidget.cpp \
    masterroutewidget.cpp \
    greenmile/greenmile.cpp \
    greenmile/routedifferencemodel.cpp \
    dailylatemasterroute/dailylatemasterroute.cpp \
    dailylategreenmilewidget.cpp \
    greenmile/routedifference.cpp \
    dailylategreenmilesettingswidget.cpp \
    netcode/oauth2.cpp \
    json2sqlite/json2sqlite.cpp

HEADERS += \
    mainwindow.h \
    netcode/netconnect.h \
    masterroute/masterroute.h \
    masterroute/route.h \
    masterroute/routestarttime.h \
    routewidget.h \
    greenmilewidget.h \
    masterroutewidget.h \
    greenmile/greenmile.h \
    greenmile/routedifference.h \
    greenmile/routedifferencemodel.h \
    dailylatemasterroute/dailylatemasterroute.h \
    dailylategreenmilewidget.h \
    dailylategreenmilesettingswidget.h \
    netcode/oauth2.h \
    json2sqlite/json2sqlite.h

FORMS += \
        mainwindow.ui \
    routewidget.ui \
    greenmilewidget.ui \
    masterroutewidget.ui \
    dailylategreenmilewidget.ui \
    dailylategreenmilesettingswidget.ui
