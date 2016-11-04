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
        VECTOR
    };

    View(View::Type type, QWidget *parent = 0);
    View(View::Type type, QGraphicsScene * scene, QWidget * parent = 0);

    void resetSize(float r);

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
    QGraphicsLineItem   *verticalLine, *horizontalLine;
    Type type;                              ///< type of the view
    QColor  verticalColor,                  ///< color of the vertical crosshair
            horizontalColor;                ///< color of the horizontal crosshair
    int verticalLimit,                      ///< the highest Y value of the displayed slice
        horizontalLimit;                    ///< the highest X value of the displayed slice
    int crosshairX,                         ///< current X value of the crosshair
        crosshairY;                         ///< current Y value of the crosshair
};

#endif
