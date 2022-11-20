#include "addresultdialog.h"
#include "ui_addresultdialog.h"
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include "codemanager.h"

AddResultDialog::AddResultDialog(int code_idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddResultDialog),
    code_idx(code_idx)
{
    ui->setupUi(this);

    //connections
    void (QPushButton::*push_button_clicked) (bool) = &QPushButton::clicked;
    void (AddResultDialog::*add_result_dialog_configOpenFileButtonClicked) () = &AddResultDialog::configOpenFileButtonClicked;
    void (AddResultDialog::*add_result_dialog_configOpenDirButtonClicked) () = &AddResultDialog::configOpenDirButtonClicked;
    void (AddResultDialog::*add_result_dialog_logOpenFileButtonClicked) () = &AddResultDialog::logOpenFileButtonClicked;
    void (AddResultDialog::*add_result_dialog_logOpenDirButtonClicked) () = &AddResultDialog::logOpenDirButtonClicked;
    void (AddResultDialog::*add_result_dialog_addNewHPKV) () = &AddResultDialog::addNewHPKV;
    void (AddResultDialog::*add_result_dialog_deleteCurrentHPKV) () = &AddResultDialog::deleteCurrentHPKV;
    void (AddResultDialog::*add_result_dialog_addNewRKV) () = &AddResultDialog::addNewRKV;
    void (AddResultDialog::*add_result_dialog_deleteCurrentRKV) () = &AddResultDialog::deleteCurrentRKV;
    connect(ui->openConfigFileButton, push_button_clicked, this, add_result_dialog_configOpenFileButtonClicked);
    connect(ui->openConfigDirButton, push_button_clicked, this, add_result_dialog_configOpenDirButtonClicked);
    connect(ui->openLogFileButton, push_button_clicked, this, add_result_dialog_logOpenFileButtonClicked);
    connect(ui->openLogDirButton, push_button_clicked, this, add_result_dialog_logOpenDirButtonClicked);
    connect(ui->addHPButton, push_button_clicked, this, add_result_dialog_addNewHPKV);
    connect(ui->deleteHPButton, push_button_clicked, this, add_result_dialog_deleteCurrentHPKV);
    connect(ui->addRButton, push_button_clicked, this, add_result_dialog_addNewRKV);
    connect(ui->deleteRButton, push_button_clicked, this, add_result_dialog_deleteCurrentRKV);
    void (QTableWidget::*table_widget_currentCellChanged) (int, int, int, int) = &QTableWidget::currentCellChanged;
    void (AddResultDialog::*add_result_dialog_refreshHPDeletable) () = &AddResultDialog::refreshHPDeletable;
    void (AddResultDialog::*add_result_dialog_refreshRDeletable) () = &AddResultDialog::refreshRDeletable;
    connect(ui->hyperParamKVTable, table_widget_currentCellChanged, this, add_result_dialog_refreshHPDeletable);
    connect(ui->resultKVTable, table_widget_currentCellChanged, this, add_result_dialog_refreshRDeletable);
}

void AddResultDialog::configOpenFileButtonClicked(){
    ui->configPathEdit->setText(QFileDialog::getOpenFileName(this, tr("Open")));
}

void AddResultDialog::configOpenDirButtonClicked(){
    ui->configPathEdit->setText(QFileDialog::getExistingDirectory(this, tr("Open")));
}

void AddResultDialog::logOpenFileButtonClicked(){
    ui->logPathEdit->setText(QFileDialog::getOpenFileName(this, tr("Open")));
}

void AddResultDialog::logOpenDirButtonClicked(){
    ui->logPathEdit->setText(QFileDialog::getExistingDirectory(this, tr("Open")));
}

void AddResultDialog::addNewHPKV(){
    this->addNewKV(ui->hyperParamKVTable);
}

void AddResultDialog::deleteCurrentHPKV(){
    this->deleteCurrentKV(ui->hyperParamKVTable);
}

