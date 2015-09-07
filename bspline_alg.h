#ifndef BSPLINE_ALG_H
#define BSPLINE_ALG_H

#include <QtCore>
#include <QtGui>
#include "armadillo"
using namespace arma;

/**
  * \class BSPLINE
  *
  * This class implements de Boor's algorithm which is a fast and numerically stable
  * algorithm for evaluating spline curves in B-spline form. The algorithm is recursive.
  * This implementation is an adaptation to the Matlab implementation:
  * Interactive B-spline drawing by Levente Hunyadi, found at FEX:
  * http://www.mathworks.com/matlabcentral/fileexchange/27374-b-splines
  * For more information, review:
  * https://en.wikipedia.org/wiki/De_Boor's_algorithm
  * The current implementation traverses in reverse order comapred to the wikipedia page.
  */
class BSPLINE
{
public:
    // static since we are not creating a BSPLINE object
    static QVector<QVector3D> bSplineDeBoor(const QVector<QVector3D>& ctrlPoints, uint qual);
private:
    // inline for speed
    inline static QVector3D deBoor(int k,
                                   int degree,
                                   int i,
                                   double u,
                                   const ucolvec& knots,
                                   const QVector<QVector3D> &ctrlPoints);
};
#endif // BSPLINE_ALG_H
