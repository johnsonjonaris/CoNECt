#ifndef MISC_FUNCTIONS_H
#define MISC_FUNCTIONS_H


#include <QVector>
#include <QColor>
#include <QTableWidget>
#include <QApplication>
#include <QClipboard>
#include "armadillo"

using namespace arma;

// FOR AN UNKOWN REASON, A COMPILER ERROR HAPPENS
// IF THIS FUNCTION IS PUT IN THE CPP FILE
static QVector<QRgb> getColorTable(float beta)
{
    // this functin creates a black and white color table
    // then controls its brightness.
    // beta should vary from -1 to 1

    QVector<QRgb> cTable;
    cTable.reserve(256);

    if (beta == 0.0) {
        for ( uint i = 0; i < 256; i++ )
            cTable.append(qRgb(i,i,i));
        return cTable;
    }

    float   gamma = 1.0,
            tol = 1.0 - 1e-10;
    if (beta > 0)
        gamma = 1 - qMin(tol,beta);
    else if (beta < 0)
        gamma = 1/(1.0 + qMax(-tol,beta));

    // construct a normal colormap
    vec c = linspace<vec>(0,1,256);
    c =  round(255*arma::pow(c,gamma));
    for ( uint i = 0; i < 256; i++ )
        cTable.append(qRgb( c(i), c(i), c(i) ));
    return cTable;
}

void onCopyKeysPressed(QTableWidget *table);

uvec matlabUnique(const uvec &v);

umat getIndex(const uvec &idx, int r, int c=-1, int s=-1);

#endif // MISC_FUNCTIONS_H
