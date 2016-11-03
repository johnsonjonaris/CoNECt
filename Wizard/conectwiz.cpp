#include "conectwiz.h"


CoNECtWizard::CoNECtWizard(QWidget *parent) : QWizard(parent)
{
    setDefaultProperty("QGroupBox","checked","toggled");
    setDefaultProperty("QDoubleSpinBox","value","valueChanged");

    setPage(PAGE_INTRO, new IntroPage);
    setPage(PAGE_1_DW, new Page1DW);
    setPage(PAGE_2_DW, new Page2DW);
    setPage(PAGE_1_BNA,new Page1BNA);

    setStartId(PAGE_INTRO);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
    setWindowTitle("Data Analysis Wizard");
}

void CoNECtWizard::getTractographyParameters(TrackingParameters *tp)
{
    const double PI          = 3.1415926535;
    tp->tractsPerSeed        = this->field("page2dw.tractPerVoxel").toUInt();
    tp->maxBendingAngle      = qCos(this->field("page2dw.maxBendingAngle").toUInt()*PI/180);
    tp->propStep             = this->field("page2dw.stepSize").toFloat();
    tp->smooth               = this->field("page2dw.smoothFibers").toBool();
    tp->smoothingQual        = this->field("page2dw.smoothQuality").toUInt();
    tp->limitingMetricStart  = this->field("page2dw.startFA").toFloat();
    tp->limitingMetricStop   = this->field("page2dw.stopFA").toFloat();
    tp->minFiberLength       = this->field("page2dw.minFiber").toFloat();
    DiffusionModelType model = static_cast<DiffusionModelType>(this->field("page1dw.imagingModel").
                                                               toUInt());
    tp->useLimitingMetric    = (model == DTI);
}

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("New Study Wizard");
    setPixmap(QWizard::WatermarkPixmap, QPixmap("/images/wiz.png"));
    registerField("intro.dw",DwRadio);
    registerField("intro.singleSubject",SingleSubjectRadio);
}



