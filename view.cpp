#include "view.h"


View::View(QWidget *parent) : QGraphicsView(parent) {
    this->setScene(new QGraphicsScene(parent));
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setBackgroundBrush(palette().color(QPalette::Normal, QPalette::Window));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
}

View::View(QGraphicsScene * scene, QWidget * parent) : QGraphicsView(scene, parent) {
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setBackgroundBrush(palette().color(QPalette::Normal, QPalette::Window));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
}

void View::wheelEvent(QWheelEvent *ev)
{
    int numSteps = ev->delta()/120;
    double factor = qPow(1.125, numSteps);
    scale(factor, factor);
    emit wheelScrolled( ev );
}

void View::resetSize(float r)
{
    /*
      let scene size be X1,Y1, viewport size be X2,Y2,
      we want the factor f to be < X2/X1 and Y2/(Y1*|r|)
      since we will scale X1 by f and Y1 by f*r
      */
    if (!this->isEnabled())
        return;
    double widthRatio, heightRatio, factor;
    widthRatio = double(this->width())/double(this->sceneRect().width());
    heightRatio = double(this->height())/double(this->sceneRect().height());
    factor = qMin(widthRatio,heightRatio/abs(r))*0.95;
    QMatrix matrix;
    matrix.scale(factor,-factor*r);
    this->setMatrix(matrix);
}

void View::showContextMenu(const QPoint &pos)
{
    QMenu *myMenu = new QMenu;
    QAction *saveAs, *Copy;
    saveAs = myMenu->addAction("Save Image As");
    Copy = myMenu->addAction("Copy Image");

    QAction* selectedItem = myMenu->exec(this->mapToGlobal(pos));

    if (selectedItem != NULL) {
        QPixmap img = QPixmap::grabWidget(this->viewport(),
                                             this->mapFromScene(this->sceneRect().toRect()).boundingRect());
        if (selectedItem == Copy)
            QApplication::clipboard()->setPixmap(img, QClipboard::Clipboard);
        else {
             // writes pixmap in PNG format
            QString fileName = QFileDialog::getSaveFileName(this,"Save Image", ".", "(*.png)");
            if (fileName.isEmpty()) //check for cancel press
                return;
            img.save(fileName, "PNG");
        }
    }
    // delete the menu will delete the actions
    delete myMenu;
}

//void View::updateSlice(const mat &activeVol,const QVector<QRgb> &cTable,
//                       bool isGrayscale,double minActVol,double maxActVol,
//                       int slice)
//{
//    QImage tmp;
//    if (isGrayscale) {
//        mat a = 255.0*((activeVol(span(slice),span::all,span::all)
//                        -minActVol)/(maxActVol-minActVol));
//        uchar_mat b = conv_to<uchar_mat>::from(a);
//        tmp = QImage(b.memptr(),b.n_rows,b.n_cols,b.n_rows,QImage::Format_Indexed8);
//        tmp.setColorTable(cTable);
//    }
//    else {
//        tmp = QImage(nColsImg, nSlicesImg,QImage::Format_ARGB32);
//        for (uint i=0; i<nSlicesImg;i++)
//            for (uint j=0; j<nColsImg;j++)
//                tmp.setPixel(j,i,cTable.at( int(activeVol(slice,j,i)) ));
//    }
////    QPen pen = QPen(Qt::DashLine);
////    pen.setWidthF(0.2);
//    this->scene()->clear();
//    this->scene()->addPixmap(QPixmap::fromImage(tmp));
////    pen.setColor(Qt::blue);
////    this->scene()->addLine(QLineF(0,selAxSlice+0.5,nColsImg,selAxSlice+0.5),pen);
////    pen.setColor(Qt::green);
////    this->scene()->addLine(QLineF(selSagSlice+0.5,0,selSagSlice+0.5,nSlicesImg),pen);
//    this->setSceneRect(0,0,tmp.width(),tmp.height());
//}
