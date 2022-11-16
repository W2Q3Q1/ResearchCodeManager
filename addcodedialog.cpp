#include "addcodedialog.h"
#include "ui_addcodedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "codemanager.h"

AddCodeDialog::AddCodeDialog(int research_idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCodeDialog),
    research_idx(research_idx)
{
    ui->setupUi(this);

    //connections
    void (QPushButton::*push_button_click) (bool) = &QPushButton::clicked;
    void (AddCodeDialog::*add_code_dialog_open_file_button_clicked) () = &AddCodeDialog::openFileButtonClicked;
    void (AddCodeDialog::*add_code_dialog_open_dir_button_clicked) () = &AddCodeDialog::openDirButtonClicked;
    connect(ui->openFileButton, push_button_click, this, add_code_dialog_open_file_button_clicked);
    connect(ui->openDirButton, push_button_click, this, add_code_dialog_open_dir_button_clicked);
}

AddCodeDialog::~AddCodeDialog()
{
    delete ui;
}

QString AddCodeDialog::getNameInput(){
    return ui->nameEdit->text();
}

QString AddCodeDialog::getPathInput(){
    return ui->pathEdit->text();
}

QString AddCodeDialog::getInformationInput(){
    return ui->informationEdit->document()->toPlainText();
}

void AddCodeDialog::openFileButtonClicked(){
    ui->pathEdit->setText(QFileDialog::getOpenFileName(this, tr("Open")));
}

void AddCodeDialog::openDirButtonClicked(){
    ui->pathEdit->setText(QFileDialog::getExistingDirectory(this, tr("Open")));
}

void AddCodeDialog::accept(){
    CodeManager *manager = CodeManager::getSelf();
    if(ui->nameEdit->text().isEmpty()){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Code Name Empty"));
        return;
    }
    if(manager->researchIDCodeNameExist(this->research_idx, ui->nameEdit->text())){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Code Name Duplicated"));
        return;
    }
    QDialog::accept();
}
