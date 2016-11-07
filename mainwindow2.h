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
#include "Wizard\conectwiz.h"
#include "Wizard\datainspectiondialog.h"
#include "Wizard\reportdialog.h"
#include "UI\myprogressdialog.h"
// visualization and GUI
#include "UI\qxtspanslider.h"
#include "UI\view.h"
#include "UI\delegate.h"
#include "DMRI\vectors.h"
#include "DMRI\mrivtkwidget.h"
#include "DMRI\roiitem.h"
// dialogs
#include "UI\statsDialog.h"
#include "UI\ShowQTableDialog.h"
#include "UI\openVol.h"
#include "UI\saveVol.h"
#include "UI\aboutFrame.h"
#include "UI\table.h"
#include "UI\trackFibersDialog.h"
// data
#include "DMRI\mriimage.h"
#include "DMRI\diffusionmodel.h"
#include "DMRI\fibertracts.h"
#include "Algorithms\geometryFunctions.h"
// needed for connectome stuff
#include "Connectome\connectomeview.h"
#include "Connectome\connectome.h"
#include "Connectome\cvtkWidget.h"
#include "UI\showmetricsdialog.h"
#include "UI\modulardialog.h"
#include "UI\connectomegeneratedialog.h"


#endif // MAINWINDOW2_H
