#include "table.h"

TableTree::TableTree(QWidget *parent) : QTreeWidget(parent) {

    setFrameShape(QFrame::Box);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setTabKeyNavigation(true);
    setRootIsDecorated(false);
    setUniformRowHeights(true);

    connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(ItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this,SLOT(CurItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,SLOT(ItemChanged(QTreeWidgetItem*,int)));
}

void TableTree::ItemDoubleClicked(QTreeWidgetItem *item, int col)
{
    // when an item is double clicked, allow editing if available
    if (item->isFirstColumnSpanned() || editableColumns.isEmpty())
        return;
    for (int i = 0;i<editableColumns.size();i++)
        if (editableColumns.at(i) == col) {
            item->setFlags(item->flags()|Qt::ItemIsEditable);
            return;
        }
}

void TableTree::ItemChanged(QTreeWidgetItem *item, int col)
{
    // if the item contents changed and it was editable, disable editing
    if (item->isFirstColumnSpanned() || editableColumns.isEmpty())
        return;

    for (int i = 0;i<editableColumns.size();i++)
        if (editableColumns.at(i) == col) {
            item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            return;
        }
}

void TableTree::CurItemChanged(QTreeWidgetItem *curItem, QTreeWidgetItem *prevItem)
{
    Q_UNUSED(curItem);
    // if current item is changed close the editor and prevent editing
    // this is done to correct change of forcus from current item under edit
    // if no editor open, nothing will happen
    if (editableColumns.isEmpty())
        return;
    if (prevItem) {
        for (int i = 0; i< editableColumns.size();i++) {
            closePersistentEditor(prevItem,editableColumns.at(i));
            prevItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        }
    }
}
