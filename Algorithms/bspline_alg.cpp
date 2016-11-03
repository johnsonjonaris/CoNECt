#include "bspline_alg.h"

QVector<QVector3D> BSPLINE::bSplineDeBoor(const QVector<QVector3D> &ctrlPoints, uint qual)
{
    if (ctrlPoints.size() < 3)
        return ctrlPoints;
    // always use quadratic poly
    int crvDeg = 2, knotSize, nOpPoints, nCtrlPts;
    nOpPoints = qual*ctrlPoints.size();
    nCtrlPts = ctrlPoints.size();
    // generate knot vector
    knotSize = crvDeg + ctrlPoints.size();
    ucolvec knotVec = Col<u32>(knotSize+1);
    for(int c = 0; c < crvDeg+1; c++)
        knotVec(c) = 0;
    for (int c = crvDeg+1; c < knotSize-crvDeg;c++)
        knotVec(c) = c;
    for (int c = knotSize-crvDeg; c <= knotSize;c++)
        knotVec(c) = knotSize;
    // generate U vector
    fcolvec U = linspace<fcolvec>(0,knotSize,nOpPoints);
    // get Intervals of U: t(i) <= U < t(i+1),
    // we reply on the fact that both vectors are sorted
    ucolvec I = Col<u32>(nOpPoints);
    int c = crvDeg+1;
    bool ok;
    for (int i = 0; i< nOpPoints; i++) {
        ok = (U(i) <= knotVec(c));
        while (!ok) {
            c++;
            ok = (U(i) < knotVec(c));
        }
        I(i) = c;
    }
    // prepare output points
    QVector<QVector3D> OpPoints;
    OpPoints.reserve(nOpPoints);

    // begin deBoor algorithm
    for (int i = 0; i<nOpPoints;i++)
        OpPoints.append(deBoor(crvDeg, crvDeg, I(i)-1, U(i), knotVec, ctrlPoints));
    return OpPoints;
}

// this De Boor's Algorithm, implemented in a recursive manner
QVector3D BSPLINE::deBoor(int k,
                          int degree,
                          int i,
                          double u,
                          const ucolvec &knots,
                          const QVector<QVector3D> &ctrlPoints
                          )
{
    if( k == 0)
        return ctrlPoints[i];
    else {
        double alpha = (u-knots[i])/(knots[i+degree-k+2]-knots[i]);
        return deBoor(k-1,degree, i-1, u, knots, ctrlPoints)*(1-alpha)
                +deBoor(k-1,degree, i, u, knots, ctrlPoints)*alpha ;
    }
}
