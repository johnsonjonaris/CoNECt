#include "view.h"


View::View(Type type, QWidget *parent) :
    View(type, new QGraphicsScene(parent), parent)
{}

View::View(Type type, QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(scene, parent)
  , verticalLine(nullptr), horizontalLine(nullptr), slice(nullptr)
  , sliceHeight(1), sliceWidth(1)
  , crosshairX(0), crosshairY(0), type(type)
  , aspectRatio(1.0), currentScale(1.0), minScale(1.0), minPixSize(200)
{
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setBackgroundBrush(palette().color(QPalette::Normal, QPalette::Window));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
    switch (type) {
    case Type::CORONAL:
        verticalColor = Qt::green;
        horizontalColor = Qt::blue;
        setStyleSheet("border: 2px solid red");
        break;
    case Type::SAGITTAL:
        verticalColor = Qt::red;
        horizontalColor = Qt::blue;
        setStyleSheet("border: 2px solid green");
        break;
    case Type::AXIAL:
        verticalColor = Qt::red;
        horizontalColor = Qt::green;
        setStyleSheet("border: 2px solid blue");
        break;
    default:
        break;
    }
}

void View::clear()
{
    QGraphicsScene *scen = scene();
    if (scen) {
        scen->clear();
        verticalLine = nullptr;
        horizontalLine = nullptr;
        slice = nullptr;
    }
}

void View::zoom(int level)
{
    // don't allow scale less than a specific value
    if ((level < 0) && (currentScale < minScale))
        return;
    double factor = qPow(1.05, level);
    currentScale *= factor;
    scale(factor, factor);
}

void View::wheelEvent(QWheelEvent *ev)
{
    int numSteps = ev->delta()/120;
    zoom(numSteps);
    emit wheelScrolled( ev );
}

void View::resetSize()
{
    /*
      let scene size be X1,Y1, viewport size be X2,Y2,
      we want the factor f to be < X2/X1 and Y2/(Y1*|r|)
      since we will scale X1 by f and Y1 by f*r
      */
    if (!this->isEnabled())
        return;
    double margin = 0.95;
    double widthRatio = double(width())/double(sliceWidth);
    double heightRatio = double(height())/double(sliceHeight);
    double factor = qMax(qMin(widthRatio, heightRatio/abs(aspectRatio))*margin,
                         minScale);
    QMatrix matrix;
    matrix.scale(factor,-factor*aspectRatio);
    setMatrix(matrix);
    currentScale = factor;
}

void View::setCrosshairLocation(int x, int y)
{
    QGraphicsScene *scen = scene();
    if (scen == nullptr)
        return;
    if (verticalLine) {
        scen->removeItem(verticalLine);
        delete verticalLine;
        verticalLine = nullptr;
    }
    if (horizontalLine) {
        scen->removeItem(horizontalLine);
        delete horizontalLine;
        horizontalLine = nullptr;
    }

    QPen pen = QPen(Qt::DashLine);
    pen.setWidthF(0.6);
    pen.setColor(horizontalColor);
    horizontalLine = scen->addLine(QLineF(0,y+0.5,sliceWidth,y+0.5),pen);
    horizontalLine->setZValue(1);
    pen.setColor(verticalColor);
    verticalLine = scen->addLine(QLineF(x+0.5,0,x+0.5,sliceHeight),pen);
    verticalLine->setZValue(1);
    crosshairX = x;
    crosshairY = y;
}

void View::updateSlice(const QPixmap& image)
{
    QGraphicsScene *scen = scene();
    if (scen == nullptr)
        return;
    if (slice) {
        scen->removeItem(slice);
        delete slice;
        slice = nullptr;
    }
    slice = scen->addPixmap(image);
    slice->setZValue(0);
    sliceHeight = image.height();
    sliceWidth = image.width();
    minScale = qMax(minPixSize/double(sliceWidth),
                            minPixSize/(double(sliceHeight*abs(aspectRatio))));
    setSceneRect(0,0,sliceWidth,sliceHeight);
}

void View::showContextMenu(const QPoint &pos)
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
    // delete the menu will delete the actions
    delete myMenu;
}

void View::setAspectRatio(double r)
{
    aspectRatio = r;
    minScale = qMax(minPixSize/double(sliceWidth),
                    minPixSize/(double(sliceHeight*abs(aspectRatio))));
}
