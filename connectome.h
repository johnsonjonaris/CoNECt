#ifndef CONNECTOME_H
#define CONNECTOME_H

#include <QtCore>
#include <QTextStream>
#include "fibertracts.h"
#include "misc_functions.h"
#include "myprogressdialog.h"
#include "mriimage.h"
#include "armadillo"

using namespace arma;

class CvtkWidget;
class Connectome
{

    struct Header{
        char fileTag[4];            // file tag, should be Cnct
        quint32 nRegions;           // number of labels
        float dx,dy,dz;             // label coordinates
        int nR,nC,nS;               // mask dimensions
    }header;
    mat NW;                         // the connectome network
    fmat centroids;                 // the labels centroids
    QStringList names;              // the labels names
    uchar_cube maskImage;           // a mask cube
    // metrics
    mat localMetrics;               // local metrics = nMetrics X nNodes
    vec globalMetrics;              // global metrics
    mat pl;                         // path length
    mat edgeBetweeness;             // edge betweeness centrality

    bool metricsExist;

    friend class CvtkWidget;


public:

    Connectome();

    // read, write operations
    bool read(const QString &fileName, bool quiet = false);
    bool write(const QString &fileName, bool quiet = false);
    static int readLabel (const QString &fileName, s16_cube &label,
                          AnalyzeHeader &hdr, ImageFileType fileType,
                          MyProgressDialog *progress);
    bool writeMetrics(const QString &fileName, bool quiet = false);

    void setNamesToFreesurfer() {readNames(QDir::currentPath()+"/FreesurferLabels.txt");}

    // access stuff
    bool generate(s16_cube ,const FiberTracts &,bool,MyProgressDialog *);
    inline void setVoxelDim(float dim[3])
    { header.dx = dim[0]; header.dy = dim[1]; header.dz = dim[2]; }
    bool readNames(const QString &fileName);
    void setNames(const QStringList &n) { names = n; }
    void setMask(const uchar_cube &m) {maskImage = m; }

    mat getNW() const { return NW; }
    fmat getCentroids() const { return centroids; }
    QStringList getNames() const { return names; }
    uchar_cube getMask() const { return maskImage; }
    mat getLocalMetrics() const { return localMetrics; }
    vec getGlobalMetrics() const { return globalMetrics; }
    mat getPathLength() const { return pl; }
    mat getEdgeBetweeness() const { return edgeBetweeness; }
    Header getHeader() const { return header; }
    inline bool haveMetrics() const {return metricsExist; }

    // test
    inline bool isEmpty() const { return NW.is_empty(); }
    void clear();

    void computeMetrics();

    // compute metrics for any network. All functions are derived
    // from the BCT toolbox: https://sites.google.com/site/bctnet/
    // version dated: 25 December 2013
    // topological
    static rowvec strength(const mat &W);
    static double density(const mat &W);
    static rowvec degree(const mat &W);
    // clustering and segregation
    static rowvec betweenessCentrality(const mat &G, mat &EBC);
    static rowvec clusteringCoeff(const mat &W);
    static double transitivity(const mat &W);
    static urowvec modularity_louvain(mat W,double *Qopt,double gamma=1.0);
    static rowvec participationCoefficient(const mat &W, const urowvec &Ci);
    static rowvec moduleZscore(const mat &W, const urowvec &Ci);
    // functional integration
    static mat pathLength(const mat &G);
    static rowvec localEfficiency(const mat &W);
    static double globalEfficiency(const mat &W);
    static rowvec vulnerability(const mat &W);
    // core structure
    static double assortativity(const mat &W);
    static rowvec richClub(const mat &W,int klevel);

    // metrics, overload functions for convinience
    // topological
    inline void strength()
    {
        localMetrics.row(0) = strength(NW);
        globalMetrics(0) = mean(localMetrics.row(0));
    }
    inline void density() { globalMetrics(7) = density(NW); }
    inline void degree()
    {
        localMetrics.row(1) = degree(NW);
        globalMetrics(1) = mean(localMetrics.row(1));
    }
    // clustering and segregation
    inline void betweenessCentrality() { localMetrics.row(6) =
                betweenessCentrality(1/NW,edgeBetweeness); }
    inline void clusteringCoeff()
    {
        localMetrics.row(5) = clusteringCoeff(NW);
        globalMetrics(2) = mean(localMetrics.row(5));
    }
    inline void transitivity() { globalMetrics(6) = transitivity(NW); }
    inline void modularity_louvain(double gamma=1.0)
    { localMetrics.row(9) = conv_to<rowvec>::from(modularity_louvain(NW,&globalMetrics(3),gamma)); }
    inline void participationCoefficient(const urowvec &Ci)
    { localMetrics.row(7) = participationCoefficient(NW,Ci); }
    inline void moduleZscore(const urowvec &Ci) { localMetrics.row(8) = moduleZscore(NW,Ci); }
    // functional integration
    inline void pathLength() {
        uint n = NW.n_rows;
        pl = pathLength(1/NW);
        pl(find(pl == datum::inf)).fill(0);
        localMetrics.row(2) = sum(pl,0)/n;
        globalMetrics(4) = accu(pl)/(n*n-n);
        mat invPL = 1/pl;
        invPL(find(invPL == datum::inf)).fill(0);
        globalMetrics(5) = accu(invPL)/(n*n-n);
    }
    inline void localEfficiency() { localMetrics.row(3) = localEfficiency(NW); }
    inline void globalEfficiency() { globalMetrics(5) = globalEfficiency(NW); }
    inline void vulnerability() { localMetrics.row(4) = vulnerability(NW); }
    // core structure
    inline void assortativity() { globalMetrics(8) = assortativity(NW); }
    inline void richClub(int klevel) { localMetrics.row(10) = richClub(NW,klevel); }
};

#endif // CONNECTOME_H
