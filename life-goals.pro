QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = life-goals
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

INCLUDEPATH += \
    core \
    models \
    dialogs

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    core/goalitem.cpp \
    core/datamanager.cpp \
    models/user.cpp \
    models/reminder.cpp \
    dialogs/goaldialog.cpp \
    dialogs/subgoaldialog.cpp \
    dialogs/userdialog.cpp \
    dialogs/reminderdialog.cpp \
    dialogs/searchdialog.cpp

HEADERS += \
    mainwindow.h \
    core/goalitem.h \
    core/progressobserver.h \
    core/datamanager.h \
    models/user.h \
    models/reminder.h \
    dialogs/goaldialog.h \
    dialogs/subgoaldialog.h \
    dialogs/userdialog.h \
    dialogs/reminderdialog.h \
    dialogs/searchdialog.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
