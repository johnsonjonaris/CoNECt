#include "connectome.h"

Connectome::Connectome()
{
    metricsExist = false;
}

void Connectome::clear()
{
    NW.clear();
    centroids.clear();
    names.clear();
    maskImage.clear();
    localMetrics.clear();
    globalMetrics.clear();
    metricsExist = false;
}

bool Connectome::generate(s16_cube label, const FiberTracts &fibers,
                          bool isFreesurfer, MyProgressDialog *progress)
{
    if (label.is_empty() || fibers.isEmpty())
        return false;
    // prepare progress bar
    progress->setLabelText("Generating Connectome ... ");
    progress->setRange(0,fibers.size()+100);
    progress->setFixedSize(progress->sizeHint());
    progress->setModal(true);
    progress->setValue(0);
    progress->show();
    // remove bad labels from label volume if freesurfer
    // in order to eliminate the unwanted values, the label must first
    // be converted into a 1D vector
    if (isFreesurfer) {
        label(find(label<8)).fill(0);
        s16_vec badLabels;
        badLabels<<14<<15<<24<<30<<31<<41<<43<<44<<46<<1000<<1004<<2000<<2004;
        for (int i = 0; i< badLabels.n_elem; ++i)
            label(find(label == badLabels.at(i))).fill(0);
        // remove regions > 60 and < 1000
        uvec loc = find(label > 60);
        label(loc(find(label(loc)<1000))).fill(0);
    }
    s16_vec lbl = s16_vec(label.memptr(),label.n_elem);
    // get labels values and their sizes, it is assumed that hist
    // will give the count of every label since the nbins = n labels
    s16_vec labels = unique(lbl);
    uvec labelsSize = hist(lbl,labels);
    if (labels[0] == 0) {
        labels.shed_row(0);
        labelsSize.shed_row(0);
    }
    int cc = 0;
    s16_vec idx = zeros<s16_vec>(labels.max()+1);
    for (uint i=0; i<labels.n_elem; ++i)
        idx(labels(i)) = cc++;
    progress->setValue(1);
    // compute centroids
    int r = label.n_rows, c = label.n_cols, s = label.n_slices;
    centroids = zeros<fmat>(labels.n_elem,3);
    for (uint i=0;i<labels.size();++i)
        centroids(i,span::all) = mean(conv_to<fmat>::
                                      from(getIndex(find(lbl == labels(i)),r,c,s)),0);

    // fill header
    fibers.getVoxelDimension(header.dx,header.dy,header.dz);
    header.nR = r, header.nC = c, header.nS = s;
    header.nRegions = labels.n_elem;
    // prepare intersection cube
//    frowvec t; t<<header.dx<<header.dy<<header.dz;
//    float cubeSizeLimit = max((cubeSize/2)/t);
//    float endSize = ceil(cubeSizeLimit*2)/2;
//    fvec index = linspace<fvec>(0,cubeSizeLimit,floor(cubeSizeLimit));
//    index.in;
    // form the network
    NW = zeros<mat>(labels.n_elem,labels.n_elem);
    // compute fiber pairs
    int x=0,y=0,z=0, p1, p2;
    for (int i =0; i<fibers.size();++i) {
        // first end
        x = qMax(qMin(qRound(fibers.getFiber(i).first().x()),r),0);
        y = qMax(qMin(qRound(fibers.getFiber(i).first().y()),c),0);
        z = qMax(qMin(qRound(fibers.getFiber(i).first().z()),s),0);
        p1 = label(x,y,z);
        // second end
        x = qMax(qMin(qRound(fibers.getFiber(i).last().x()),r),0);
        y = qMax(qMin(qRound(fibers.getFiber(i).last().y()),c),0);
        z = qMax(qMin(qRound(fibers.getFiber(i).last().z()),s),0);
        p2 = label(x,y,z);
        if (p1 > 0 && p2 > 0) {
            NW(idx(p1),idx(p2))++;
            NW(idx(p2),idx(p1))++;
        }
        progress->setValue(progress->value()+1);
    }
    // remove self connections
    NW.diag().fill(0);
    // normalize the network w.r.t. labels sizes
    for (uint i =0;i<labels.n_elem;++i) {
        for (uint j =i+1;j<labels.n_elem;++j) {
            NW(i,j) = NW(i,j)/(labelsSize(i)+labelsSize(j));
            NW(j,i) = NW(i,j);
        }
    }
    // prepare mask
    label(find(label>0)).fill(1);
    maskImage = conv_to<uchar_cube>::from(label);
    progress->setValue(progress->maximum());
    if (isFreesurfer)
        readNames(QDir::currentPath()+"/FreesurferLabels.txt");
    return true;
}

