#ifndef GEOMETRYFUNCTIONS_H
#define GEOMETRYFUNCTIONS_H

#include <QVector3D>
#define SMALL_NUM   0.00000001 // anything that avoids division overflow

// intersect3D_SegmentPlane(): find the 3D intersection of a segment and a plane
// check http://geomalgorithms.com/a05-_intersect-1.html
//    Input:  a segment: p1 and p2,
//            a plane = {Point planeP;  Vector planeN;}
//    Output: I = the intersect point (when it exists)
//    Return: 0 = disjoint (no intersection)
//            1 = intersection in the unique point *I0
//            2 = the  segment lies in the plane

inline static bool intersect3D_SegmentPlane( const QVector3D &p1, const QVector3D &p2,
                                             const QVector3D &planeN, const QVector3D &planeP,
                                             QVector3D &I )
{
    QVector3D u = p2 - p1;                  // Vector    u = S.P1 - S.P0;
    QVector3D w = planeP - p1;              // Vector    w = S.P0 - Pn.V0;

    float     D = QVector3D::dotProduct(planeN,u);  // dot(Pn.n, u);
    float     N = QVector3D::dotProduct(planeN,w);  // -dot(Pn.n, w);

    if (fabs(D) < SMALL_NUM) {              // segment is parallel to plane
        if (N == 0) {                       // segment lies in plane
            I = (p1+p2)/2;
            return true;
        }
        else
            return false;                   // no intersection
    }
    // they are not parallel
    // compute intersect param
    float sI = N / D;
    if (sI < 0 || sI > 1)
        return false;                       // no intersection

    I = p1 + sI*u;                          // compute segment intersect point
    return true;
}
//===================================================================

#endif // GEOMETRYFUNCTIONS_H
