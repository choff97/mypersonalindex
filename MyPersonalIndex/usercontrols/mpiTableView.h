#ifndef MPITABLEVIEW_H
#define MPITABLEVIEW_H

#include <QObject>
#include <QTableView>
#include <QWidget>
#include <QTextStream>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include "functions.h"


class mpiTableView : public QTableView
{
    Q_OBJECT

public:
    mpiTableView(const bool &hasRowLabels, QWidget *parent = 0): QTableView(parent), m_hasRowLabels(hasRowLabels) {}

public slots:
    void exportTable(bool copy = false);
    void copyTable() { exportTable(true); }

private:
    bool m_hasRowLabels;
};

#endif // MPITABLEVIEW_H