bool Connectome::read(const QString &fileName, bool quiet)
{
    if (!this->isEmpty())
        clear();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        if (!quiet)
            QMessageBox::critical(0, "Warning!", QString("Cannot read file %1:\n%2.")
                                  .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    qint64 sz = sizeof(header);
    if ( file.read( (char *) &header, sz ) != sz ) {
        file.close();
        return false;
    }
    // test it is a Connectome file
    QString test = QString(header.fileTag);
    test.resize(4);
    if ( test != "Cnct" ) {
        QMessageBox::critical(0, "Warning!", "This is not a Connectome file.");
        file.close();
        return false;
    }
    // move file pointer 64, where data starts
    file.seek(64);
    NW = mat(header.nRegions,header.nRegions);
    sz = sizeof(double)*header.nRegions*header.nRegions;
    if (file.read( (char *) NW.memptr(), sz ) != sz) {
        file.close();
        return false;
    }
    centroids = fmat(header.nRegions,3);
    sz = sizeof(float)*header.nRegions*3;
    if (file.read( (char *) centroids.memptr(), sz ) != sz) {
        file.close();
        return false;
    }
    sz = header.nR*header.nC*header.nS;
    maskImage = uchar_cube(header.nR,header.nC,header.nS);
    if (file.read( (char *) maskImage.memptr(), sz ) != sz) {
        file.close();
        return false;
    }
    // assuming the rest of the file is a ; separated string list
    sz = file.size() - file.pos();
    char *tmp = new char[sz];
    if (file.read( (char *) tmp, sz ) != sz) {
        file.close();
        return false;
    }
    file.close();
    names = QString(tmp).split(";");
    delete tmp;
    if (names.count() != header.nRegions)
        return false;
    computeMetrics();
    return true;
}

bool Connectome::write(const QString &fileName, bool quiet)
{
    if (this->isEmpty())
        return false;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        if (!quiet)
            QMessageBox::critical(0, "Warning!", QString("Cannot write file %1:\n%2.")
                                  .arg(file.fileName()).arg(file.errorString()));
        return false;
    }    
    // write header
    char tmp[] = "Cnct";
    for (int i=0;i<4;++i)
        header.fileTag[i] = tmp[i];
    qint64 sz = sizeof(header);
    if ( file.write( (char *) &header, sz ) != sz ) {
        file.close();
        return false;
    }
    // move file pointer 64, where data starts
    file.seek(64);
    sz = sizeof(double)*NW.n_elem;
    if (file.write( (char *) NW.memptr(), sz ) != sz) {
        file.close();
        return false;
    }
    sz = sizeof(float)*centroids.n_elem;
    if (file.write( (char *) centroids.memptr(), sz ) != sz) {
        file.close();
        return false;
    }
    sz = maskImage.n_elem;
    if (file.write( (char *) maskImage.memptr(), sz ) != sz) {
        file.close();
        return false;
    }
    // labels should be separated by ;
    QString str = "";
    for (int i=0;i<header.nRegions-1;++i)
        str += names.at(i) + ";";
    str += names.last();
    sz = str.size();
    if (file.write( (char *) str.toLocal8Bit().data(), sz ) != sz) {
        file.close();
        return false;
    }
    file.close();
    return true;
}

// get names
bool Connectome::readNames(const QString &fileName)
{
    // this function reads the label names, it assumes they are in the
    // increasing order of the labels values
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    names.clear();
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;
        names.append(line);
    }
    if (!this->isEmpty() && !(names.size() == header.nRegions)) {
        names.clear();
        return false;
    }
    return true;
}

