#ifndef TRACKFIBERSDIALOG_H
#define TRACKFIBERSDIALOG_H

#include <QDialog>
#include "ui_trackFibersDialog.h"
#include "DataTypes.h"
#include "openVol.h"

using namespace std;

class TrackFibersDialog : public QDialog, public Ui::TrackFibersDialog
{
    Q_OBJECT
    
public:
    TrackFibersDialog(QWidget *parent = 0);
    ~TrackFibersDialog() {qDeleteAll(this->children());}
    void setLoadedImagesList(QStringList);

    QVector<uchar>      orientation;
    AnalyzeHeader       AnHdr;
    uint                fileType;
    TrackingParameters  tp;

    
private slots:
    void changeTrackingMethod(int value);
    void changeMetricOption(int);
    void checkBeforeExit();
    void metricBoxToggled();

};

#endif // TRACKFIBERSDIALOG_H
