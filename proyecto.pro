TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    ospf.c \
    scheduler.c \
    Topologias.c \
    lcgrand.c

HEADERS += \
    ospf.h \
    scheduler.h \
    Topologias.h \
    lcgrand.h