// read a label file
int Connectome::readLabel(const QString &fileName, s16_cube &label,
                          AnalyzeHeader &hdr, ImageFileType fileType,
                          MyProgressDialog *progress)
{
    // this function returns:
    // 0    correct reading
    // 1    error reading image
    // 2    number of volumes > 1
    // 3    non label or unsuported data type
    if (fileName.isEmpty())
        return 1;
    if (fileType == RAW) return 1;
    QString fn = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();
    QString imgFileName = (fileType == ANALYZE)?(fn+".img"):(fn+".nii");
    QString headerFileName = (fileType == ANALYZE)?(fn+".hdr"):(fn+".nii");
    // read data
    // start by reading header data
    if (!readHeader(headerFileName, hdr, fileType))
        return 1;
    // Acquire information from the header
    uint dt = hdr.dime.datatype;
    // file should contain only one image
    uint nVol = hdr.dime.dim[4];
    if (nVol > 1)
        return 2;
    // read data according to its type, label data can not be float or double
    if (dt == DT_UNSIGNED_CHAR) {
        QList< uchar_cube > out = QList< uchar_cube >();
        if (readImage(imgFileName,fileType,false,progress,hdr,out)) {
            label = conv_to<s16_cube>::from(out.first());
            out.clear();
        } else return 1;
    }
    else if (dt == DT_SIGNED_SHORT) {
        QList< s16_cube > out = QList< s16_cube >();
        if (readImage(imgFileName,fileType,false,progress,hdr,out)) {
            label = out.first();
            out.clear();
        } else return 1;
    }
    else if (dt == DT_SIGNED_INT) {
        QList< s32_cube > out = QList< s32_cube >();
        if(readImage(imgFileName,fileType,false,progress,hdr,out)) {
            label = conv_to<s16_cube>::from(out.first());
            out.clear();
        } else return 1;
    }
    else {
        return 3;
    }
    return 0;
}

// topology
rowvec Connectome::strength(const mat &W)
{
    return sum(W,0);
}
double Connectome::density(const mat &W)
{
    /*
    Density is the fraction of present connections to possible connections.

       Input:      W,      undirected (weighted/binary) connection matrix
       Output:     kden,   density
                   N,      number of vertices
                   K,      number of edges
       Notes:  Assumes CIJ is undirected and has no self-connections.
               Weight information is discarded.
    */
    uint N = W.n_rows;
    uvec t = find(trimatu(W)!= 0);
    double K = t.n_elem;
    return K/((N*N-N)/2.0);
}
rowvec Connectome::degree(const mat &W)
{
    return  sum(abs(sign(W)),0);
}


