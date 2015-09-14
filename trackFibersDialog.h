#ifndef TRACKFIBERSDIALOG_H
#define TRACKFIBERSDIALOG_H

#include <QDialog>
#include "ui_trackFibersDialog.h"
#include "datatypes.h"
#include "openVol.h"

using namespace std;

/**
  * \class TrackFibersDialog
  *
  * Dialog to specify tracking parameters.
  */
class TrackFibersDialog : public QDialog, public Ui::TrackFibersDialog
{
    Q_OBJECT
    
public:
    QVector<uchar>      orientation;
    AnalyzeHeader       AnHdr;
    uint                fileType;
    TrackingParameters  tp;

    TrackFibersDialog(QWidget *parent = 0);
    ~TrackFibersDialog() { qDeleteAll(this->children()); }
    /// import loaded images as metrics to be used for tracking
    void setLoadedImagesList(QStringList);


    
private slots:
    /// modify tracking options according to tracking method
    void changeTrackingMethod(int value);
    ///
    void changeMetricOption(int);
    void checkBeforeExit();
    void metricBoxToggled();

};

#endif // TRACKFIBERSDIALOG_H
