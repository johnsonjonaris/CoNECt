#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QApplication>
#include <QFileDialog>
#include <QClipboard>
#include <QMenu>
#include <QtGui>

#include "armadillo"

using namespace std;

/**
  * \class View
  *
  * Custom QGraphicsView that captures mouse in\out motion, press, scroll
  * as well as keyboard presses and emits the appropriate signal.
  * The class accepts a QImage slice to be drawn and a location for a crosshair.
  * A min pixel size limits the zoom out, it is defaulted to 200 pixel of the
  * smallest side of the visualized image.
  * The class also defines 5 Types for the visualized image: a CORONAL, SAGITTAL
  * or AXIAL anatomical cut; a VECTOR color image or a more GENERAL image which
  * can be any image.
  * An appropriate frame color will be drawn according to the chosen Type.
  *
  */
class View : public QGraphicsView
{
    Q_OBJECT

public:

    enum class Type {
        CORONAL,
        SAGITTAL,
        AXIAL,
        VECTOR,
        GENERAL
    };

    View(View::Type type, QWidget *parent = 0);
    View(View::Type type, QGraphicsScene * scene, QWidget * parent = 0);

    /**
     * @brief resetSize the method rescales the visualized image to fit the View.
     */
    void resetSize();

    /**
     * @brief updateSlice updating the current image
     * @param image Image to be displayed
     */
    void updateSlice(const QPixmap& image);

    /**
     * @brief setCrosshairLocation updating the current crosshair
     * @param x X location of the crosshair
     * @param y Y location of the crosshair
     */
    void setCrosshairLocation(int x, int y);

    /**
     * @brief clear clear all the contents of the View
     */
    void clear();

    /**
     * @brief setAspectRatio sets the aspect ratio of the image X/Y
     * @param r aspect ratio
     */
    void setAspectRatio(double r);

    /**
     * @brief zoom zoom the scene by this value
     * @param level if positive: zoom in, if negative zoom out
     */
    void zoom(int level);

    /**
     * @brief setMinPixSize set the min pixel size of the slice
     * @param value
     */
    void setMinPixSize(int value) { minPixSize = value; }

signals:

    void mouseMoved( QMouseEvent *ev );
    void mousePressed( QMouseEvent *ev );
    void keyPressed( QKeyEvent *ev );
    void mouseReleased( QMouseEvent *ev );
    void wheelScrolled( QWheelEvent *ev );
    void paintEv( QPaintEvent *ev );

protected slots:
    void showContextMenu(const QPoint& pos);

protected:
    void enterEvent(QEvent *ev) {
        Q_UNUSED(ev);
        this->viewport()->setCursor(Qt::CrossCursor);
    }
    void keyPressEvent ( QKeyEvent *ev ) {
        emit keyPressed( ev );
        QGraphicsView::keyPressEvent(ev);
    }
    void mouseMoveEvent( QMouseEvent *ev ) {
        emit mouseMoved( ev );
        QGraphicsView::mouseMoveEvent(ev);
    }
    void mousePressEvent ( QMouseEvent *ev ) {
        emit mousePressed( ev );
        QGraphicsView::mousePressEvent(ev);
    }
    void mouseReleaseEvent(QMouseEvent *ev) {
        emit mouseReleased( ev );
        QGraphicsView::mouseReleaseEvent(ev);
        this->viewport()->setCursor(Qt::CrossCursor);
    }
    void wheelEvent( QWheelEvent *ev );
    void paintEvent( QPaintEvent *ev ) {
        QGraphicsView::paintEvent(ev);
        emit paintEv( ev );
    }

private:

    QGraphicsPixmapItem *slice;
    QGraphicsLineItem   *verticalLine, *horizontalLine;
    Type type;                              ///< type of the view
    QColor  verticalColor,                  ///< color of the vertical crosshair
            horizontalColor;                ///< color of the horizontal crosshair
    int sliceHeight,                        ///< the highest Y value of the displayed slice
        sliceWidth;                         ///< the highest X value of the displayed slice
    int crosshairX,                         ///< current X value of the crosshair
        crosshairY;                         ///< current Y value of the crosshair
    double aspectRatio;                     ///< aspect ratio X/Y of the input image
    double currentScale;                    ///< current scale value
    double minScale;                        ///< min scale value
    quint16 minPixSize;                     ///< min pix size of an image
};

#endif