// clustering and segregation
rowvec Connectome::betweenessCentrality(const mat &G, mat &EBC)
{
    /*
   Node betweenness centrality is the fraction of all shortest paths in
   the network that contain a given node. Nodes with high values of
   betweenness centrality participate in a large number of shortest paths.

        Input:      G,      weighted (directed/undirected) connection matrix.
        Output:     BC,     node betweenness centrality vector.
                    EBC,    edge betweenness centrality matrix.

   Notes:
       The input matrix must be a mapping from weight to distance. For
   instance, in a weighted correlation network, higher correlations are
   more naturally interpreted as shorter distances, and the input matrix
   should consequently be some inverse of the connectivity matrix.
       Betweenness centrality may be normalised to [0,1] via BC/[(N-1)(N-2)]

   Reference: Brandes (2001) J Math Sociol 25:163-177.
    */
    uint n = G.n_rows,q = n-1,v=0,w=0;
    double Duw,DPvw;
    vec t;
    rowvec BC = zeros(1,n),D,NP,DP;
    uvec S,Q,V,tt,W;
    mat G1;
    umat P;
    EBC = zeros<mat>(n,n);

    for (uint u = 0; u<n;++u) {
        D = rowvec(1,n).fill(datum::inf); D(u) = 0;
        NP = zeros(1,n); NP(u) = 1;
        S = linspace<uvec>(0,n-1,n);
        P = zeros<umat>(n,n);
        Q = zeros<uvec>(n);
        q = n-1;
        G1 = G;
        V = uvec(1).fill(u);
        while (true) {
            // instead of replacing indices by 0 like S(V)=0;
            // we declare all indices then remove the V indeces
            // from S. Notice that it is assured that tt should
            // be one element since indices don't repeat
            for (int i=0;i<V.n_elem;++i) {
                tt = find(S == V(i),1);
                if (!tt.is_empty())
                    S.shed_row(tt(0));
            }
            G1.cols(V).fill(0);
            for (uint i=0; i<V.n_elem;++i) {
                v = V(i);
                Q(q) = v; --q;
                W = find( G1.row(v) != 0);
                for (uint j = 0;j<W.n_elem;++j) {
                    w = W(j);
                    Duw = D(v)+G1(v,w);
                    if (Duw<D(w)) {
                        D(w) = Duw;
                        NP(w) = NP(v);
                        P.row(w).fill(0);
                        P(w,v) = 1;
                    }
                    else if (Duw == D(w)) {
                        NP(w) += NP(v);
                        P(w,v) = 1;
                    }
                }
            }

            if (S.is_empty())
                break;
            t = D(S);
            if ( !is_finite(t.min()) ){
                // the number of inf elements is assumed to be always = q
                Q.subvec(0,q) = find(D == datum::inf);
                break;
            }
            V = find(D == t.min());
        }
        DP = zeros(1,n);
        for (uint i=0; i<Q.n_elem-1;++i) {
            w = Q(i);
            BC(w) += DP(w);
            tt = find(P.row(w) != 0);
            for (uint j=0; j<tt.n_elem;++j) {
                v = tt(j);
                DPvw = (1+DP(w))*NP(v)/NP(w);
                DP(v) += DPvw;
                EBC(v,w) += DPvw;
            }
        }
    }
    return BC;
}

rowvec Connectome::clusteringCoeff(const mat &W)
{
    /*
    The weighted clustering coefficient is the average "intensity" of
    triangles around a node.
       Input:      NW,     weighted undirected connection matrix
       Output:     C,      clustering coefficient vector
    Reference: Onnela et al. (2005) Phys Rev E 71:065103
    */
    rowvec K = degree(W);
    mat t = arma::pow(W,(1.0/3.0));
    rowvec cyc3 = conv_to<rowvec>::from(diagvec(t*t*t));
    K(find(cyc3 == 0)).fill(datum::inf);
    return cyc3/(K%(K-1));
}

double Connectome::transitivity(const mat &W)
{
    /*
    Transitivity is the ratio of 'triangles to triplets' in the network.
    (A classical version of the clustering coefficient).

        Input:      W       weighted undirected connection matrix
        Output:     T       transitivity scalar

    Reference: Onnela et al. (2005) Phys Rev E 71:065103
    */
    rowvec K = degree(W);
    mat t = arma::pow(W,(1.0/3.0));
    vec cyc3 = diagvec(t*t*t);
    return accu(cyc3)/sum(K%(K-1));
}

