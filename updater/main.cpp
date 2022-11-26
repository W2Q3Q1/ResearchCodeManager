#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QNetworkProxy>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QObject>
#include <QDebug>
#include <iostream>
#include "checkanddownload.h"
#include "overrideold.h"
#include "removenew.h"
#include <QSharedMemory>
#include <QMessageBox>
#include "enumeration.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //translate
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "updater_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    //parser
    QCommandLineParser parser;
    //升级类型——UpdateMe(0), OverrideOld(1), RemoveUpdate(2)
    //UpdateMe: 查询升级并下载升级包，解压，以OverrideOld调用升级包内Updater
    //OverrideOld: 用当前包内程序替换旧版本，根据旧版本版本号更新数据存储结构，并以RemoveUpdate重新调用（替换后的）安装位置Updater
    //RemoveUpdate: 删除升级包
    QCommandLineOption update_type(
        "UpdateType",
        QObject::tr("The Process Stage of Updater"),
        "UpdateTypeValue"
    );
    //Major版本，Minor版本和Fix版本号，当且仅当升级类型为OverrideOld时提供
    QCommandLineOption major_version(
        "MajorVersion",
        QObject::tr("Major Version of Old"),
        "MajorVersionValue"
    );
    QCommandLineOption minor_version(
        "MinorVersion",
        QObject::tr("Minor Version of Old"),
        "MinorVersion"
    );
    QCommandLineOption fix_version(
        "FixVersion",
        QObject::tr("Fix Version of Old"),
        "FixVersionValue"
    );
    parser.addOption(update_type);
    parser.addOption(major_version);
    parser.addOption(minor_version);
    parser.addOption(fix_version);
    parser.process(a);
    if(!parser.isSet(update_type)){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Error Args,please use -h option for a help!"));
        return 0;
    }

    //单实例运行
    QSharedMemory shared_memory("updater");
    if(!shared_memory.create(1)){
        if(shared_memory.error() == QSharedMemory::AlreadyExists){
            shared_memory.attach();
        }else{
            QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Shared Memory Error: ") + shared_memory.errorString());
            return 0;
        }
    }
    shared_memory.lock();
    int result = 0;
    try{
        UpdateType update_type_input = static_cast<UpdateType>(parser.value(update_type).toInt());
        int major_version_input=-1, minor_version_input=-1, fix_version_input=-1;
        if(update_type_input == UpdateType::CHECK_AND_DOWNLOAD){
            major_version_input = parser.value(major_version).toInt();
            minor_version_input = parser.value(minor_version).toInt();
            fix_version_input = parser.value(fix_version).toInt();
            CheckAndDownload c(major_version_input, minor_version_input, fix_version_input);
            c.show();
            result = a.exec();
        }else if(update_type_input == UpdateType::OVERRIDE_OLD){
            major_version_input = parser.value(major_version).toInt();
            minor_version_input = parser.value(minor_version).toInt();
            fix_version_input = parser.value(fix_version).toInt();
            OverrideOld o(major_version_input, minor_version_input, fix_version_input);
            o.show();
            result = a.exec();
        }else if(update_type_input == UpdateType::REMOVE_NEW){
            RemoveNew r;
            r.show();
            result = a.exec();
        }else{
            assert(false);
        }
    }catch(...){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Unknown Error"));
    }
    shared_memory.unlock();
    shared_memory.detach();
    return result;
}
