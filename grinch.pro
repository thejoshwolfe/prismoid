
QT -= core gui

TARGET = grinch
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

LIBS += -lsfml-graphics

SOURCES += src/main.cpp \
    src/Entity.cpp \
    src/Game.cpp \
    src/MovingEntity.cpp \
    src/StaticEntity.cpp \
    src/Util.cpp

HEADERS += \
    src/Entity.h \
    src/Game.h \
    src/Util.h \
    src/MovingEntity.h \
    src/StaticEntity.h
