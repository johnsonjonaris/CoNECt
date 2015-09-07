#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QMainWindow>
#include <QGraphicsEllipseItem>
#include <QtCore>
#include <QtGui>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>

#include "ui_mainwindow.h"
#include "armadillo"
#include "datatypes.h"
#include "conectwiz.h"
#include "datainspectiondialog.h"
#include "reportdialog.h"
#include "myprogressdialog.h"
// visualization and GUI
#include "qxtspanslider.h"
#include "view.h"
#include "vectors.h"
#include "mrivtkwidget.h"
#include "delegate.h"
#include "roiitem.h"
// dialogs
#include "statsDialog.h"
#include "ShowQTableDialog.h"
#include "openVol.h"
#include "saveVol.h"
#include "aboutFrame.h"
#include "table.h"
#include "trackFibersDialog.h"
// data
#include "mriimage.h"
#include "diffusionmodel.h"
#include "fibertracts.h"
#include "geometryFunctions.h"
// needed for connectome stuff
#include "connectomeview.h"
#include "connectome.h"
#include "cvtkWidget.h"
#include "showmetricsdialog.h"
#include "modulardialog.h"
#include "connectomegeneratedialog.h"


#endif // MAINWINDOW2_H
