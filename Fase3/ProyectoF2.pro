QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adjacencylist.cpp \
    arbolabb.cpp \
    arbolavl.cpp \
    arbolb.cpp \
    block.cpp \
    blockchain.cpp \
    comentar.cpp \
    grafoamistades.cpp \
    huffman.cpp \
    listapublicaciones.cpp \
    listnode.cpp \
    main.cpp \
    mainwindow.cpp \
    matrix.cpp \
    matrixnode.cpp \
    merkletree.cpp \
    node.cpp \
    nuevapublicacion.cpp \
    registrarse.cpp \
    sha256.cpp \
    simplelist.cpp \
    vadmin.cpp \
    verarbolcomentarios.cpp \
    vercomentarios.cpp \
    vuser.cpp

HEADERS += \
    adjacencylist.h \
    arbolabb.h \
    arbolavl.h \
    arbolb.h \
    block.h \
    blockchain.h \
    comentar.h \
    grafoamistades.h \
    huffman.h \
    listapublicaciones.h \
    listnode.h \
    mainwindow.h \
    matrix.h \
    matrixnode.h \
    merkletree.h \
    node.h \
    nodoavl.h \
    nuevapublicacion.h \
    registrarse.h \
    sha256.h \
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
