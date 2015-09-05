#-------------------------------------------------
#
# Project created by QtCreator 2011-07-05T15:51:23
#
#-------------------------------------------------

QT       += core gui

CONFIG -= console

TARGET = TractCreator
TEMPLATE = app

unix:!macx {
    QMAKE_CXXFLAGS += -fpermissive
}


SOURCES +=  main.cpp\
            mainwindow.cpp \
            mtrand.cpp \
            statsdialog.cpp \
            openVol.cpp \
            saveVol.cpp \
            ShowQTableDialog.cpp \
            view.cpp \
            vectors.cpp \
            scene.cpp \
            aboutFrame.cpp \
            bspline_alg.cpp \
            delegate.cpp \
            trackFibersDialog.cpp \
            qxtspanslider.cpp \
            table.cpp \
            roiitem.cpp \
            diffusionmodel.cpp \
            fibertracts.cpp \
            mrivtkwidget.cpp \
            dtidialog.cpp \
            connectome.cpp \
            connectomeview.cpp \
            cview.cpp \
            cvtkWidget.cpp \
            showmetricsdialog.cpp

HEADERS  += mainwindow.h \
            mainwindow2.h \
            mtrand.h \
            statsdialog.h \
            openVol.h \
            saveVol.h \
            ShowQTableDialog.h \
            view.h \
            vectors.h \
            scene.h \
            aboutFrame.h \
            bspline_alg.h \
            delegate.h \
            DataTypes.h \
            trackFibersDialog.h \
            qxtspanslider_p.h \
            qxtspanslider.h \
            qxtglobal.h \
            table.h \
            roiitem.h \
            diffusionmodel.h \
            fibertracts.h \
            mrivtkwidget.h \
            dtidialog.h \
            mriimage.h \
            geometryFunctions.h \
            connectome.h \
            connectomeview.h \
            cview.h \
            cvtkWidget.h \
            showmetricsdialog.h


FORMS    += mainwindow.ui \
            statsdialog.ui \
            openVol.ui \
            saveVol.ui \
            ShowQTableDialog.ui \
            aboutFrame.ui \
            trackFibersDialog.ui \
            dtidialog.ui \
            ConnectomeView.ui

RESOURCES     = TractCreator.qrc

win32 {
    LIBS += -L$$quote(C:/VTK/64/Install/lib/vtk-5.6) -lvtkCommon -lvtkIO -lvtkFiltering -lvtksys -lQVTK -lvtkViews -lvtkWidgets -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkmetaio   # -lvtkalglib -lvtkverdict  -lvtkexpat -lvtkInfovis  -lvtkNetCDF -lvtksqlite -lvtkexoIIc -lvtkftgl -lvtkfreetype -lvtkHybrid -lvtkQtChart -lvtklibxml2 -lvtkpng -lvtktiff -lvtkzlib -lvtkjpeg
    LIBS += -L$$quote(E:/Programming/C++/LAPACK/VS/x64) -lblas_win64_MT -llapack_win64_MT

    INCLUDEPATH += $$quote(E:/QT/armadillo4/include) \
                    $$quote(C:/VTK/64/Install/include/vtk-5.6) \
                    $$quote(C:/VTK/64/Install/lib/vtk-5.6) \
                    $$quote(F:/Programming/C++/LAPACK/VS/x64)
}

unix:!macx {

    INCLUDEPATH += $$/usr/include/armadillo4/include

    LIBS += -L$$/usr/lib/lapack/ -llapack

    INCLUDEPATH += $$/usr/lib/lapack
    DEPENDPATH += $$/usr/lib/lapack

    PRE_TARGETDEPS += $$/usr/lib/lapack/liblapack.a

    LIBS += -L$$/usr/lib/libblas/ -lblas

    INCLUDEPATH += $$/usr/lib/libblas
    DEPENDPATH += $$/usr/lib/libblas

    PRE_TARGETDEPS += $$/usr/lib/libblas/libblas.a

    LIBS +=  -L$$/usr/lib -lvtkCommon -lvtkIO -lvtkFiltering -lvtksys -lQVTK -lvtkViews -lvtkWidgets -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkmetaio -lvtkHybrid

    INCLUDEPATH +=  $$/usr/include/vtk-5.8 \
                    $$/usr/lib
}
