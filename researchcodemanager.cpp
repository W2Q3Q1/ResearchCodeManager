#include "researchcodemanager.h"
#include "./ui_researchcodemanager.h"
#include "codemanager.h"
#include <QDebug>
#include <QList>
#include <QVector>
#include <QVariant>
#include <QTreeWidgetItem>
#include <QPair>
#include <QAction>
#include <QCursor>
#include "addresearchdialog.h"
#include <QDialog>
#include <QMessageBox>
#include "safedialog.h"
#include "addcodedialog.h"
#include <QDir>
#include <QFileDialog>
#include "addresultdialog.h"

void ResearchCodeManager::refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType checked_type, int checked_idx){
    ui->researchTree->clear();
    CodeManager *manager = CodeManager::getSelf();
    QList<QVector<QVariant>> research_infos = manager->getAllResearchInfo();
    QTreeWidgetItem *checked_item = nullptr;
    for(const QVector<QVariant> &research_info: research_infos){
        int research_idx = research_info[0].toInt();
        QString research_name = research_info[1].toString();
        QTreeWidgetItem *research_widget_item = new QTreeWidgetItem(ResearchCodeManager::ResearchTreeType::RESEARCH);
        research_widget_item->setData(0, ResearchCodeManager::ResearchDataType::RESEARCH_IDX, research_info[0]);
        research_widget_item->setData(0, ResearchCodeManager::ResearchDataType::RESEARCH_NAME, research_info[1]);
        research_widget_item->setData(0, ResearchCodeManager::ResearchDataType::RESEARCH_TIME, research_info[2]);
        research_widget_item->setData(0, ResearchCodeManager::ResearchDataType::RESEARCH_INFORMATION, research_info[3]);
        research_widget_item->setText(0, research_name);
        QList<QVector<QVariant>> code_infos = manager->getAllCodeInfoOfResearch(research_idx);
        for(const QVector<QVariant> &code_info: code_infos){
            int code_idx = code_info[0].toInt();
            QString code_name = code_info[1].toString();
            QTreeWidgetItem *code_widget_item = new QTreeWidgetItem(ResearchCodeManager::ResearchTreeType::CODE);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_IDX, code_info[0]);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_NAME, code_info[1]);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_TIME, code_info[2]);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_INFORMATION, code_info[3]);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_SYSTEMTYPE, code_info[4]);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_HAS_FILE, code_info[5]);
            code_widget_item->setData(0, ResearchCodeManager::CodeDataType::CODE_RESEARCH_IDX, research_idx);
            code_widget_item->setText(0, code_name);
            QList<QVector<QVariant>> result_infos = manager->getAllResultInfoOfCode(code_idx);
            for(const QVector<QVariant> &result_info: result_infos){
                int result_idx = result_info[0].toInt();
                QString result_name = result_info[1].toString();
                QTreeWidgetItem *result_widget_item = new QTreeWidgetItem(ResearchCodeManager::ResearchTreeType::RESULT);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_IDX, result_info[0]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_NAME, result_info[1]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_TIME, result_info[2]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_HAS_CONFIG_FILE, result_info[3]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_HAS_LOG_FILE, result_info[4]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_HYPER_PARAMETER_KV, result_info[5]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_RESULT_KV, result_info[6]);
                result_widget_item->setData(0, ResearchCodeManager::ResultDataType::RESULT_CODE_IDX, code_idx);
                result_widget_item->setText(0, result_name);
                code_widget_item->addChild(result_widget_item);
                if(checked_type == ResearchCodeManager::ResearchTreeType::RESULT && checked_idx == result_idx){
                    checked_item = result_widget_item;
                }
            }
            research_widget_item->addChild(code_widget_item);
            if(checked_type == ResearchCodeManager::ResearchTreeType::CODE && checked_idx == code_idx){
                checked_item = code_widget_item;
            }
        }
        ui->researchTree->addTopLevelItem(research_widget_item);
        //如果没有相等的，则保证第一个会被选中
        if(checked_item == nullptr || (checked_type == ResearchCodeManager::ResearchTreeType::RESEARCH && checked_idx == research_idx)){
            checked_item = research_widget_item;
        }
    }
    if(checked_item != nullptr){
        ui->researchTree->setCurrentItem(checked_item);
    }else{
        this->researchTreeCurrentItemChanged(nullptr, nullptr);
    }
}

void ResearchCodeManager::showEvent(QShowEvent *event){
    QMainWindow::showEvent(event);
    this->refreshResearchTreeWidget(this->checked_type_before_hide, this->checked_idx_before_hide);
}

