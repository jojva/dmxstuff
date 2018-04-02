QT += widgets

HEADERS       = aseqdump.h \
                k8062.h \
                mainwindow.h \
                synesthesizer.h

SOURCES       = main.cpp \
                k8062.cpp \
                aseqdump.c \
                mainwindow.cpp \
                synesthesizer.cpp

LIBS += -lm -lasound
