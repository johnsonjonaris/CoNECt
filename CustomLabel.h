#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QtGui>
#include <QLabel>
#include <QWidget>

class CustomLabel : public QLabel
{
    Q_OBJECT
signals:
    void mouseMoved( QMouseEvent *ev );
    void mousePress( QMouseEvent *ev );
    void keyPressed( QKeyEvent *ev );
    void mouseReleased( QMouseEvent *ev );
    void wheelScrolled( QWheelEvent *ev );
    void paintEv( QPaintEvent *ev );

public:
    CustomLabel( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    CustomLabel( const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0 );

protected:
    void enterEvent(QEvent *ev) {setCursor(Qt::CrossCursor);}
    void mouseMoveEvent( QMouseEvent *ev ) { emit mouseMoved( ev ); }
    void keyPressEvent ( QKeyEvent *ev ) { emit keyPressed( ev ); }
    void mousePressEvent ( QMouseEvent *ev ) { emit mousePress( ev ); }
    void mouseReleaseEvent(QMouseEvent *ev) { emit mouseReleased( ev ); }
    void wheelEvent( QWheelEvent *ev ) {emit wheelScrolled(ev);}
    void paintEvent( QPaintEvent *ev ) {
        QLabel::paintEvent(ev); emit paintEv( ev );
    }

};
#endif // CUSTOMLABEL_H