urowvec Connectome::modularity_louvain(mat W, double *Qopt, double gamma)
{
    /*
   The optimal community structure is a subdivision of the network into
   nonoverlapping groups of nodes in a way that maximizes the number of
   within-group edges, and minimizes the number of between-group edges.
   The modularity is a statistic that quantifies the degree to which the
   network may be subdivided into such clearly delineated groups.

   The Louvain algorithm is a fast and accurate community detection
   algorithm (as of writing). The algorithm may also be used to detect
   hierarchical community structure.

        Input:      W       undirected (weighted or binary) connection matrix.
                    gamma,  modularity resolution parameter (optional)
                                gamma>1     detects smaller modules
                                0<=gamma<1  detects larger modules
                                gamma=1     (default) classic modularity

        Outputs:    Ci,     community structure
                    Q,      modularity

   Note: Ci and Q may vary from run to run, due to heuristics in the
   algorithm. Consequently, it may be worth to compare multiple runs.

   Reference: Blondel et al. (2008)  J. Stat. Mech. P10008.
              Reichardt and Bornholdt (2006) Phys Rev E 74:016110.
      */
    uint N = W.n_rows, h = 1, n = N, u =0, ma =0, mb =0;
    double s = accu(W), wm = 0, max_dQ = -1;
    uvec M, t;
    rowvec dQ;
    field<urowvec> Ci(20);
    Ci(0) = urowvec(); Ci(1) = linspace<urowvec>(0,n-1,n);
    rowvec Q = "-1,0";

    while (Q(h)-Q(h-1) > 1e-10) {
        rowvec K = sum(W,0), Km = K;
        mat Knm = W;

        M = linspace<uvec>(0,n-1,n);

        bool flag = true;

        while (flag) {
            flag = false;

            arma_rng::set_seed_random();
            t = shuffle(linspace<uvec>(0,n-1,n));
            for (uint i =0;i<n;++i) {
                u = t(i);
                ma = M(u);
                dQ = Knm.row(u) - Knm(u,ma)+W(u,u)-
                        gamma*K(u)*(Km-Km(ma)+K(u))/s;
                dQ(ma) = 0;
                max_dQ = dQ.max();
                mb = as_scalar(find(dQ == max_dQ,1));
                if (max_dQ > 1e-10) {
                    flag = true;
                    M(u) = mb;

                    Knm.col(mb) += W.col(u);
                    Knm.col(ma) -= W.col(u);
                    Km(mb) += K(u);
                    Km(ma) -= K(u);
                }
            }
        }
        Ci(++h) = zeros<urowvec>(1,N);
        M = matlabUnique(M);
        for (uint u=0;u<n;++u) {
            Ci(h)(find(Ci(h-1) == u)).fill(M(u));
        }
        n = M.max()+1;
        mat w = zeros(n,n);
        for (uint u =0;u<n;++u)
            for (uint v=u;v<n;++v) {
                wm = accu(W(find(M==u),find(M==v)));
                w(u,v) = wm;
                w(v,u) = wm;
            }
        W = w;
        Q.resize(h+1);
        Q(h) = trace(W)/s - gamma*accu((W*W)/(s*s));
    }
    *Qopt = Q(h);
    return Ci(h);
}

rowvec Connectome::participationCoefficient(const mat &W, const urowvec &Ci)
{
    /*
   Participation coefficient is a measure of diversity of intermodular
   connections of individual nodes.
        Inputs:     W,      binary/weighted, directed/undirected
                            connection matrix
                    Ci,     community affiliation vector
        Output:     P,      participation coefficient

   Note: The output for directed graphs is the "out-neighbor"
         participation coefficient.
   Reference: Guimera R, Amaral L. Nature (2005) 433:895-900.
      */
    uint n = W.n_rows;
    mat binaryW = zeros(n,n);
    binaryW(find(W!=0)).fill(1);
    rowvec Ko = sum(binaryW,0);
    // A^T = (B C)^T = C^T B^T = C B^T since C = C^T
    mat Gc = diagmat(conv_to<rowvec>::from(Ci))*strans(abs(sign(W)));
    rowvec Kc2 = zeros(1,n);

    for (uint i=0;i<=Ci.max();++i) {
        mat c = zeros(n,n);
        c(find(Gc==i)).fill(1);
        Kc2 += arma::pow(sum((W%c),0),2);
    }

    rowvec P = ones(1,n) - Kc2/arma::pow(Ko,2);
    P(find(Ko == 0)).fill(0);
    return P;
}

rowvec Connectome::moduleZscore(const mat &W, const urowvec &Ci)
{
    /*
   The within-module degree z-score is a within-module version of degree
   centrality.

        Inputs:     W,      binary/weighted undirected connection matrix
                    Ci,     community affiliation vector
        Output:     Z,      within-module degree z-score.

   Reference: Guimera R, Amaral L. Nature (2005) 433:895-900.
      */
    uint n = W.n_rows;
    rowvec Z = zeros(1,n);
    rowvec Koi;
    for (uint i=0; i<=Ci.max();++i) {
        Koi = sum(W(find(Ci == i),find(Ci==i)),0);
        Z(find(Ci==i)) = (Koi - mean(Koi))/stddev(Koi);
    }
    Z(find(Z == datum::nan)).fill(0);
    return Z;
}