void ResearchCodeManager::hideEvent(QHideEvent *event){
    QTreeWidgetItem *current_item = ui->researchTree->currentItem();
    if(current_item == nullptr){
        this->checked_type_before_hide = ResearchCodeManager::ResearchTreeType::NONETYPE;
    }else{
        this->checked_type_before_hide = static_cast<ResearchCodeManager::ResearchTreeType>(current_item->type());
        switch(current_item->type()){
        case ResearchCodeManager::ResearchTreeType::RESEARCH:
            this->checked_idx_before_hide = current_item->data(0, ResearchCodeManager::ResearchDataType::RESEARCH_IDX).toInt();
            break;
        case ResearchCodeManager::ResearchTreeType::CODE:
            this->checked_idx_before_hide = current_item->data(0, ResearchCodeManager::CodeDataType::CODE_IDX).toInt();
            break;
        case ResearchCodeManager::ResearchTreeType::RESULT:
            this->checked_idx_before_hide = current_item->data(0, ResearchCodeManager::ResultDataType::RESULT_IDX).toInt();
            break;
        default:
            assert(false);
        }
    }
    QMainWindow::hideEvent(event);
}

void ResearchCodeManager::showResearchTreeWidgetMenu(const QPoint &pos){
    QTreeWidgetItem *current_item = ui->researchTree->itemAt(pos);
    if(current_item == nullptr){
        //空白处菜单
        //空白菜单不需要传递被点击对象，但是仍然要填入一个nullptr
        this->addResearchAction->setData(QVariant::fromValue(nullptr));
        this->whiteSpaceMenu->exec(QCursor::pos());
    }else if(current_item->type() == ResearchCodeManager::ResearchTreeType::RESEARCH){
        this->addCodeAction->setData(QVariant::fromValue(current_item));
        this->deleteResearchAction->setData(QVariant::fromValue(current_item));
        this->researchItemMenu->exec(QCursor::pos());
    }else if(current_item->type() == ResearchCodeManager::ResearchTreeType::CODE){
        this->addResultAction->setData(QVariant::fromValue(current_item));
        this->deleteCodeAction->setData(QVariant::fromValue(current_item));
        this->codeItemMenu->exec(QCursor::pos());
    }else if(current_item->type() == ResearchCodeManager::ResearchTreeType::RESULT){
        this->deleteResultAction->setData(QVariant::fromValue(current_item));
        this->resultItemMenu->exec(QCursor::pos());
    }else{
        assert(false);
    }
}

void ResearchCodeManager::addResearchTriggered(){
    AddResearchDialog dialog;
    int user_choice = dialog.exec();
    CodeManager *manager = CodeManager::getSelf();
    if(user_choice == QDialog::Accepted){
        QString name = dialog.getNameInput();
        QString information = dialog.getInformationInput();
        int research_idx = manager->addResearchInfo(name, information);
        this->refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType::RESEARCH, research_idx);
    }else{
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
    }
}

void ResearchCodeManager::deleteResearchTriggered(){
    QTreeWidgetItem *current_item = this->deleteResearchAction->data().value<QTreeWidgetItem*>();
    if(current_item->childCount()!=0){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Code Exists"));
        return;
    }
    int user_choice;
    for(int i=0; i<3; i++){
        user_choice = SafeDialog(tr("Delete Confirm")).exec();
        if(user_choice == QDialog::Rejected){
            QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
            return;
        }
    }
    int research_idx = current_item->data(0, ResearchCodeManager::ResearchDataType::RESEARCH_IDX).toInt();
    CodeManager::getSelf()->deleteResearchInfo(research_idx);
    this->refreshResearchTreeWidget();
}

void ResearchCodeManager::addCodeTriggered(){
    CodeManager *manager = CodeManager::getSelf();
    QTreeWidgetItem *current_item = this->addCodeAction->data().value<QTreeWidgetItem*>();
    int research_idx = current_item->data(0, ResearchCodeManager::ResearchDataType::RESEARCH_IDX).toInt();
    AddCodeDialog dialog(research_idx);
    int user_choice = dialog.exec();
    if(user_choice == QDialog::Accepted){
        QString name = dialog.getNameInput();
        QString path = dialog.getPathInput();
        QString information = dialog.getInformationInput();
        int code_idx = manager->addCodeInfo(name, information, research_idx);
        if(!path.isEmpty()){
            manager->addCodeFile(path, code_idx);
        }
        this->refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType::CODE, code_idx);
    }else{
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
    }
}

