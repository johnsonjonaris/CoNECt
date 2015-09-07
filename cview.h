#ifndef CVIEW_H
#define CVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMenu>
#include <QtGui>

/**
  * \class CView
  *
  * This class houses graphic items. It captures mouse move, release and scrolling.
  * It signals those events by emitting signals. It handles the shape of the cursor.
  * It also implement a specific contex menu on the right click to allow the user
  * to copy\save the contained view.
  */
class CView : public QGraphicsView
{
    Q_OBJECT

signals:
    void mouseMoved( QMouseEvent *ev );
    void mouseReleased( QMouseEvent *ev );
    void wheelScrolled( QWheelEvent *ev );

public:
    CView(QWidget *parent = 0);
    void resetSize();

protected slots:
    /// show context menu at a right click on the view
    void showContextMenu(const QPoint& pos);

protected:
    void enterEvent(QEvent *ev) {
        Q_UNUSED(ev);
        this->viewport()->setCursor(Qt::ArrowCursor);
    }
    void mouseMoveEvent( QMouseEvent *ev ) {
        emit mouseMoved( ev );
        QGraphicsView::mouseMoveEvent(ev);
    }
    void mouseReleaseEvent(QMouseEvent *ev) {
        emit mouseReleased( ev );
        QGraphicsView::mouseReleaseEvent(ev);
        this->viewport()->setCursor(Qt::ArrowCursor);
    }
    void wheelEvent( QWheelEvent *ev );
};


#endif // CVIEW_H
