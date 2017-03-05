#include "mainwindow.h"
#include "mapwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_mapWidget = new Tangram::MapWidget(this);
    setCentralWidget(m_mapWidget);
}

MainWindow::~MainWindow()
{
}
