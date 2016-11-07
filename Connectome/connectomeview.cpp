#include "connectomeview.h"

ConnectomeView::ConnectomeView(QWidget *parent) : QWidget(parent) {

    setupUi(this);
    view = new View(View::Type::GENERAL, frame);
    view->setAspectRatio(-1.);
    view->setMinPixSize(400);
    verticalLayout_2->addWidget(view);
    connect(view,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(onMouseMoved(QMouseEvent*)));
    connect(view,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(onMouseReleased(QMouseEvent*)));
    connect(BrightnessSlider,SIGNAL(valueChanged(int)),
            this,SLOT(onBrightnessSliderMove(int)));
    // initialize selection
    selection = NULL;
}

void ConnectomeView::plotConnectome(const mat &NW)
{
    if (NW.is_empty())
        return;
    clear();
    // normalize network and save it for further control
    nwImg.clear();
    nwImg = conv_to<uchar_mat>::from(255.0*(NW-NW.min())/(NW.max()-NW.min()));
    QVector<QRgb> cTable = getColorTable(0.0);
    QImage img = QImage(nwImg.memptr(),nwImg.n_rows,nwImg.n_cols,
                        nwImg.n_rows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    view->updateSlice(QPixmap::fromImage(img));
    view->resetSize();
    // prepare colorbar
    uchar_vec t = linspace<uchar_vec>(255,0,BarLabel->height());
    QImage bar = QImage(t.memptr(),1,t.n_elem,1,QImage::Format_Indexed8);
    bar.setColorTable(cTable);
    BarLabel->setPixmap(QPixmap::fromImage(bar));
    MinLabel->setText(QString::number(NW.min()));
    MaxLabel->setText(QString::number(NW.max()));
}

void ConnectomeView::removeSelection()
{
    if (selection) {
        view->scene()->removeItem(selection);
        delete selection;
        selection = NULL;
    }
}

void ConnectomeView::addSelection(int y)
{
    removeSelection();
    QPen p;
    p.setColor(Qt::red);
    p.setWidth(0);
    selection = new QGraphicsRectItem();
    selection->setPen(p);
    selection->setRect(0,y,view->sceneRect().width(),1);
    selection->setZValue(3);
    view->scene()->addItem(selection);
}

void ConnectomeView::addSelection(int x, int y)
{
    this->removeSelection();
    QPen p;
    p.setColor(Qt::red);
    p.setWidth(0);
    selection = new QGraphicsRectItem();
    selection->setPen(p);
    selection->setRect(x,y,1,1);
    selection->setZValue(3);
    view->scene()->addItem(selection);
}

void ConnectomeView::clear()
{
    removeSelection();
    view->clear();
    MinLabel->clear();
    MaxLabel->clear();
    BarLabel->clear();
    nwImg.clear();
    BrightnessSlider->setValue(50);
}

void ConnectomeView::onBrightnessSliderMove(int value)
{
    // the slider ranges from 0 to 100, normal value is 50
    // going to the right -> toward 100 means brightening
    // going to the left -> toward 0 means darkening
    if (nwImg.is_empty())
        return;
    QVector<QRgb> cTable = getColorTable(value/50.0 - 1.0);
    // color bar
    BarLabel->clear();
    uchar_vec t = linspace<uchar_vec>(255,0,BarLabel->height());
    QImage bar = QImage(t.memptr(),1,t.n_elem,1,QImage::Format_Indexed8);
    bar.setColorTable(cTable);
    BarLabel->setPixmap(QPixmap::fromImage(bar));
    QImage img = QImage(nwImg.memptr(),nwImg.n_rows,nwImg.n_cols,
                        nwImg.n_rows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    view->updateSlice(QPixmap::fromImage(img));
}
