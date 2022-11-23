#include "researchcodemanager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include "codemanager.h"
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //单实例运行
    QSharedMemory shared_memory("manager");
    if(shared_memory.attach()){
        shared_memory.detach();
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("An instance has already been activated!"));
        return 0;
    }
    shared_memory.create(1);
    int result = 0;
    try{
        //迫使代码创建一个CodeManager对象
        CodeManager::getSelf();
        QTranslator translator;
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "research_code_manager_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }

        ResearchCodeManager w;
        w.show();
        int result = a.exec();
        CodeManager::closeSelf();
    }catch(...){

    }
    shared_memory.detach();
    return result;
}
