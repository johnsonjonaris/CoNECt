#ifndef BSPLINE_ALG_H
#define BSPLINE_ALG_H

#include <QtGui>
#include "armadillo"
#include "datatypes.h"

using namespace arma;

/**
  * \class BSPLINE
  *
  * This class implements de-Boor's recursive algorithm which is a fast and
  * numerically stable algorithm for evaluating spline curves in B-spline form.
  * This implementation is an adaptation to the Matlab implementation:
  * Interactive B-spline drawing by Levente Hunyadi, found at FEX:
  * http://www.mathworks.com/matlabcentral/fileexchange/27374-b-splines
  * For more information, review:
  * https://en.wikipedia.org/wiki/De_Boor's_algorithm
  * The current implementation traverses in reverse order comapred to the
  * wikipedia page.
  */

class BSpline
{
public:
    /**
     * @brief bSplineDeBoor generates a B-Spline curve from a set of control points
     * @param ctrlPoints the input control points
     * @param qual smoothness of the output line
     * @return points of the B-Spline curve
     */
    static Polyline bSplineDeBoor(const Polyline& ctrlPoints, uint qual);
private:
    /**
     * @brief deBoor Evaluate explicit B-spline at specified locations.
     * @param k interval index, 1 =< k <= degree
     * @param degree B-spline order (2 for linear, 3 for quadratic, etc.)
     * @param i l-degree-k =< i <= l
     * @param u value where the B-spline is to be evaluated on the knot vector
     * @param knots knot vector
     * @param ctrlPoints control points
     * @return evaluated point in the B-Spline curve
     */
    inline static QVector3D deBoor(int k, int degree, int i, double u,
                                   const ucolvec &knots, const Polyline &ctrlPoints);
};
#endif // BSPLINE_ALG_H