void AddResultDialog::addNewRKV(){
    this->addNewKV(ui->resultKVTable);
}

void AddResultDialog::deleteCurrentRKV(){
    this->deleteCurrentKV(ui->resultKVTable);
}

void AddResultDialog::refreshHPDeletable(){
    this->refreshDeletable(ui->deleteHPButton, ui->hyperParamKVTable);
}

void AddResultDialog::refreshRDeletable(){
    this->refreshDeletable(ui->deleteRButton, ui->resultKVTable);
}

void AddResultDialog::accept(){
    //检查名称是否填写
    if(ui->resultNameEdit->text().isEmpty()){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Result Name Empty"));
        return;
    }
    //检查是否重名
    CodeManager *manager = CodeManager::getSelf();
    if(manager->codeIDResultNameExist(this->code_idx, ui->resultNameEdit->text())){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Result Name Duplicated"));
        return;
    }
    //检查重复的键和空键
    QSet<QString> keys;
    for(int i=0; i<ui->hyperParamKVTable->rowCount(); i++){
        QString now_key = ui->hyperParamKVTable->item(i, 0)->text();
        if(now_key.isEmpty()){
            QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Hyper Parameter Key Empty"));
            return;
        }
        if(keys.contains(now_key)){
            QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Hyper Parameter Key Duplicated"));
            return;
        }
        keys.insert(now_key);
    }
    keys.clear();
    for(int i=0; i<ui->resultKVTable->rowCount(); i++){
        QString now_key = ui->resultKVTable->item(i, 0)->text();
        if(now_key.isEmpty()){
            QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Result Key Empty"));
            return;
        }
        if(keys.contains(now_key)){
            QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Result Key Duplicated"));
            return;
        }
        keys.insert(now_key);
    }
    QDialog::accept();
}

void AddResultDialog::showEvent(QShowEvent *event){
    QDialog::showEvent(event);
    this->refreshRDeletable();
    this->refreshHPDeletable();
}

void AddResultDialog::addNewKV(QTableWidget *widget){
    int row_idx = widget->rowCount();
    widget->insertRow(row_idx);
    widget->setItem(row_idx, 0, new QTableWidgetItem);
    widget->setItem(row_idx, 1, new QTableWidgetItem);
}

void AddResultDialog::deleteCurrentKV(QTableWidget *widget){
    assert(widget->currentRow() >= 0);
    widget->removeRow(widget->currentRow());
}

void AddResultDialog::refreshDeletable(QPushButton *delete_button, QTableWidget *widget){
    if(widget->currentRow() >= 0){
        delete_button->setEnabled(true);
    }else{
        delete_button->setEnabled(false);
    }
}

AddResultDialog::~AddResultDialog()
{
    delete ui;
}

QString AddResultDialog::getNameInput(){
    return ui->resultNameEdit->text();
}

QMap<QString, QString> AddResultDialog::getHPInput(){
    QMap<QString, QString> result;
    QString k, v;
    int row_num = ui->hyperParamKVTable->rowCount();
    for(int i=0; i<row_num; i++){
        k = ui->hyperParamKVTable->item(i, 0)->text();
        v = ui->hyperParamKVTable->item(i, 1)->text();
        assert(!result.contains(k));
        result.insert(k, v);
    }
    return result;
}

QMap<QString, QString> AddResultDialog::getRInput(){
    QMap<QString, QString> result;
    QString k, v;
    int row_num = ui->resultKVTable->rowCount();
    for(int i=0; i<row_num; i++){
        k = ui->resultKVTable->item(i, 0)->text();
        v = ui->resultKVTable->item(i, 1)->text();
        assert(!result.contains(k));
        result.insert(k, v);
    }
    return result;
}

QString AddResultDialog::getConfigPathInput(){
    return ui->configPathEdit->text();
}

QString AddResultDialog::getLogPathInput(){
    return ui->logPathEdit->text();
}
