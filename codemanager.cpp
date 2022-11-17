#include "codemanager.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <assert.h>
#include <QByteArray>
#include <QApplication>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStack>
#include <QSet>
#include <QFileInfoList>
#include <QDebug>
#include <QSqlError>
#include <QFileDevice>
#include <QMap>

CodeManager* CodeManager::self = nullptr;

QString CodeManager::fileSHA512(const QString &file_path){
    assert(QFileInfo(file_path).isFile());
    QFile theFile(file_path);
    theFile.open(QIODevice::ReadOnly);
    QByteArray hashed_array = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Sha512);
    theFile.close();
    return hashed_array.toHex().constData();
}

bool CodeManager::isSameFile(const QString &a_path, const QString &b_path){
    //此函数使用传统方法，即逐字节比对
    //打开错误将导致判断为文件不一致
    //此函数假定文件存在
    assert(QFileInfo(a_path).isFile() && QFileInfo(b_path).isFile());
    if(a_path == b_path){
        return true;
    }
    QFile a_file(a_path);
    QFile b_file(b_path);
    if(a_file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly) && b_file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
        QByteArray a_content = a_file.readAll();
        QByteArray b_content = b_file.readAll();
        a_file.close();
        b_file.close();
        if(a_content.length() != b_content.length()){
            return false;
        }
        auto a_it = a_content.constBegin();
        auto b_it = b_content.constBegin();
        for(; a_it != a_content.constEnd() && b_it != b_content.constEnd(); a_it++, b_it++){
            if(*a_it != *b_it){
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}

bool CodeManager::researchIDExist(int research_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResearchInfo] where [ID] = %1").arg(QString::number(research_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::researchNameExist(const QString &research_name){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResearchInfo] where [Name] = \'%1\'").arg(research_name)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::researchIDCodeNameExist(int research_idx, const QString &code_name){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [CodeInfo] where [ResearchID] = %1 and [Name] = \'%2\'").arg(QString::number(research_idx), code_name)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::codeOnSystem(int code_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select [SystemType] from [CodeInfo] where [ID] = %1").arg(QString::number(code_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() == this->systemType);
    sql_result.clear();
    return result;
}

bool CodeManager::researchHasCode(int research_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [CodeInfo] where [ResearchID] = %1").arg(QString::number(research_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::codeIDCodeFileSourceExist(int code_idx, const QString &file_source){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [CodeFileInfo] where [CodeID] = %1 and [Source] = \'%2\'").arg(QString::number(code_idx), file_source)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::codeHasFile(int code_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [CodeFileInfo] where [CodeID] = %1").arg(QString::number(code_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::codeHasResult(int code_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResultInfo] where [CodeID] = %1").arg(QString::number(code_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::codeIDExist(int code_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [CodeInfo] where [ID] = %1").arg(QString::number(code_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::codeIDResultNameExist(int code_idx, const QString &result_name){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResultInfo] where [CodeID] = %1 and [Name] = \'%2\'").arg(QString::number(code_idx), result_name)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultIDExist(int result_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResultInfo] where [ID] = %1").arg(QString::number(result_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultHasConfigFile(int result_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ConfigFileInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultHasLogFile(int result_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [LogFileInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultHasHyperParameter(int result_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [HyperParamInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultHasResultKV(int result_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResultKVInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultIDConfigFileSourceExist(int result_idx, const QString &file_source){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ConfigFileInfo] where [ResultID] = %1 and [Source] = \'%2\'").arg(QString::number(result_idx), file_source)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultIDLogFileSourceExist(int result_idx, const QString &file_source){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [LogFileInfo] where [ResultID] = %1 and [Source] = \'%2\'").arg(QString::number(result_idx), file_source)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultHyperParameterHasKey(int result_idx, const QString &key){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [HyperParamInfo] where [ResultID] = %1 and [Key] = \'%2\'").arg(QString::number(result_idx), key)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::hyperParameterIDExist(int param_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [HyperParamInfo] where [ID] = %1").arg(QString::number(param_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultResultHasKey(int result_idx, const QString &key){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResultKVInfo] where [ResultID] = %1 and [Key] = \'%2\'").arg(QString::number(result_idx), key)
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

bool CodeManager::resultKVIDExist(int result_kv_idx){
    QSqlQuery sql_result = this->database.exec(
        QString("select count(*) from [ResultKVInfo] where [ID] = %1").arg(QString::number(result_kv_idx))
    );
    sql_result.next();
    bool result = (sql_result.value(0).toInt() > 0);
    sql_result.clear();
    return result;
}

int CodeManager::getTableIncrementID(const QString &table_name){
    QSqlQuery sql_result = this->database.exec(
        QString("select max([ID]) from [%1]").arg(table_name)
    );
    sql_result.next();
    int result = sql_result.value(0).toInt()+1;
    sql_result.clear();
    return result;
}

QMap<QString, QString> CodeManager::copyToCodePath(const QString &source){
    assert(QFileInfo(source).exists());
    QDir related_path(QFileInfo(source).isDir()?source:QFileInfo(source).dir().absolutePath()); //相对路劲是本体文件夹，或者是文件所在文件夹
    QMap<QString, QString> result; //前者为相对source的路径，后者为相对codePath的路径
    QStack<QString> stack_for_dfs; //存储的应为绝对路径
    stack_for_dfs.push(source);
    //计算文件sha512
    while(!stack_for_dfs.empty()){
        QString now_path = stack_for_dfs.top();
        stack_for_dfs.pop();
        if(QFileInfo(now_path).isDir()){
            //需要将其下面的每一个文件和文件夹添加到stack_for_dfs
            QDir now_dir(now_path);
            now_dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
            QFileInfoList sub_path_list = now_dir.entryInfoList();
            for(const QFileInfo &sub_path: sub_path_list){
                stack_for_dfs.push(sub_path.absoluteFilePath());
            }
            if(now_path != source){
                //如果source是文件夹，则结果中不包含此
                result.insert(
                    related_path.relativeFilePath(now_path),
                    QString()
                );
            }
        }else if(QFileInfo(now_path).isFile()){
            QString sha512 = fileSHA512(now_path);
            //遍历每一个可能的文件，判断是否相同
            int file_idx = -1;
            QString target_file_name, target_file_path;
            do{
                file_idx++;
                target_file_name = sha512 + QString("_") + QString::number(file_idx);
                target_file_path = this->codePath.absoluteFilePath(target_file_name);
            }while(QFileInfo(target_file_path).isFile() && !isSameFile(now_path, target_file_path));
            //判断now_file_path是否存在
            //如不存在，则复制
            //存在则直接插入
            result.insert(
                related_path.relativeFilePath(now_path),
                target_file_name
            );
            if(QFileInfo(target_file_path).isFile()){
                ;
            }else{
                QFile(now_path).copy(target_file_path);
            }
        }else{
            assert(false);
        }
    }
    return result;
}

void CodeManager::delUselessFile(){
    //搜索codePath
    QFileInfoList cached_files = this->codePath.entryInfoList(QDir::NoDotAndDotDot);
    for(const QFileInfo &file_info: cached_files){
        //此处需要检查多个位置
        //测试代码
        QSqlQuery sql_result = this->database.exec(
            QString("select count(*) from [CodeFileInfo] where [Destination] =\'%1\' ").arg(file_info.fileName())
        );
        sql_result.next();
        int refers = sql_result.value(0).toInt();
        sql_result.clear();
        if(refers > 0){
            continue;
        }
        //测试配置文件
        sql_result = this->database.exec(
            QString("select count(*) from [ConfigFileInfo] where [Destination] =\'%1\' ").arg(file_info.fileName())
        );
        sql_result.next();
        refers = sql_result.value(0).toInt();
        sql_result.clear();
        if(refers > 0){
            continue;
        }
        //测试Log
        sql_result = this->database.exec(
            QString("select count(*) from [LogFileInfo] where [Destination] =\'%1\' ").arg(file_info.fileName())
        );
        sql_result.next();
        refers = sql_result.value(0).toInt();
        sql_result.clear();
        if(refers > 0){
            continue;
        }
        //删除文件
        QFile(file_info.absoluteFilePath()).remove();
    }
}

CodeManager::CodeManager():
#ifdef Q_OS_WINDOWS
    systemType(0)
#endif
#ifdef Q_OS_LINUX
  systemType(1)
#endif
{
    this->codePath = QDir(QApplication::applicationDirPath());
    if(!this->codePath.exists("code_cache")){
        this->codePath.mkdir("code_cache");
    }
    this->codePath.cd("code_cache");
    if(!this->codePath.exists("file_db.sqlite")){
        QFile(":/file/file_db.sqlite").copy(this->codePath.absoluteFilePath("file_db.sqlite"));
        QFile(this->codePath.absoluteFilePath("file_db.sqlite")).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    }
    this->database = QSqlDatabase::addDatabase("QSQLITE", "code_cache_database");
    this->database.setDatabaseName(this->codePath.absoluteFilePath("file_db.sqlite"));
    this->database.open();
}

CodeManager::~CodeManager(){
    this->database.close();
}

CodeManager *CodeManager::getSelf(){
    if(CodeManager::self == nullptr){
        CodeManager::self = new CodeManager();
    }
    return CodeManager::self;
}

void CodeManager::closeSelf(){
    if(CodeManager::self != nullptr){
        delete CodeManager::self;
    }
    CodeManager::self = nullptr;
}

int CodeManager::addResearchInfo(const QString &name, const QString &information){
    assert(!this->researchNameExist(name));
    int research_idx = this->getTableIncrementID("ResearchInfo");
    QString current_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    this->database.transaction();
    this->database.exec(
        QString("insert into [ResearchInfo]([ID], [Time], [Information], [Name]) values(%1, \'%2\', \'%3\', \'%4\')").arg(QString::number(research_idx), current_time, information, name)
    );
    this->database.commit();
    return research_idx;
}

void CodeManager::deleteResearchInfo(int research_idx){
    assert(this->researchIDExist(research_idx));
    assert(!this->researchHasCode(research_idx));
    this->database.transaction();
    this->database.exec(
        QString("delete from [ResearchInfo] where [ID] = %1").arg(QString::number(research_idx))
    );
    this->database.commit();
}

int CodeManager::addCodeInfo(const QString &name, const QString &information, int research_idx){
    assert(CodeManager::researchIDExist(research_idx));
    //确定research_idx, name的组合不存在
    assert(!CodeManager::researchIDCodeNameExist(research_idx, name));
    //添加CodeInfo
    QString current_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int code_idx = this->getTableIncrementID("CodeInfo");
    this->database.transaction();
    this->database.exec(
        QString("insert into [CodeInfo]([ID], [Name], [Time], [Information], [ResearchID], [SystemType]) values(%1, \'%2\', \'%3\', \'%4\', %5, %6)").arg(QString::number(code_idx), name, current_time, information, QString::number(research_idx), QString::number(this->systemType))
    );
    this->database.commit();
    return code_idx;
}

void CodeManager::addCodeFile(const QString &source, int code_idx){
    assert(this->codeIDExist(code_idx));
    assert(!this->codeHasFile(code_idx));
    //复制文件
    QMap<QString, QString> source_target_map = this->copyToCodePath(source);
    //起始ID
    int start_code_file_idx = this->getTableIncrementID("CodeFileInfo");
    this->database.transaction();
    auto source_target_ptr = source_target_map.constBegin();
    for(; source_target_ptr != source_target_map.constEnd(); source_target_ptr++, start_code_file_idx++){
        QString source = source_target_ptr.key();
        QString target = source_target_ptr.value();
        assert(!this->codeIDCodeFileSourceExist(code_idx, source));
        if(target.isNull()){
            //文件夹
            qDebug() << QString("insert into [CodeFileInfo]([ID], [CodeID], [Source], [Destination], [FileType]) values(%1, %2, \'%3\', \'%4\', %5)").arg(QString::number(start_code_file_idx), QString::number(code_idx), source, target, QString::number(CodeManager::FileType::DIRECTORY)) << "\n";
            this->database.exec(
                QString("insert into [CodeFileInfo]([ID], [CodeID], [Source], [Destination], [FileType]) values(%1, %2, \'%3\', \'%4\', %5)").arg(QString::number(start_code_file_idx), QString::number(code_idx), source, target, QString::number(CodeManager::FileType::DIRECTORY))
            );
            qDebug() << this->database.lastError().text() << "\n";
        }else{
            qDebug() << QString("insert into [CodeFileInfo]([ID], [CodeID], [Source], [Destination], [FileType]) values(%1, %2, \'%3\', \'%4\', %5)").arg(QString::number(start_code_file_idx), QString::number(code_idx), source, target, QString::number(CodeManager::FileType::FILE)) << "\n";
            this->database.exec(
                QString("insert into [CodeFileInfo]([ID], [CodeID], [Source], [Destination], [FileType]) values(%1, %2, \'%3\', \'%4\', %5)").arg(QString::number(start_code_file_idx), QString::number(code_idx), source, target, QString::number(CodeManager::FileType::FILE))
            );
            qDebug() << this->database.lastError().text() << "\n";
        }
    }
    this->database.commit();
}

void CodeManager::exportCodeFile(int code_idx, const QString &destination_path){
    assert(this->codeIDExist(code_idx));
    assert(QFileInfo(destination_path).isDir());
    assert(QDir(destination_path).isEmpty());
    assert(this->codeOnSystem(code_idx));

    QDir destination_dir(destination_path);
    QSqlQuery sql_result = this->database.exec(
        QString("select [Source], [Destination], [FileType] from [CodeFileInfo] where [CodeID] = %1").arg(QString::number(code_idx))
    );
    while(sql_result.next()){
        QString source = QDir(destination_path).absoluteFilePath(sql_result.value(0).toString());
        QString target = this->codePath.absoluteFilePath(sql_result.value(1).toString());
        CodeManager::FileType file_type = static_cast<CodeManager::FileType>(sql_result.value(2).toInt());
        if(file_type == CodeManager::FileType::DIRECTORY){
            if(!QFileInfo(source).isDir()){
                destination_dir.mkpath(source);
            }
        }else{
            //判断其父文件夹是否存在
            if(!QFileInfo(source).dir().exists()){
                destination_dir.mkpath(QFileInfo(source).dir().absolutePath());
            }
            //复制
            QFile(target).copy(source);
        }
    }
    sql_result.clear();
}

void CodeManager::deleteCodeInfo(int code_idx){
    assert(this->codeIDExist(code_idx));
    assert(!this->codeHasFile(code_idx));
    assert(!this->codeHasResult(code_idx));
    //删除数据库中信息
    this->database.transaction();
    this->database.exec(
        QString("delete from [CodeInfo] where [ID] = %1").arg(code_idx)
    );
    this->database.commit();
}

int CodeManager::addResultInfo(const QString &name, int code_idx){
    assert(this->codeIDExist(code_idx));
    //判断code_idx和name的组合不存在
    assert(!this->codeIDResultNameExist(code_idx, name));
    //添加ResultInfo
    QString current_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int result_idx = this->getTableIncrementID("ResultInfo");
    this->database.transaction();
    this->database.exec(
        QString("insert into [ResultInfo]([ID], [Name], [Time], [CodeID]) values(%1, \'%2\', \'%3\', %4)").arg(QString::number(result_idx), name, current_time, QString::number(code_idx))
    );
    this->database.commit();
    return result_idx;
}

void CodeManager::addConfigFile(const QString &source, int result_idx){
    assert(this->resultIDExist(result_idx));
    assert(!this->resultHasConfigFile(result_idx));
    //复制文件
    QMap<QString, QString> source_target_map = this->copyToCodePath(source);
    //起始ID
    int start_config_file_idx = this->getTableIncrementID("ConfigFileInfo");
    this->database.transaction();
    auto source_target_ptr = source_target_map.constBegin();
    for(; source_target_ptr != source_target_map.constEnd(); source_target_ptr++, start_config_file_idx++){
        QString source = source_target_ptr.key();
        QString target = source_target_ptr.value();
        assert(!this->resultIDConfigFileSourceExist(result_idx, source));
        if(target.isNull()){
            //文件夹
            qDebug() << QString("insert into [ConfigFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::DIRECTORY)) << "\n";
            this->database.exec(
                QString("insert into [ConfigFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::DIRECTORY))
            );
            qDebug() << this->database.lastError().text() << "\n";
        }else{
            qDebug() << QString("insert into [ConfigFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::FILE)) << "\n";
            this->database.exec(
                QString("insert into [ConfigFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::FILE))
            );
            qDebug() << this->database.lastError().text() << "\n";
        }
    }
    this->database.commit();
}

void CodeManager::exportConfigFile(int result_idx, const QString &destination_path){
    assert(this->resultIDExist(result_idx));
    assert(QFileInfo(destination_path).isDir());
    assert(QDir(destination_path).isEmpty());

    QDir destination_dir(destination_path);
    QSqlQuery sql_result = this->database.exec(
        QString("select [Source], [Destination], [FileType] from [ConfigFileInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    while(sql_result.next()){
        QString source = QDir(destination_path).absoluteFilePath(sql_result.value(0).toString());
        QString target = this->codePath.absoluteFilePath(sql_result.value(1).toString());
        CodeManager::FileType file_type = static_cast<CodeManager::FileType>(sql_result.value(2).toInt());
        if(file_type == CodeManager::FileType::DIRECTORY){
            if(!QFileInfo(source).isDir()){
                destination_dir.mkpath(source);
            }
        }else{
            //判断其父文件夹是否存在
            if(!QFileInfo(source).dir().exists()){
                destination_dir.mkpath(QFileInfo(source).dir().absolutePath());
            }
            //复制
            QFile(target).copy(source);
        }
    }
    sql_result.clear();
}

void CodeManager::deleteConfigFile(int result_idx){
    assert(this->resultIDExist(result_idx));
    this->database.transaction();
    this->database.exec(
        QString("delete from [ConfigFileInfo] where [ResultID] = %1").arg(result_idx)
    );
    this->database.commit();
    //更新文件引用
    this->delUselessFile();
}

void CodeManager::addLogFile(const QString &source, int result_idx){
    assert(this->resultIDExist(result_idx));
    assert(!this->resultHasLogFile(result_idx));
    //复制文件
    QMap<QString, QString> source_target_map = this->copyToCodePath(source);
    //起始ID
    int start_config_file_idx = this->getTableIncrementID("LogFileInfo");
    this->database.transaction();
    auto source_target_ptr = source_target_map.constBegin();
    for(; source_target_ptr != source_target_map.constEnd(); source_target_ptr++, start_config_file_idx++){
        QString source = source_target_ptr.key();
        QString target = source_target_ptr.value();
        assert(!this->resultIDLogFileSourceExist(result_idx, source));
        if(target.isNull()){
            //文件夹
            qDebug() << QString("insert into [LogFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::DIRECTORY)) << "\n";
            this->database.exec(
                QString("insert into [LogFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::DIRECTORY))
            );
            qDebug() << this->database.lastError().text() << "\n";
        }else{
            qDebug() << QString("insert into [LogFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::FILE)) << "\n";
            this->database.exec(
                QString("insert into [LogFileInfo]([ID], [Source], [Destination], [ResultID], [FileType]) values(%1, \'%2\', \'%3\', %4, %5)").arg(QString::number(start_config_file_idx), source, target, QString::number(result_idx), QString::number(CodeManager::FileType::FILE))
            );
            qDebug() << this->database.lastError().text() << "\n";
        }
    }
    this->database.commit();
}

void CodeManager::exportLogFile(int result_idx, const QString &destination_path){
    assert(this->resultIDExist(result_idx));
    assert(QFileInfo(destination_path).isDir());
    assert(QDir(destination_path).isEmpty());

    QDir destination_dir(destination_path);
    QSqlQuery sql_result = this->database.exec(
        QString("select [Source], [Destination], [FileType] from [LogFileInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    while(sql_result.next()){
        QString source = QDir(destination_path).absoluteFilePath(sql_result.value(0).toString());
        QString target = this->codePath.absoluteFilePath(sql_result.value(1).toString());
        CodeManager::FileType file_type = static_cast<CodeManager::FileType>(sql_result.value(2).toInt());
        if(file_type == CodeManager::FileType::DIRECTORY){
            if(!QFileInfo(source).isDir()){
                destination_dir.mkpath(source);
            }
        }else{
            //判断其父文件夹是否存在
            if(!QFileInfo(source).dir().exists()){
                destination_dir.mkpath(QFileInfo(source).dir().absolutePath());
            }
            //复制
            QFile(target).copy(source);
        }
    }
    sql_result.clear();
}

void CodeManager::deleteLogFile(int result_idx){
    assert(this->resultIDExist(result_idx));
    this->database.transaction();
    this->database.exec(
        QString("delete from [LogFileInfo] where [ResultID] = %1").arg(result_idx)
    );
    this->database.commit();
    //更新文件引用
    this->delUselessFile();
}

int CodeManager::addHyperParameterKV(const QString &key, const QString &value, int result_idx){
    assert(this->resultIDExist(result_idx));
    assert(!this->resultHyperParameterHasKey(result_idx, key));

    int hyper_param_idx = this->getTableIncrementID("HyperParamInfo");
    this->database.transaction();
    this->database.exec(
        QString("insert into [HyperParamInfo]([ID], [Key], [Value], [ResultID]) values(%1, \'%2\', \'%3\', %4)").arg(QString::number(hyper_param_idx), key, value, QString::number(result_idx))
    );
    this->database.commit();
    return hyper_param_idx;
}

void CodeManager::deleteHyperParameterKV(int param_idx){
    assert(this->hyperParameterIDExist(param_idx));

    this->database.transaction();
    this->database.exec(
        QString("delete from [HyperParamInfo] where [ID] = %1").arg(QString::number(param_idx))
    );
    this->database.commit();
}

void CodeManager::deleteAllHyperParameterKVOfResult(int result_idx){
    assert(this->resultIDExist(result_idx));

    this->database.transaction();
    this->database.exec(
        QString("delete from [HyperParamInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    this->database.commit();
}

int CodeManager::addResultKV(const QString &key, const QString &value, int result_idx){
    assert(this->resultIDExist(result_idx));
    assert(!this->resultResultHasKey(result_idx, key));

    int result_kv_idx = this->getTableIncrementID("ResultKVInfo");
    this->database.transaction();
    this->database.exec(
        QString("insert into [ResultKVInfo]([ID], [Key], [Value], [ResultID]) values(%1, \'%2\', \'%3\', %4)").arg(QString::number(result_kv_idx), key, value, QString::number(result_idx))
    );
    this->database.commit();
    return result_kv_idx;
}

void CodeManager::deleteResultKV(int result_kv_idx){
    assert(this->resultKVIDExist(result_kv_idx));

    this->database.transaction();
    this->database.exec(
        QString("delete from [ResultKVInfo] where [ID] = %1").arg(QString::number(result_kv_idx))
    );
    this->database.commit();
}

void CodeManager::deleteAllResultKVOfResult(int result_idx){
    assert(this->resultIDExist(result_idx));

    this->database.transaction();
    this->database.exec(
        QString("delete from [ResultKVInfo] where [ResultID] = %1").arg(QString::number(result_idx))
    );
    this->database.commit();
}

void CodeManager::deleteResultInfo(int result_idx){
    assert(this->resultIDExist(result_idx));
    assert(!this->resultHasConfigFile(result_idx));
    assert(!this->resultHasLogFile(result_idx));
    assert(!this->resultHasHyperParameter(result_idx));
    assert(!this->resultHasResultKV(result_idx));
    //删除数据库中信息
    this->database.transaction();
    this->database.exec(
        QString("delete from [ResultInfo] where [ID] = %1").arg(result_idx)
    );
    this->database.commit();
}

QList<QVector<QVariant> > CodeManager::getAllResearchInfo(){
    QList<QVector<QVariant>> result;
    QSqlQuery sql_result = this->database.exec(
        QString("select [ID], [Name], [Time], [Information] from [ResearchInfo]")
    );
    while(sql_result.next()){
        QVector<QVariant> result_line(
            {
                sql_result.value(0),
                sql_result.value(1),
                sql_result.value(2),
                sql_result.value(3)
            }
        );
        result.push_back(result_line);
    }
    sql_result.clear();
    return result;
}

QList<QVector<QVariant> > CodeManager::getAllCodeInfoOfResearch(int research_idx){
    assert(this->researchIDExist(research_idx));
    QList<QVector<QVariant>> result;
    QSqlQuery sql_result = this->database.exec(
        QString("select [ID], [Name], [Time], [Information], [SystemType] from [CodeInfo] where [ResearchID] = %1").arg(QString::number(research_idx))
    );
    while(sql_result.next()){
        QVector<QVariant> result_line(
            {
                sql_result.value(0),
                sql_result.value(1),
                sql_result.value(2),
                sql_result.value(3),
                sql_result.value(4),
                this->codeHasFile(sql_result.value(0).toInt())
            }
        );
        result.push_back(result_line);
    }
    sql_result.clear();
    return result;
}

QList<QVector<QVariant> > CodeManager::getAllResultInfoOfCode(int code_idx){
    assert(this->codeIDExist(code_idx));
    QList<QVector<QVariant>> result;
    QSqlQuery sql_result = this->database.exec(
        QString("select [ID], [Name], [Time] from [ResultInfo] where [CodeID] = %1").arg(QString::number(code_idx))
    );
    while(sql_result.next()){
        int result_idx = sql_result.value(0).toInt();
        bool has_config = this->resultHasConfigFile(result_idx);
        bool has_log = this->resultHasLogFile(result_idx);
        QMap<QString, QString> hp_kv;
        QMap<QString, QString> result_kv;
        //查询hyper-parameter
        QSqlQuery hp_sql_result = this->database.exec(
            QString("select [Key], [Value] from [HyperParamInfo] where [ResultID] = %1").arg(QString::number(result_idx))
        );
        while(hp_sql_result.next()){
            hp_kv.insert(
                hp_sql_result.value(0).toString(),
                hp_sql_result.value(1).toString()
            );
        }
        hp_sql_result.clear();
        //查询result
        QSqlQuery result_sql_result = this->database.exec(
            QString("select [Key], [Value] from [ResultKVInfo] where [ResultID] = %1").arg(QString::number(result_idx))
        );
        while(result_sql_result.next()){
            result_kv.insert(
                result_sql_result.value(0).toString(),
                result_sql_result.value(1).toString()
            );
        }
        result_sql_result.clear();
        QVector<QVariant> result_line(
            {
                sql_result.value(0),
                sql_result.value(1),
                sql_result.value(2),
                has_config,
                has_log,
                QVariant::fromValue(hp_kv),
                QVariant::fromValue(result_kv)
            }
        );
        result.push_back(result_line);
    }
    sql_result.clear();
    return result;
}

void CodeManager::deleteCodeFile(int code_idx){
    assert(this->codeIDExist(code_idx));
    this->database.exec(
        QString("delete from [CodeFileInfo] where [CodeID] = %1").arg(code_idx)
    );
    //更新文件引用
    this->delUselessFile();
}
