#include "addresearchdialog.h"
#include "ui_addresearchdialog.h"
#include "codemanager.h"
#include <QMessageBox>

AddResearchDialog::AddResearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddResearchDialog)
{
    ui->setupUi(this);
}

AddResearchDialog::~AddResearchDialog()
{
    delete ui;
}

QString AddResearchDialog::getNameInput(){
    return ui->nameEdit->text();
}

QString AddResearchDialog::getInformationInput(){
    return ui->informationEdit->document()->toPlainText();
}

void AddResearchDialog::accept(){
    CodeManager *manager = CodeManager::getSelf();
    if(ui->nameEdit->text().isEmpty()){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Research Name Empty"));
        return;
    }
    if(manager->researchNameExist(ui->nameEdit->text())){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Research Name Duplicated"));
        return;
    }
    QDialog::accept();
}
