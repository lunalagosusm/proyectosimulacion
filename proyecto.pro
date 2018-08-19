TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    lcgrand.c \
    ospf.c \
    scheduler.c \
    Topologias.c

HEADERS += \
    lcgrand.h \
    ospf.h \
    scheduler.h \
    Topologias.h
