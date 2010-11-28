#ifndef FRMMAINAA_UI_H
#define FRMMAINAA_UI_H

#include <QVBoxLayout>
#include <QShortcut>
#include "mpiTableView.h"
#include "frmMainToolbar_UI.h"

class frmMainTableView_UI: public frmMainToolbar_UI
{
public:
    QWidget *widget;
    QVBoxLayout *layout;
    QWidget *widgetTable;
    QHBoxLayout *layoutTable;
    mpiTableView *table;
    QShortcut *tableCopy;

    virtual ~frmMainTableView_UI() {}

    virtual void setupUI(const QMap<int, QString> &columnNames_, QWidget *parent_ = 0);
};

#endif // FRMMAINAA_UI_H