void ResearchCodeManager::deleteCodeTriggered(){
    QTreeWidgetItem *current_item = this->deleteCodeAction->data().value<QTreeWidgetItem*>();
    if(current_item->childCount()!=0){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Result Exists"));
        return;
    }
    int user_choice;
    for(int i=0; i<3; i++){
        user_choice = SafeDialog(tr("Delete Confirm")).exec();
        if(user_choice == QDialog::Rejected){
            QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
            return;
        }
    }
    int code_idx = current_item->data(0, ResearchCodeManager::CodeDataType::CODE_IDX).toInt();
    //首先删除文件
    if(current_item->data(0, ResearchCodeManager::CodeDataType::CODE_HAS_FILE).toBool()){
        CodeManager::getSelf()->deleteCodeFile(code_idx);
    }
    CodeManager::getSelf()->deleteCodeInfo(code_idx);
    this->refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType::RESEARCH, current_item->data(0, ResearchCodeManager::CodeDataType::CODE_RESEARCH_IDX).toInt());
}

void ResearchCodeManager::addResultTriggered(){
    CodeManager *manager = CodeManager::getSelf();
    QTreeWidgetItem *current_item = this->addResultAction->data().value<QTreeWidgetItem*>();
    int code_idx = current_item->data(0, ResearchCodeManager::CodeDataType::CODE_IDX).toInt();
    AddResultDialog dialog(code_idx);
    int user_choice = dialog.exec();
    if(user_choice == QDialog::Accepted){
        QString name = dialog.getNameInput();
        QString config_path = dialog.getConfigPathInput();
        QString log_path = dialog.getLogPathInput();
        QMap<QString, QString> hp_kv = dialog.getHPInput();
        QMap<QString, QString> r_kv = dialog.getRInput();
        int result_idx = manager->addResultInfo(name, code_idx);
        if(!config_path.isEmpty()){
            manager->addConfigFile(config_path, result_idx);
        }
        if(!log_path.isEmpty()){
            manager->addLogFile(log_path, result_idx);
        }
        for(auto it = hp_kv.begin(); it!=hp_kv.end(); it++){
            manager->addHyperParameterKV(it.key(), it.value(), result_idx);
        }
        for(auto it = r_kv.begin(); it!=r_kv.end(); it++){
            manager->addResultKV(it.key(), it.value(), result_idx);
        }
        this->refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType::RESULT, result_idx);
    }else{
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
    }
}

void ResearchCodeManager::deleteResultTriggered(){
    int user_choice;
    for(int i=0; i<3; i++){
        user_choice = SafeDialog(tr("Delete Confirm")).exec();
        if(user_choice == QDialog::Rejected){
            QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
            return;
        }
    }
    CodeManager *manager = CodeManager::getSelf();
    QTreeWidgetItem *current = this->deleteResultAction->data().value<QTreeWidgetItem*>();
    int result_idx = current->data(0, ResearchCodeManager::ResultDataType::RESULT_IDX).toInt();
    //删除文件
    if(current->data(0, ResearchCodeManager::ResultDataType::RESULT_HAS_CONFIG_FILE).toBool()){
        manager->deleteConfigFile(result_idx);
    }
    if(current->data(0, ResearchCodeManager::ResultDataType::RESULT_HAS_LOG_FILE).toBool()){
        manager->deleteLogFile(result_idx);
    }
    //删除KV
    manager->deleteAllHyperParameterKVOfResult(result_idx);
    manager->deleteAllResultKVOfResult(result_idx);
    //删除result
    manager->deleteResultInfo(result_idx);
    //更新
    this->refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType::CODE, current->data(0, ResearchCodeManager::ResultDataType::RESULT_CODE_IDX).toInt());
}

void ResearchCodeManager::refreshResearchTreeWidget(){
    this->refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType::NONETYPE, -1);
}

void ResearchCodeManager::exportCodeClicked(){
    QString path = QFileDialog::getExistingDirectory(this, tr("Open"));
    CodeManager *manager = CodeManager::getSelf();
    int code_idx = ui->researchTree->currentItem()->data(0, ResearchCodeManager::CodeDataType::CODE_IDX).toInt();
    if(path.isEmpty()){
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("User Canceled"));
    }else if(!QDir(path).isEmpty()){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Directory Not Empty"));
    }else if(!manager->codeOnSystem(code_idx)){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Code From Different System"));
    }else{
        manager->exportCodeFile(code_idx, path);
    }
}

