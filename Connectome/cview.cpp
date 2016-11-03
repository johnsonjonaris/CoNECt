#include "cview.h"

CView::CView(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setBackgroundBrush(palette().color(QPalette::Normal, QPalette::Window));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));
}

void CView::wheelEvent(QWheelEvent *ev)
{
    int numSteps = ev->delta()/120;
    double factor = qPow(1.125, numSteps);
    scale(factor, factor);
    emit wheelScrolled( ev );
}

void CView::resetSize()
{
    /*
      let scene size be X1,Y1, viewport size be X2,Y2,
      we want the factor f to be < X2/X1 and Y2/(Y1)
      since we will scale X1 by f and Y1 by f
      */
    if (!this->isEnabled())
        return;
    double widthRatio, heightRatio, factor;
    widthRatio = double(this->width())/double(this->sceneRect().width());
    heightRatio = double(this->height())/double(this->sceneRect().height());
    factor = qMin(widthRatio,heightRatio)*0.95;
    QMatrix matrix;
    matrix.scale(factor,factor);
    this->setMatrix(matrix);
}

void CView::showContextMenu(const QPoint &pos)
{
    QMenu *myMenu = new QMenu;
    QAction *saveAs, *copy;
    saveAs = myMenu->addAction("Save Image As");
    copy = myMenu->addAction("Copy Image");

    QAction* selectedItem = myMenu->exec(this->mapToGlobal(pos));

    if (selectedItem != NULL) {
        QPixmap img = QPixmap::grabWidget(this->viewport(),
                                          this->mapFromScene(this->sceneRect().toRect()).boundingRect());
        if (selectedItem == copy)
            QApplication::clipboard()->setPixmap(img, QClipboard::Clipboard);
        else {
             // writes pixmap in PNG format
            QString fileName = QFileDialog::getSaveFileName(this,"Save Image", ".", "(*.png)");
            if (fileName.isEmpty()) //check for cancel press
                return;
            img.save(fileName, "PNG");
        }
    }
    // deleting the menu will delete the actions
    delete myMenu;
}
