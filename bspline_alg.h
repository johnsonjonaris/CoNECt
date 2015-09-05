#ifndef BSPLINE_ALG_H
#define BSPLINE_ALG_H

#include <QtCore>
#include <QtGui>
#include "armadillo"
using namespace arma;

class BSPLINE
{
public:
    // static since we are not creating a BSPLINE object
    static QVector<QVector3D> bSplineDeBoor(const QVector<QVector3D>& ctrlPoints, uint qual);
private:
    // inline for speed
    inline static QVector3D deBoor(int k,int degree, int i, double u,
                     const ucolvec& knots, const QVector<QVector3D> &ctrlPoints);
};
#endif // BSPLINE_ALG_H