ResearchCodeManager::ResearchCodeManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ResearchCodeManager)
    , checked_type_before_hide(ResearchCodeManager::ResearchTreeType::NONETYPE)
    , checked_idx_before_hide(-1)
{
    ui->setupUi(this);

    //创建对象
    this->addResearchAction = new QAction(tr("Add Research"), this);
    this->deleteResearchAction = new QAction(tr("Delete Research"), this);
    this->addCodeAction = new QAction(tr("Add Code"), this);
    this->deleteCodeAction = new QAction(tr("Delete Code"), this);
    this->addResultAction = new QAction(tr("Add Result"), this);
    this->deleteResultAction = new QAction(tr("Delete Result"), this);
    this->refreshResearchTreeAction = new QAction(tr("Refresh"), this);

    //初始化组件
    //初始化四个菜单，使用不含Data的Action
    this->whiteSpaceMenu = new QMenu(this);
    this->whiteSpaceMenu->addAction(this->addResearchAction);
    this->whiteSpaceMenu->addAction(this->refreshResearchTreeAction);
    this->researchItemMenu = new QMenu(this);
    this->researchItemMenu->addAction(this->addCodeAction);
    this->researchItemMenu->addAction(this->deleteResearchAction);
    this->researchItemMenu->addAction(this->refreshResearchTreeAction);
    this->codeItemMenu = new QMenu(this);
    this->codeItemMenu->addAction(this->addResultAction);
    this->codeItemMenu->addAction(this->deleteCodeAction);
    this->codeItemMenu->addAction(this->refreshResearchTreeAction);
    this->resultItemMenu = new QMenu(this);
    this->resultItemMenu->addAction(this->deleteResultAction);
    this->resultItemMenu->addAction(this->refreshResearchTreeAction);

    //绑定事件
    //researchTreeWidget选中切换
    void (QTreeWidget::*research_tree_widget_currentItemChanged)(QTreeWidgetItem*, QTreeWidgetItem*) = &QTreeWidget::currentItemChanged;
    void (ResearchCodeManager::*research_code_manager_researchTreeCurrentItemChanged)(QTreeWidgetItem*, QTreeWidgetItem*) = &ResearchCodeManager::researchTreeCurrentItemChanged;
    connect(ui->researchTree, research_tree_widget_currentItemChanged, this, research_code_manager_researchTreeCurrentItemChanged);
    //researchTreeWidget右键菜单
    void (QTreeWidget::*research_tree_widget_customContextMenuRequested)(const QPoint&) = &QTreeWidget::customContextMenuRequested;
    void (ResearchCodeManager::*research_code_manager_showResearchTreeWidgetMenu) (const QPoint&) = &ResearchCodeManager::showResearchTreeWidgetMenu;
    connect(ui->researchTree, research_tree_widget_customContextMenuRequested, this, research_code_manager_showResearchTreeWidgetMenu);
    //add/delete菜单项事件
    void (QAction::*actions_triggered) (bool) = &QAction::triggered;
    void (ResearchCodeManager::*research_code_manager_addResearchTriggered) () = &ResearchCodeManager::addResearchTriggered;
    void (ResearchCodeManager::*research_code_manager_deleteResearchTriggered) () = &ResearchCodeManager::deleteResearchTriggered;
    void (ResearchCodeManager::*research_code_manager_addCodeTriggered) () = &ResearchCodeManager::addCodeTriggered;
    void (ResearchCodeManager::*research_code_manager_deleteCodeTriggered) () = &ResearchCodeManager::deleteCodeTriggered;
    void (ResearchCodeManager::*research_code_manager_addResultTriggered) () = &ResearchCodeManager::addResultTriggered;
    void (ResearchCodeManager::*research_code_manager_deleteResultTriggered) () = &ResearchCodeManager::deleteResultTriggered;
    void (ResearchCodeManager::*research_code_manager_refreshResearchTreeWidget) () = &ResearchCodeManager::refreshResearchTreeWidget;
    connect(this->addResearchAction, actions_triggered, this, research_code_manager_addResearchTriggered);
    connect(this->deleteResearchAction, actions_triggered, this, research_code_manager_deleteResearchTriggered);
    connect(this->addCodeAction, actions_triggered, this, research_code_manager_addCodeTriggered);
    connect(this->deleteCodeAction, actions_triggered, this, research_code_manager_deleteCodeTriggered);
    connect(this->addResultAction, actions_triggered, this, research_code_manager_addResultTriggered);
    connect(this->deleteResultAction, actions_triggered, this, research_code_manager_deleteResultTriggered);
    connect(this->refreshResearchTreeAction, actions_triggered, this, research_code_manager_refreshResearchTreeWidget);
    //export事件
    void (QPushButton::*push_buttons_clicked) (bool) = &QPushButton::clicked;
    void (ResearchCodeManager::*research_code_manager_exportCodeClicked) () = &ResearchCodeManager::exportCodeClicked;
    connect(ui->codeExportButton, push_buttons_clicked, this, research_code_manager_exportCodeClicked);
}

