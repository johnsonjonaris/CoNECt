#ifndef TABLE_H
#define TABLE_H

#include <QTreeWidget>

/**
  * \class QTreeWidget
  *
  * This class is a derivative of the QTreeWidget:
  *     It will allow some pre-specified columns to be editable.
  *     It will handle the double click and ending the edit state.
  * Note: QTreeWidget allows editing the whole row
  * by setting ItemIsEditable, hence, this class comes in handy.
  * The user have to set the appropriate delegate to each item when
  * creating the tree.
  */

class TableTree : public QTreeWidget
{
    Q_OBJECT
public:

    TableTree(QWidget *parent = 0);

    QVector<quint16> editableColumns;   ///< store editable columns
    /// set editable columns
    void setEditableColumns(QVector<quint16> l) {editableColumns = l;}

private slots:
    /// invoked when a cell is double clicked
    void ItemDoubleClicked(QTreeWidgetItem *, int);
    /// invoked when a cell is changed
    void ItemChanged(QTreeWidgetItem *, int);
    /// invoked when current cell is changed
    void CurrentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *);
};

#endif // TABLE_H
