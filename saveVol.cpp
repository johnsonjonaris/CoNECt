#include "saveVol.h"

SaveImageDialog::SaveImageDialog(const QStringList &list,const mat &info,QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    LoadedVolsList->insertItems(0,list);
    volsInfo = info;            // should be X*8
    connect(buttonBox,      SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox,      SIGNAL(accepted()), this, SLOT(checkBeforeExit()));
    connect(TransferButton, SIGNAL(clicked()),  this, SLOT(addItem()));
    connect(RemoveButton,   SIGNAL(clicked()),  this, SLOT(removeItem()));
}
// transfer volume to the ToBeSavedVolsList
void SaveImageDialog::addItem()
{
    // in order to show all elements at once, disable update before editing
    // then enable it at the end
    ToBeSavedVolsList->setUpdatesEnabled(false);
    QModelIndexList indexes = LoadedVolsList->selectionModel()->selectedIndexes();
    // sort the list
    ucolvec tmp(indexes.size());
    for (int i =0;i<indexes.size();i++)
        tmp(i) = indexes.at(i).row();
    tmp = sort(tmp);
    // add to be saved items to the right list
    for(uint i = 0;i<tmp.n_elem; i++) {
        ToBeSavedVolsList->addItem(LoadedVolsList->item(tmp(i))->text());
        volsToBeSaved.append(tmp(i));
    }
    ToBeSavedVolsList->setUpdatesEnabled(true);
}
// remove volume from the ToBeSavedVolsList
void SaveImageDialog::removeItem()
{
    ToBeSavedVolsList->setUpdatesEnabled(false);
    QModelIndexList indexes = ToBeSavedVolsList->selectionModel()->selectedIndexes();
    qSort(indexes.begin(), indexes.end());
    for(int i = indexes.size() - 1; i > -1; --i) {
         ToBeSavedVolsList->takeItem(indexes.at(i).row());
         volsToBeSaved.remove(indexes.at(i).row());
    }
    ToBeSavedVolsList->setUpdatesEnabled(true);
}

void SaveImageDialog::checkBeforeExit()
{
    if (volsToBeSaved.isEmpty()) {
        QMessageBox::warning(this,"Save Image","Please select image(s) to be saved.");
        return;
    }
    bool check = true;
    // get information of volumes to be saved, they must be identical
    // datatype,dx,dy,dz,nRows,nCols,nSlices
    mat tmpVolsInfo = Mat<double>(volsToBeSaved.size(),volsInfo.n_cols);
    for (int i=0;i<volsToBeSaved.size();i++)
        tmpVolsInfo(span(i),span::all) = volsInfo(span(volsToBeSaved[i]),span::all);
    for (uint i=0; i<7;i++) {
        colvec z = sort(tmpVolsInfo.col(i));
        if (z(0) != z(z.n_elem-1)) {
            check = false;
            break;
        }
    }
    if (check)
        accept();
    else
        QMessageBox::warning(this,"Save Image","Chosen image(s) are incompatible.");
}
