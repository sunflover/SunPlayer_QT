#-------------------------------------------------
#
# Project created by QtCreator 2016-01-18T16:44:07
#
#-------------------------------------------------

QT  += core gui
QT  += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

TEMPLATE = app
RC_ICONS = player.ico
RC_FILE = res.rc

TARGET = SunPlayer

MAKEFILE_GENERATOR

INCLUDEPATH +=  ../Common/
INCLUDEPATH +=  ../Common/ffmpeg-win32/include/
INCLUDEPATH +=  ../Common/pthreads-win32/include/
INCLUDEPATH +=  ../Common/ffplayer/include/

LIBS += -lwinmm -lgdi32 -luser32 -ld3d9
LIBS += -L$$PWD/../Common/ffmpeg-win32/bin/
LIBS += -lavcodec -lavformat -lavutil -lswresample -lswscale

LIBS += -L$$PWD/../Common/pthreads-win32/lib/ -lpthreadVC2

debug:LIBS += -L$$PWD/../Common/ffplayer/lib/Win32_MSVC2013.Debug/ -lffplayer
release:LIBS += -L$$PWD/../Common/ffplayer/lib/Win32_MSVC2013.Release/ -lffplayer


HEADERS  += main_widget.h \
    SingleApplication.h \
    title_widget.h \
    push_button.h \
    center_widget.h \
    ctrl_widget.h \
    SunFun.h \
    about.h \
    ../Common/ffplayer/include/ffplayer.h \
    ../Common/ffplayer/include/stdefine.h


SOURCES  += main.cpp\
    main_widget.cpp \
    SingleApplication.cpp \
    title_widget.cpp \
    push_button.cpp \
    center_widget.cpp \
    ctrl_widget.cpp \
    SunFun.cpp \
    about.cpp



RESOURCES += sunplayer.qrc
