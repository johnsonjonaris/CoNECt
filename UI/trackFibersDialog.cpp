#include "trackFibersDialog.h"

TrackFibersDialog::TrackFibersDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    connect(TrackingMethodBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(changeTrackingMethod(int)));
    connect(LimitingMetric,SIGNAL(activated(int)),
            this,SLOT(changeMetricOption(int)));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(checkBeforeExit()));
    connect(MetricBox, SIGNAL(clicked()), this, SLOT(metricBoxToggled()));
    fileType = NULL;
    changeTrackingMethod(0);
}

void TrackFibersDialog::setLoadedImagesList(QStringList list)
{
    LimitingMetric->insertItems(2,list);
}

void TrackFibersDialog::changeTrackingMethod(int value)
{
    // enable tract per seed for probabilistic tractography
    TractsPerSeed->setEnabled(bool(value > 1));
    // FA is a must for FACT
    MetricBox->setChecked(MetricBox->isChecked() || bool(value == 0));
    LimitingMetric->setEnabled(bool(value != 0) && MetricBox->isChecked());
    if (value == 0) {
        LimitingMetric->setCurrentIndex(0);
        filePath->clear();
        StartTrackingValue->setValidator(new QDoubleValidator(0,1.0,4,this));
        StopTrackingValue->setValidator(new QDoubleValidator(0,1.0,4,this));
    }
}

void TrackFibersDialog::changeMetricOption(int value)
{
    filePath->setEnabled(bool(value == 1));
    fileType = NULL;
    if (value == 0) {
        StartTrackingValue->setValidator(new QDoubleValidator(0,1.0,4,this));
        StopTrackingValue->setValidator(new QDoubleValidator(0,1.0,4,this));
    }
    else {
        StartTrackingValue->setValidator(new QDoubleValidator(-1000,1000,4,this));
        StopTrackingValue->setValidator(new QDoubleValidator(-1000,1000,4,this));
    }
    if (value == 1) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        "Load Image", ".",
                                                        "All files (*.*)\n");
        if (!fileName.isEmpty()) {
            OpenImageDialog w(fileName,this);
            if (w.exec()) {
                filePath->setText(fileName);
                fileType = w.fileType();
                orientation = w.orientation;
                if (w.fileType() == RAW)
                    w.getHeader(AnHdr);
            }
        }
    }
}

void TrackFibersDialog::checkBeforeExit()
{
    bool test;
    if (MetricBox->isChecked()) {
        test = !StartTrackingValue->text().isEmpty() &&
                StartTrackingValue->hasAcceptableInput() &&
                !StopTrackingValue->text().isEmpty() &&
                StopTrackingValue->hasAcceptableInput();
        if (LimitingMetric->currentIndex() == 1)
            test = test && !filePath->text().isEmpty() && (fileType != NULL);
    }
    else
        test = true;
    if (test) {
        const double PI         = 3.1415926535;
        tp.tractsPerSeed        = TractsPerSeed->value();
        tp.maxBendingAngle      = qCos(MaxBendAngle->value()*PI/180);
        tp.propStep             = StepSize->value();
        tp.smoothingQual        = SmoothQual->value();
        tp.limitingMetricStart  = StartTrackingValue->text().toFloat();
        tp.limitingMetricStop   = StopTrackingValue->text().toFloat();
        tp.minFiberLength       = MinLength->value();
        tp.smooth               = SmoothFibers->isChecked();
        tp.useLimitingMetric    = MetricBox->isChecked();
        accept();
    }
    else {
        QMessageBox::warning(this, "Warning!",
                             "Please fill all boxes. Note that FACT algorithm must uses FA as a "
                             "metric which varies from 0.0 to 1.0.");
    }
}

void TrackFibersDialog::metricBoxToggled()
{
    if (TrackingMethodBox->currentIndex() == 0)
        MetricBox->setChecked(true);
}
