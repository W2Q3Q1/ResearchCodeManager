#include "proxysettingdialog.h"
#include "ui_proxysettingdialog.h"

ProxySettingDialog::ProxySettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProxySettingDialog)
{
    ui->setupUi(this);
}

ProxySettingDialog::~ProxySettingDialog()
{
    delete ui;
}