// functional integration
rowvec Connectome::localEfficiency(const mat &W)
{
  /*
   The local efficiency is the global efficiency computed on the
   neighborhood of the node, and is related to the clustering coefficient.

   Inputs:     W,              weighted undirected or directed connection matrix
                               (all weights in W must be between 0 and 1)
   Output:     Eloc,           local efficiency (vector)

   Notes:
       The  efficiency is computed using an auxiliary connection-length
   matrix L, defined as L_ij = 1/W_ij for all nonzero L_ij; This has an
   intuitive interpretation, as higher connection weights intuitively
   correspond to shorter lengths.
       The weighted local efficiency broadly parallels the weighted
   clustering coefficient of Onnela et al. (2005) and distinguishes the
   influence of different paths based on connection weights of the
   corresponding neighbors to the node in question. In other words, a path
   between two neighbors with strong connections to the node in question
   contributes more to the local efficiency than a path between two weakly
   connected neighbors. Note that this weighted variant of the local
   efficiency is hence not a strict generalization of the binary variant.

   Algorithm:  Dijkstra's algorithm

   References: Latora and Marchiori (2001) Phys Rev Lett 87:198701.
               Onnela et al. (2005) Phys Rev E 71:065103
               Fagiolo (2007) Phys Rev E 76:026107.
               Rubinov M, Sporns O (2010) NeuroImage 52:1059-69
    */
    int n = W.n_rows;
    mat A = abs(sign(W)), L = 1/W, e ,se;
    L.diag().fill(0);
    rowvec Eloc = zeros(1,n), w1, sw, sa;
    colvec w2;
    uvec V;
    double numer = 0, denom = 0;

    for (int u =0;u<n;++u) {
        V = find( A.row(u)+ trans(A.col(u)) != 0 );
        w1 = arma::pow(W(uvec(1).fill(u),V),(1.0/3.0));
        w2 = arma::pow(W(V,uvec(1).fill(u)),(1.0/3.0));
        sw = w1 + trans(w2);
        // e=distance_inv_wei(L(V,V));
        e = 1/pathLength(L(V,V));
        e(find(e == datum::inf)).fill(0);

        se = arma::pow(e,(1.0/3.0)) + arma::pow(trans(e),(1.0/3.0));
        numer = accu((trans(sw)*sw)%se)/2.0;

        if (numer!=0.0) {
            sa = A(uvec(1).fill(u),V) + trans(A(V,uvec(1).fill(u)));
            denom = qPow(sum(sa),2.0)-accu(sa%sa) ;
            Eloc(u) = numer/denom;
        }
    }
    return Eloc;
}

double Connectome::globalEfficiency(const mat &W)
{
    uint n = W.n_rows;
    mat e = 1/pathLength(1/W);
    e(find(e == datum::inf)).fill(0);
    return accu(e)/(n*n-n);
}

mat Connectome::pathLength(const mat &G)
{
    /*
   D = pathLength(G);

   The distance matrix contains lengths of shortest paths between all
   pairs of nodes. An entry (u,v) represents the length of shortest path
   from node u to node v. The average shortest path length is the
   characteristic path length of the network.

        Input:      G,      weighted directed/undirected connection matrix
        Output:     D,      distance matrix

       The input matrix must be a mapping from weight to distance. For
   instance, in a weighted correlation network, higher correlations are
   more naturally interpreted as shorter distances, and the input matrix
   should consequently be some inverse of the connectivity matrix.
       Lengths between disconnected nodes are set to Inf.
       Lengths on the main diagonal are set to 0.

   Algorithm: Dijkstra's algorithm.
   */

    uint n = G.n_rows,v=0;
    mat D = mat(n,n).fill(datum::inf), G1, t;
    D.diag().fill(0);
    uvec S, V, W, tt;
    for (uint u=0;u<n;++u) {
        S = linspace<uvec>(0,n-1,n);
        G1 = G;
        V = uvec(1).fill(u);

        while (true) {
            // instead of replacing indices by 0 like S(V)=0;
            // we declare all indices then remove the V indeces
            // from S. Notice that it is assured that tt should
            // be one element since indices don't repeat
            for (int i=0;i<V.n_elem;++i) {
                tt = find(S == V(i),1);
                if (!tt.is_empty())
                    S.shed_row(tt(0));
            }
            G1.cols(V).fill(0);
            for (uint j = 0;j<V.n_elem;++j) {
                v = V(j);
                W = find(G1.row(v)>0);
                D(uvec(1).fill(u),W) = arma::min(D(uvec(1).fill(u),W),
                                                 D(u,v)+G1(uvec(1).fill(v),W));
            }
            t = D(uvec(1).fill(u),S);
            if (t.is_empty() || !is_finite(t.min()))
                break;
            V = find( D.row(u) == t.min());
        }
    }
    return D;
}

