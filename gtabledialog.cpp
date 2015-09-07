#include "gtabledialog.h"
#include <QtDebug>

gTableDialog::gTableDialog(QList<mat> *list, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    // connect signals
    connect(gTableListBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(ongTableListBoxChange(int)));
    connect(ImportTableButton,SIGNAL(clicked()),
            this,SLOT(onImportTabelButtonPress()));
    connect(AddTableButton,SIGNAL(clicked()),
            this,SLOT(onAddTableButtonPress()));
    connect(SaveButton,SIGNAL(clicked()),
            this,SLOT(onSaveButtonPress()));
    connect(DeleteTableButton,SIGNAL(clicked()),
            this,SLOT(onDeleteTableButtonPress()));
    connect(gTableEdit,SIGNAL(textChanged()),
            this,SLOT(onTableEditChange()));
    gTableList = list;
    SaveButton->setEnabled(false);
}

bool gTableDialog::readGradientTable(QString fileName,mat &gTable)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream in(&file);
    QStringList fields;
    gTable = Mat<double>(1024,3);
    int c =0;
    bool ok1,ok2,ok3;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;
        // parse text, expected 4 fields: No.:  X, Y, Z
        fields = line.split(QRegExp("\\s+|:|,"), QString::SkipEmptyParts);
        if (fields.size() < 4)
            return false;
        // fill gradient table
        gTable(c,0) = fields.at(1).toDouble(&ok1);
        gTable(c,1) = fields.at(2).toDouble(&ok2);
        gTable(c,2) = fields.at(3).toDouble(&ok3);
        if (!ok1 || !ok2 || !ok3)
            return false;
        c++;
        if (c>1023)
            return false;
    }
    gTable.shed_rows(c,1023);
    return true;
}

void gTableDialog::ongTableListBoxChange(int index)
{
    // display table accordingly
    gTableEdit->clear();
    if ((gTableList == NULL) ||
            gTableList->isEmpty() ||
            (index<0) ||
            (index > gTableList->size()-1))
        return;
    for (int i=0; i<gTableList->at(index).n_rows; i++) {
        QString str = QString::number(i)+ ": " + QString::number(gTableList->at(index).at(i,0)) +
                ", " + QString::number(gTableList->at(index).at(i,1)) +
                ", " + QString::number(gTableList->at(index).at(i,2));
        gTableEdit->appendPlainText(str);
    }
    gTableEdit->moveCursor(QTextCursor::Start);
    gTableEdit->ensureCursorVisible();
    SaveButton->setEnabled(false);
}

bool gTableDialog::scangTableEdit(mat &gTable)
{
    QStringList str = gTableEdit->toPlainText().split(QRegExp("\\s+|:|,"), QString::SkipEmptyParts);
    // the table should be made of 4 columns separated by : or ,
    // expected the number of elements to be multiple of 4
    if (str.isEmpty() || (str.size()%4 != 0))
        return false;
    gTable = Mat<double>(str.size()/4,3);
    bool ok;
    if (!str.isEmpty()) {
        // skip first element every 4 elements
        for (int i = 0; i<str.size()/4;i++)
            for (int j = 0; j<4;j++) {
                if (j>0) {
                    gTable(i,j-1) = str.at(i*4+j).toDouble(&ok);
                    if (!ok)
                        return false;
                }
            }
    }
    return true;
}

void gTableDialog::onSaveButtonPress()
{
    mat gTable;
    if(scangTableEdit(gTable)) {
        // store the table
        gTableList->replace(gTableListBox->currentIndex(), gTable);
        storeCurrentTables();
        SaveButton->setEnabled(false);
    }
    else {
        QMessageBox::critical(this, "Warning!", "Errors in the current table, "
                              "please review your edits and try again.");
    }

}

bool gTableDialog::storeCurrentTables()
{
    // write the table in the file
    QString fileName = QDir::currentPath() + "/gTables.gtbl";
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QString msg = QString("Cannot open gradient file %1:\n%2 for edit. Can not save edits.")
                .arg(file.fileName()).arg(file.errorString());
        QMessageBox::critical(this, "Warning!", msg);
        return false;
    }
    QTextStream out(&file);
    for (int t=0;t<gTableList->size();t++) {
        mat gTable = gTableList->at(t);
        // skip empty tables
        if (gTable.is_empty())
            continue;
        out<<QString("#"+gTableListBox->itemText(t)+"\n");
        for (int i=0;i<gTable.n_rows;i++) {
            QString str = QString::number(i)+ ": " + QString::number(gTable(i,0)) +
                    ", " + QString::number(gTable(i,1)) +
                    ", " + QString::number(gTable(i,2)) + "\n";
            out<<str;
        }
    }
    return true;
}

void gTableDialog::onImportTabelButtonPress()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select file", ".",
                                                    "Gradient files (*.*)");
    mat gTable;
    if (!fileName.isEmpty()) {
        if (readGradientTable(fileName,gTable)) {
            // if correct table save it and add an entry in the list box and save it
            QString str = getTableName();
            if (!str.isEmpty())
                gTableListBox->addItem(str);
            else
                return;
            gTableList->append(gTable);
            gTableListBox->setCurrentIndex(gTableListBox->count()-1);
            storeCurrentTables();
        }
        else
            QMessageBox::critical(this, "Warning!", "Can not open file, check "
                                  " the file content and try again.");
    }
}

void gTableDialog::onDeleteTableButtonPress()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Table",
                                                              "Do you want to delete this table?",
                                                              QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int idx = gTableListBox->currentIndex();
        gTableList->removeAt(idx);
        gTableListBox->removeItem(idx);
        storeCurrentTables();
    }
}

void gTableDialog::onAddTableButtonPress()
{
    QString str = getTableName();
    if (!str.isEmpty()) {
        gTableList->append(Mat<double>(0,0));
        gTableListBox->addItem(str);
        gTableListBox->setCurrentIndex(gTableListBox->count()-1);
    }
}

QString gTableDialog::getTableName()
{
    QDialog *w = new QDialog(this);
    w->setWindowTitle("Add Table");
    w->setWindowModality(Qt::ApplicationModal);
    QVBoxLayout *vl = new QVBoxLayout(w);
    vl->setSpacing(6);
    vl->setContentsMargins(11, 11, 11, 11);
    QLineEdit *line = new QLineEdit("",w);
    vl->addWidget(line);
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Cancel|
                                                QDialogButtonBox::Ok,Qt::Horizontal,w);
    vl->addWidget(bb);
    connect(bb, SIGNAL(accepted()), w, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), w, SLOT(reject()));
    w->setFixedSize(w->sizeHint());
    if(w->exec())
        return line->text();
    else
        return QString();
}
