#include "removenew.h"
#include "ui_removenew.h"

RemoveNew::RemoveNew(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RemoveNew)
{
    ui->setupUi(this);
}

RemoveNew::~RemoveNew()
{
    delete ui;
}