rowvec Connectome::vulnerability(const mat &W)
{
    // describes decrease in global efficiency
    // when a node is removed from the network

    uint N = W.n_rows;
    rowvec V = zeros(1,N);
    double Eglob = globalEfficiency(W), Eglobx = 0.0;
    mat newW = W;
    for (uint x=0; x<N;++x) {
        newW.shed_row(x);
        newW.shed_col(x);
        Eglobx = globalEfficiency(newW);
        V(x) = (Eglob - Eglobx)/Eglob;
        newW = W;
    }
    return V;
}

// core structure
double Connectome::assortativity(const mat &W)
{
    /*
   The assortativity coefficient is a correlation coefficient between the
   strengths (weighted degrees) of all nodes on two opposite ends of a link.
   A positive assortativity coefficient indicates that nodes tend to link to
   other nodes with the same or similar strength.

        Inputs:     CIJ,    weighted directed/undirected connection matrix
        Outputs:    r,      assortativity coefficient

   Notes: The function accepts weighted networks, but all connection
   weights are ignored. The main diagonal should be empty. For flag 1
   the function computes the directed assortativity described in Rubinov
   and Sporns (2010) NeuroImage.

   Reference:  Newman (2002) Phys Rev Lett 89:208701
               Foster et al. (2010) PNAS 107:10815–10820
      */
    rowvec str = strength(W);
    mat Wt = trimatu(W); Wt.diag().fill(0);
    umat idx = getIndex(find(Wt>0),W.n_rows,W.n_cols,0);
    uint K = idx.n_rows;
    vec stri = str(idx.col(0));
    vec strj = str(idx.col(1));
    double  a = accu(stri%strj)/K,
            b = accu(0.5*(stri+strj))/K,
            c = accu(0.5*(pow(stri,2)+pow(strj,2)))/K;
    return (a-b*b)/(c-b*b);
}

rowvec Connectome::richClub(const mat &W, int klevel)
{
    /*
        inputs:     W:       weighted connection matrix
                    optional:
                    k-level: max level of RC(k).
                    When k-level is -1, k-level is set to max of degree of W
        output:     rich:         rich-club curve

    adopted from Opsahl et al. Phys Rev Lett, 2008, 101(16)

      */
    rowvec nodeDegree = degree(W);
    if (klevel == -1)
        klevel = nodeDegree.max();
    vec wrank = sort(vectorise(W),"descend");
    rowvec Rw = rowvec(1,W.n_rows).fill(datum::nan);
    uvec smallNodes;
    for (uint kk=0;kk<klevel;++kk) {
        smallNodes = find(nodeDegree<kk+1);
        if (smallNodes.is_empty())
            continue;
        mat cutOutW = W;
        smallNodes = sort(smallNodes,"descend");
        for (uint i =0;i<smallNodes.n_elem;++i) {
            cutOutW.shed_row(smallNodes(i));
            cutOutW.shed_col(smallNodes(i));
        }
        double Wr = accu(cutOutW);
        uvec t = find(cutOutW != 0);
        if (!t.is_empty())
            Rw(kk) = Wr/ accu(wrank.subvec(0,t.n_elem-1));
    }
    return Rw;
}

