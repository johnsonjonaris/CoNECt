/*
    A delegate that allows the user to change integer values from the model
    using a spin box widget.
*/


#include "delegate.h"

SpinBoxDelegate::SpinBoxDelegate(QSpinBox *s, QObject *parent) :
    QItemDelegate(parent),
    s(s)
{}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(100);
    editor->setSingleStep(10);
    editor->setFrame(false);
    // allow edits to the editor to appear in the main QSpinBox in the GUI
    if (s)
        connect(editor,SIGNAL(valueChanged(int)),s,SLOT(setValue(int)));
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor,
                                   QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
