TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gtk+-3.0
LIBS += `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

SOURCES += main.cpp \
    mainwindow.cpp \
    parser.cpp

HEADERS += \
    mainwindow.h \
    parser.h
