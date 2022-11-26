#include "overrideold.h"
#include "ui_overrideold.h"

OverrideOld::OverrideOld(int old_major, int old_minor, int old_fix, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OverrideOld)
{
    ui->setupUi(this);
}

OverrideOld::~OverrideOld()
{
    delete ui;
}
