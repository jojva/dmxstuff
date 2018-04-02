QT += widgets

HEADERS       = aseqdump.h \
                channel.h \
                k8062.h \
                mainwindow.h \
                synesthesizer.h

SOURCES       = main.cpp \
                aseqdump.c \
                channel.cpp \
                k8062.cpp \
                mainwindow.cpp \
                synesthesizer.cpp

LIBS += -lm -lasound