void Connectome::computeMetrics()
{
    if (NW.is_empty())
        return;
    // local metrics order:
    // strength, degree, PL, eff, vul, CC, betweeness,
    // PC, Z-score, modularity, rich-club
    // nodal local metrics

    uint n = NW.n_rows;
    localMetrics.set_size(11,n);
    localMetrics.row(0) = strength(NW);                 // strength
    localMetrics.row(1) = degree(NW);                   // degree
    pl = pathLength(1/NW);
    pl(find(pl == datum::inf)).fill(0);
    localMetrics.row(2) = sum(pl,0)/n;                  // path length
    localMetrics.row(3) = localEfficiency(NW);          // efficiency
    localMetrics.row(4) = vulnerability(NW);            // vulnerability

    localMetrics.row(5) = clusteringCoeff(NW);          // CC
    localMetrics.row(6) = betweenessCentrality(1/NW,    // betweeness: Hubs
                                               edgeBetweeness);
    // Even though modularity is not a local metric, however,
    // the output have a number of elements = number of nodes
    double Q;
    urowvec Ci = modularity_louvain(NW,&Q);
    localMetrics.row(7)=participationCoefficient(NW,Ci);// participation coeff
    localMetrics.row(8) = moduleZscore(NW,Ci);          // Z score

    localMetrics.row(9) = conv_to<rowvec>::from(Ci);    // modularity
    localMetrics.row(10) = richClub(NW,-1);             // rich club

    // global metrics order:
    // Strength_mean, degree_mean, CCmean,
    // Q modularity, CPL, GE, trans, density
    // assortativity
    globalMetrics.set_size(10,1);
    globalMetrics(0) = mean(localMetrics.row(0));   // mean strength
    globalMetrics(1) = mean(localMetrics.row(1));   // mean degree
    globalMetrics(2) = mean(localMetrics.row(5));   // mean CC
    globalMetrics(3) = Q;                           // modularity
    globalMetrics(4) = accu(pl)/(n*n-n);            // CPL
    // reusing computed shortest path to compute GE
    mat invPL = 1/pl;
    invPL(find(invPL == datum::inf)).fill(0);
    globalMetrics(5) = accu(invPL)/(n*n-n);         // global eff
    globalMetrics(6) = transitivity(NW);            // transitivity
    globalMetrics(7) = density(NW);                 // density
    globalMetrics(8) = assortativity(NW);           // assortativity
    globalMetrics(9) = 0.0;

    metricsExist = true;
}

bool Connectome::writeMetrics(const QString &fileName,bool quiet)
{
    if (!metricsExist)
        return false;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        if (!quiet)
            QMessageBox::critical(0, "Warning!", QString("Cannot write file %1:\n%2.")
                                  .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out<<"Local Metrics:"<<endl;
    for (uint i=0;i<names.size();++i)
        out<<names[i]<<"\t";
    out<<endl;
    QStringList metrics;
    metrics << "Strength" << "Degree" << "PL" << "Efficiency" << "Vulnerability" << "CC"
            << "Betweeness Centrality" << "Participation Coeff" << "Z-score";
    for (uint i =0; i<localMetrics.n_rows-2;++i) {
        out<<metrics[i]<<"\t";
        for (uint j =0; j<localMetrics.n_cols;++j)
            out<<localMetrics(i,j)<<"\t";
        out<<endl;
    }
    out<<QString("Modularity of value %1 at:").arg(globalMetrics(3));
    for (uint j =0; j<localMetrics.n_cols;++j)
        out<<localMetrics(9,j)<<"\t";
    out<<"Rich Club:";
    for (uint j =0; j<localMetrics.n_cols;++j)
        out<<localMetrics(10,j)<<"\t";
    metrics.clear();
    metrics << "Mean strength" << "Mean degree" << "Mean CC" << "Modularity"
            << "CPL" << "Global efficiency" << "Transitivity" << "Density"
            << "Assortativity";
    out<<"Global Metrics:"<<endl;
    for (uint i =0; i<globalMetrics.n_elem-1;++i) {
        out<<metrics[i]<<"\t"<<globalMetrics(i)<<endl;
    }
    file.close();
    return true;
}
