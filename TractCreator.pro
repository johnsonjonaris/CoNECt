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

SOURCES += main.cpp\
        mainwindow.cpp \
        mtrand.cpp \
        view.cpp \
        vectors.cpp \
        bspline_alg.cpp \
        delegate.cpp \
        qxtspanslider.cpp \
        table.cpp \
        roiitem.cpp \
        diffusionmodel.cpp \
        fibertracts.cpp \
        mrivtkwidget.cpp \
        connectome.cpp \
        connectomeview.cpp \
        cview.cpp \
        cvtkwidget.cpp \
        misc_functions.cpp \
        mriimage.cpp \
        dw_wizardpages.cpp \
        conectwiz.cpp \
        bna_wizardpages.cpp \
    trackFibersDialog.cpp \
    statsdialog.cpp \
    sizedialog.cpp \
    ShowQTableDialog.cpp \
    showmetricsdialog.cpp \
    saveVol.cpp \
    reportdialog.cpp \
    openVol.cpp \
    myprogressdialog.cpp \
    modulardialog.cpp \
    gtabledialog.cpp \
    dtidialog.cpp \
    datainspectiondialog.cpp \
    connectomeview.cpp \
    connectomegeneratedialog.cpp \
    connectomedatainspectiondialog.cpp \
    aboutFrame.cpp \
    dblqprogressdialog.cpp


HEADERS  += mainwindow.h \
            mainwindow2.h \
            mtrand.h \
            view.h \
            vectors.h \
            bspline_alg.h \
            delegate.h \
            qxtspanslider_p.h \
            qxtspanslider.h \
            qxtglobal.h \
            table.h \
            roiitem.h \
            diffusionmodel.h \
            fibertracts.h \
            mrivtkwidget.h \
            mriimage.h \
            geometryFunctions.h \
            connectome.h \
            connectomeview.h \
            cview.h \
            cvtkwidget.h \
            misc_functions.h \
            conectwiz.h \
            datatypes.h \
    trackFibersDialog.h \
    statsDialog.h \
    sizedialog.h \
    ShowQTableDialog.h \
    showmetricsdialog.h \
    saveVol.h \
    reportdialog.h \
    openVol.h \
    myprogressdialog.h \
    modulardialog.h \
    gtabledialog.h \
    dtidialog.h \
    datainspectiondialog.h \
    connectomeview.h \
    connectomegeneratedialog.h \
    connectomedatainspectiondialog.h \
    aboutFrame.h \
    dblqprogressdialog.h


FORMS    += mainwindow.ui \
            openVol.ui \
            saveVol.ui \
            aboutFrame.ui \
            ConnectomeView.ui \
            page2dw.ui \
            page1dw.ui \
            page1bna.ui \
            intropage.ui \
    trackFibersDialog.ui \
    statsdialog.ui \
    sizedialog.ui \
    ShowQTableDialog.ui \
    showmetricsdialog.ui \
    saveVol.ui \
    reportdialog.ui \
    openVol.ui \
    myprogressdialog.ui \
    modulardialog.ui \
    gtabledialog.ui \
    dtidialog.ui \
    datainspectiondialog.ui \
    ConnectomeView.ui \
    connectomegeneratedialog.ui \
    connectomedatainspectiondialog.ui \
    aboutFrame.ui


RESOURCES     = TractCreator.qrc

win32 {
    VTK_PATH = D:/VTK/64/Install/
    LIBS += -L$$quote($${VTK_PATH}/lib/vtk-5.6) -lvtkCommon -lvtkIO -lvtkFiltering -lvtksys -lQVTK -lvtkViews -lvtkWidgets
    LIBS += -L$$quote($${VTK_PATH}/lib/vtk-5.6) -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkmetaio -lvtkHybrid
    LIBS += -L$$quote(E:/Programming/C++/libs/LAPACK/VS/x64) -lblas_win64_MT -llapack_win64_MT

    INCLUDEPATH += $$quote(E:/Programming/C++/libs/armadillo4_2/include) \
                $$quote($${VTK_PATH}/include/vtk-5.6) \
                $$quote($${VTK_PATH}/lib/vtk-5.6) \
                $$quote(E:/Programming/C++/libs/LAPACK/VS/x64)
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
