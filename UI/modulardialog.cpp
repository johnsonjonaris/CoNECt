#include "modulardialog.h"


ModularDialog::ModularDialog(const QVector<QColor> &cList, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    // create a dynamic number of color change options
    cListOut = QVector<QColor>(cList);
    uint n = cListOut.size();
    for (uint i=0;i<n;++i) {
        QHBoxLayout *horizontalLayout = new QHBoxLayout();
        QLabel *label = new QLabel("Module "+QString::number(i+1)+": ",
                                   scrollAreaWidgetContents);

        horizontalLayout->addWidget(label);

        QPushButton2 *pushButton = new QPushButton2(QString::number(i),scrollAreaWidgetContents);
        pushButton->setFixedSize(QSize(50, 25));
        QPalette pal;
        pal.setColor(QPalette::Active,QPalette::Button,cListOut.at(i));
        pushButton->setPalette(pal);

        horizontalLayout->addWidget(pushButton);
        connect(pushButton,SIGNAL(clicked(QPushButton2*)),
                this,SLOT(onModuleButtonPress(QPushButton2*)));

        verticalLayout->addLayout(horizontalLayout);
    }
    this->setFixedSize(this->sizeHint());
}

void ModularDialog::onModuleButtonPress(QPushButton2 *pb)
{
    if (pb == NULL)
        return;
    QColor c = QColorDialog::getColor(pb->palette().color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    pb->setPalette(pal);
    // update the list
    cListOut.replace(pb->text().toUInt(),c);
}
