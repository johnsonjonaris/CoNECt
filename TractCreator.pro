#-------------------------------------------------
#
# Project created by QtCreator 2011-07-05T15:51:23
#
#-------------------------------------------------

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG -= console

TARGET = CoNECt
TEMPLATE = app

unix {
    QMAKE_CXXFLAGS += -fpermissive
}

SOURCES +=  main.cpp\
            mainwindow.cpp \
            Algorithms\*.cpp \
            DMRI\*.cpp \
            Connectome\*.cpp \
            UI\*.cpp \
            Wizard\*.cpp


HEADERS  += mainwindow.h \
            mainwindow2.h \
            datatypes.h \
            Algorithms\*.h \
            DMRI\*.h \
            Connectome\*.h \
            UI\*.h \
            Wizard\*.h

FORMS    += mainwindow.ui \
            Connectome\*.ui \
            UI\*.ui \
            Wizard\*.ui

RESOURCES     = TractCreator.qrc

win32 {
    VTK_PATH = C:/VTK/5.10.1_2013/64/Release
#    Release: VTK_PATH = C:/VTK/5.10.1_2013/64/Release
#    Debug: VTK_PATH = C:/VTK/5.10.1_2013/64/Debug

    LIBS += -L$$quote($${VTK_PATH}/lib/vtk-5.10) -lvtkCommon -lvtkIO -lvtkFiltering -lvtksys -lQVTK -lvtkViews -lvtkWidgets
    LIBS += -L$$quote($${VTK_PATH}/lib/vtk-5.10) -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkmetaio -lvtkHybrid
    LIBS += -L$$quote(F:/Workspace/C++/libs/LAPACK/VS/x64) -lblas_win64_MT -llapack_win64_MT

    INCLUDEPATH += $$quote(F:/Workspace/C++/libs/armadillo4_2/include) \
                $$quote($${VTK_PATH}/include/vtk-5.10) \
                $$quote($${VTK_PATH}/lib/vtk-5.10) \
                $$quote($${VTK_PATH}/bin/) \
                $$quote(F:/Workspace/C++/libs/LAPACK/VS/x64)
}

unix:!macx {
    # armadillo
    INCLUDEPATH += $$/usr/include/armadillo4/include
    # lapack
    LIBS            += -L$$/usr/lib/lapack/ -llapack
    INCLUDEPATH     += $$/usr/lib/lapack
    DEPENDPATH      += $$/usr/lib/lapack
    PRE_TARGETDEPS  += $$/usr/lib/lapack/liblapack.a
    # blas
    LIBS            += -L$$/usr/lib/libblas/ -lblas
    INCLUDEPATH     += $$/usr/lib/libblas
    DEPENDPATH      += $$/usr/lib/libblas
    PRE_TARGETDEPS  += $$/usr/lib/libblas/libblas.a
    # VTK
    LIBS +=  -L$$/usr/lib -lvtkCommon -lvtkIO -lvtkFiltering -lvtksys -lQVTK -lvtkViews -lvtkWidgets -lvtkRendering
    LIBS +=  -L$$/usr/lib -lvtkGraphics -lvtkImaging -lvtkmetaio -lvtkHybrid
    INCLUDEPATH +=  $$/usr/include/vtk-5.8 \
                    $$/usr/lib
}
