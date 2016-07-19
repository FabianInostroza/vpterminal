TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
TARGET = ../bin/vpterminal
RESOURCES -= application.qrc
SOURCES += main.cpp \
    mainwindow.cpp \
    serial.cpp \
    settingsdialog.cpp \
    asciidialog.cpp \
    cpphighlighter.cpp \
    qcinter.cpp \
    scriptexecwin.cpp \
    vpplaintextedit.cpp
HEADERS += mainwindow.h \
    serial.h \
    settingsdialog.h \
    asciidialog.h \
    sleep.h \
    cpphighlighter.h \
    qcinter.h \
    scriptexecwin.h \
    vpplaintextedit.h
FORMS += mainwindow.ui \
    settingsdialog.ui \
    asciidialog.ui \
    scriptexecwin.ui
RESOURCES += main.qrc
win32:LIBS += -lsetupapi
RC_FILE = vpterminal.rc
