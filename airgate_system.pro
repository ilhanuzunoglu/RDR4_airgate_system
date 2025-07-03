#-------------------------------------------------
#
# Project created by QtCreator 2025-07-02T07:42:29
#
#-------------------------------------------------

QT       += core gui widgets serialport sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = airgate_system
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    cardreader.cpp \
    rolepage_pilot.cpp \
    rolepage_hostess.cpp \
    rolepage_passenger.cpp \
    assignrole_dialog.cpp \
    weatherinfo.cpp \
    newsinfo.cpp \
    currencyinfo.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    cardreader.h \
    rolepage_pilot.h \
    rolepage_hostess.h \
    rolepage_passenger.h \
    assignrole_dialog.h \
    weatherinfo.h \
    newsinfo.h \
    currencyinfo.h

FORMS += \
    mainwindow.ui \
    rolepage_pilot.ui \
    rolepage_hostess.ui \
    rolepage_passenger.ui \
    assignrole_dialog.ui
