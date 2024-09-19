QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arbolabb.cpp \
    arbolavl.cpp \
    arbolb.cpp \
    comentar.cpp \
    listapublicaciones.cpp \
    listnode.cpp \
    main.cpp \
    mainwindow.cpp \
    matrix.cpp \
    matrixnode.cpp \
    nuevapublicacion.cpp \
    registrarse.cpp \
    simplelist.cpp \
    vadmin.cpp \
    verarbolcomentarios.cpp \
    vercomentarios.cpp \
    vuser.cpp

HEADERS += \
    arbolabb.h \
    arbolavl.h \
    arbolb.h \
    comentar.h \
    listapublicaciones.h \
    listnode.h \
    mainwindow.h \
    matrix.h \
    matrixnode.h \
    nodoavl.h \
    nuevapublicacion.h \
    registrarse.h \
    simplelist.h \
    vadmin.h \
    verarbolcomentarios.h \
    vercomentarios.h \
    vuser.h

FORMS += \
    comentar.ui \
    mainwindow.ui \
    nuevapublicacion.ui \
    registrarse.ui \
    vadmin.ui \
    verarbolcomentarios.ui \
    vercomentarios.ui \
    vuser.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
