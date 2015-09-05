#include "connectomeview.h"

ConnectomeView::ConnectomeView(QWidget *parent) : QWidget(parent) {

    setupUi(this);
    View = new CView(frame);
    verticalLayout_2->addWidget(View);
    connect(View,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(viewMouseMoved(QMouseEvent*)));
    connect(View,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(viewMouseReleased(QMouseEvent*)));
    connect(BrightnessSlider,SIGNAL(valueChanged(int)),
            this,SLOT(onBrightnessSliderMove(int)));
    // initialize selection
    selection = NULL;
    nwPixmapItem = NULL;
}

void ConnectomeView::plotConnectome(const mat &NW)
{
    if (NW.is_empty())
        return;
    this->clear();
    // normalize network and save it for further control
    nwImg.clear();
    nwImg = conv_to<uchar_mat>::from(255.0*(NW-NW.min())/(NW.max()-NW.min()));
    QVector<QRgb> cTable = getColorTable(0.0);
    QImage img = QImage(nwImg.memptr(),nwImg.n_rows,nwImg.n_cols,
                        nwImg.n_rows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    nwPixmapItem = View->scene()->addPixmap(QPixmap::fromImage(img));
    View->setSceneRect(0,0,NW.n_rows,NW.n_cols);
    View->resetSize();
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
        View->scene()->removeItem(selection);
        delete selection;
        selection = NULL;
    }
}

void ConnectomeView::addSelection(int y)
{
    this->removeSelection();
    QPen p;
    p.setColor(Qt::red);
    p.setWidth(0);
    selection = new QGraphicsRectItem();
    selection->setPen(p);
    selection->setRect(0,y,View->sceneRect().width(),1);
    selection->setZValue(3);
    View->scene()->addItem(selection);
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
    View->scene()->addItem(selection);
}

void ConnectomeView::clear()
{
    removeSelection();
    View->scene()->clear();
    nwPixmapItem = NULL;
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
    // network
    // we remove the old image and delete it, note that we
    // were not able to use clear for the scene since this
    // might delete the selection as well, since clear removes
    // items and delete them too.
    if (nwPixmapItem) {
        View->scene()->removeItem(nwPixmapItem);
        delete nwPixmapItem;
    }
    QImage img = QImage(nwImg.memptr(),nwImg.n_rows,nwImg.n_cols,
                        nwImg.n_rows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    nwPixmapItem = View->scene()->addPixmap(QPixmap::fromImage(img));
}
