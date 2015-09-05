#include "misc_functions.h"

void onCopyKeysPressed(QTableWidget *table)
{
    QAbstractItemModel *abmodel = table->model();
    QModelIndexList list = table->selectionModel()->selectedIndexes();
    qSort(list);

    if(list.size() < 1)
        return;

    QString tableTxt;
    QModelIndex previous = list.first();

    list.removeFirst();

    for(int i = 0; i < list.size(); ++i) {
        tableTxt.append(abmodel->data(previous).toString());

        QModelIndex index = list.at(i);
        if(index.row() != previous.row())
            tableTxt.append('\n');
        else
            tableTxt.append('\t');
        previous = index;
    }

    tableTxt += abmodel->data(list.last()).toString() + '\n';

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tableTxt);
}

uvec matlabUnique(const uvec &v)
{
    /*
        This function mimics the behavior of Matlab unique
        [C,IA,IC] = UNIQUE(A) such that C = A(IA) and A = C(IC).
        This function returns IC
      */
    if (v.n_elem == 1)
        return uvec(1).fill(0);
    uvec ndx = sort_index(v);
    uvec b = v(ndx);
    uvec db = b.subvec(1,b.n_elem-1)-b.subvec(0,b.n_elem-2);
    uvec d = abs(sign(db));
    d.insert_rows(0,1);
    uvec pos = cumsum(d);
    pos(ndx) = pos;
    return pos;
}

umat getIndex(const uvec &idx, int r, int c, int s)
{
    umat indices;
    if (c < 2)
        return idx;
    else if (s < 1)
        indices = zeros<umat>(idx.n_elem,2);
    else
        indices = zeros<umat>(idx.n_elem,3);
    for (uint i =0; i<idx.n_elem;++i) {
        indices(i,0) = idx(i) % r;
        indices(i,1) = (idx(i)/r) % c;
        if (s>0)
            indices(i,2) = (idx(i)/(r*c)) % s;
    }
    return indices;
}

