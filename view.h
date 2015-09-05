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

class View : public QGraphicsView
{
    Q_OBJECT

signals:
    void mouseMoved( QMouseEvent *ev );
    void mousePressed( QMouseEvent *ev );
    void keyPressed( QKeyEvent *ev );
    void mouseReleased( QMouseEvent *ev );
    void wheelScrolled( QWheelEvent *ev );
    void paintEv( QPaintEvent *ev );

public:
    View(QWidget *parent = 0);
    View(QGraphicsScene * scene, QWidget * parent = 0);
    void resetSize(float r);
//    void updateSlice();

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

};

#endif