ResearchCodeManager::~ResearchCodeManager(){
    delete ui;
}

void ResearchCodeManager::researchTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous){
    qDebug() << ((previous)?previous->text(0):"None") << " -> " << (current == nullptr?"None":current->text(0)) << "\n";
    if(current == nullptr){
        ui->stackedWidget->setCurrentWidget(ui->researchPage);
        ui->researchNameLabel->setText("");
        ui->researchTimeLabel->setText("");
        ui->researchInformationLabel->setPlainText("");
    }else{
        switch(current->type()){
        case ResearchCodeManager::ResearchTreeType::RESEARCH:
            ui->stackedWidget->setCurrentWidget(ui->researchPage);
            ui->researchNameLabel->setText(current->data(0, ResearchCodeManager::ResearchDataType::RESEARCH_NAME).toString());
            ui->researchTimeLabel->setText(current->data(0, ResearchCodeManager::ResearchDataType::RESEARCH_TIME).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            ui->researchInformationLabel->setPlainText(current->data(0, ResearchCodeManager::ResearchDataType::RESEARCH_INFORMATION).toString());
            break;
        case ResearchCodeManager::ResearchTreeType::CODE:
            ui->stackedWidget->setCurrentWidget(ui->codePage);
            ui->codeNameLabel->setText(current->data(0, ResearchCodeManager::CodeDataType::CODE_NAME).toString());
            ui->codeTimeLabel->setText(current->data(0, ResearchCodeManager::CodeDataType::CODE_TIME).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            ui->codeInformationLabel->setPlainText(current->data(0, ResearchCodeManager::CodeDataType::CODE_INFORMATION).toString());
            ui->codeExportButton->setEnabled(current->data(0, ResearchCodeManager::CodeDataType::CODE_HAS_FILE).toBool());
            break;
        case ResearchCodeManager::ResearchTreeType::RESULT:
            ui->stackedWidget->setCurrentWidget(ui->resultPage);
            ui->resultNameLabel->setText(current->data(0, ResearchCodeManager::ResultDataType::RESULT_NAME).toString());
            ui->resultTimeLabel->setText(current->data(0, ResearchCodeManager::ResultDataType::RESULT_TIME).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            ui->configExportButton->setEnabled(current->data(0, ResearchCodeManager::ResultDataType::RESULT_HAS_CONFIG_FILE).toBool());
            ui->logExportButton->setEnabled(current->data(0, ResearchCodeManager::ResultDataType::RESULT_HAS_LOG_FILE).toBool());
            ui->hyperParamKVTable->setRowCount(0);
            ui->resultKVTable->setRowCount(0);
            //hyper-parameter和result-kv
            QMap<QString, QString> hp_kv_map = current->data(0, ResearchCodeManager::ResultDataType::RESULT_HYPER_PARAMETER_KV).value<QMap<QString, QString>>();
            QMap<QString, QString> re_kv_map = current->data(0, ResearchCodeManager::ResultDataType::RESULT_RESULT_KV).value<QMap<QString, QString>>();
            for(auto it = hp_kv_map.begin(); it != hp_kv_map.end(); it++){
                int now_row_count = ui->hyperParamKVTable->rowCount();
                ui->hyperParamKVTable->insertRow(now_row_count);
                ui->hyperParamKVTable->setItem(now_row_count, 0, new QTableWidgetItem(it.key()));
                ui->hyperParamKVTable->setItem(now_row_count, 1, new QTableWidgetItem(it.value()));
            }
            for(auto it = re_kv_map.begin(); it != re_kv_map.end(); it++){
                int now_row_count = ui->resultKVTable->rowCount();
                ui->resultKVTable->insertRow(now_row_count);
                ui->resultKVTable->setItem(now_row_count, 0, new QTableWidgetItem(it.key()));
                ui->resultKVTable->setItem(now_row_count, 1, new QTableWidgetItem(it.value()));
            }
            break;
        }
    }
}

