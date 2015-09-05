#ifndef TABLE_H
#define TABLE_H

#include <QTreeWidget>
/*
    This class is a derivative of the QTreeWidget, it differs
    from its parent that it will allow some pre-specified columns
    to be editable. It will handle the double click and ending the
    edit state. Note that QTreeWidget allows editing the whole row
    by setting ItemIsEditable, hence, this class comes in handy.
    The user have to set the appropriate delegate to each item when
    creating the tree.
  */
class TableTree : public QTreeWidget
{
    Q_OBJECT
public:

    TableTree(QWidget *parent = 0);

    QVector<quint16> editableColumns;
    void setEditableColumns(QVector<quint16> l) {editableColumns = l;}

private slots:

    void ItemDoubleClicked(QTreeWidgetItem *, int);
    void ItemChanged(QTreeWidgetItem *, int);
    void CurItemChanged(QTreeWidgetItem *,QTreeWidgetItem *);
};

#endif // TABLE_H
