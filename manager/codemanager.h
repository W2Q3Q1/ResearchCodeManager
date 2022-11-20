#ifndef CODEMANAGER_H
#define CODEMANAGER_H
#include <QString>
#include <QSqlDatabase>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QVector>
#include <QList>
#include <QVariant>

/*
    代码管理类
    此类直接操作底层数据库、文件，因此不会进行任何的用户提示
    如需用户提示（如确定等），写在界面类
*/
class CodeManager: public QObject
{
    Q_OBJECT
    private:
        static CodeManager* self;
        QSqlDatabase database;
        QDir codePath;

        enum FileType{
            FILE = 0,
            DIRECTORY = 1
        };
        const int systemType;

        /*
            无需public的工具函数
        */
        QString fileSHA512(const QString &file_path);
        bool isSameFile(const QString &a_path, const QString &b_path);
        bool researchIDExist(int research_idx);
        bool researchHasCode(int research_idx);
        bool codeIDExist(int code_idx);
        bool codeIDCodeFileSourceExist(int code_idx, const QString &file_source);
        bool codeHasFile(int code_idx);
        bool codeHasResult(int code_idx);
        bool resultIDExist(int result_idx);
        bool resultHasConfigFile(int result_idx);
        bool resultHasLogFile(int result_idx);
        bool resultHasHyperParameter(int result_idx);
        bool resultHasResultKV(int result_idx);
        bool resultIDConfigFileSourceExist(int result_idx, const QString &file_source);
        bool resultIDLogFileSourceExist(int result_idx, const QString &file_source);
        bool resultHyperParameterHasKey(int result_idx, const QString &key);
        bool hyperParameterIDExist(int param_idx);
        bool resultResultHasKey(int result_idx, const QString &key);
        bool resultKVIDExist(int result_kv_idx);
        int getTableIncrementID(const QString &table_name);

        /*
            复制文件/文件夹到codePath
            Args:
                source: const QString &: 文件/文件夹路径
            Returns:
                QMap<QString, QString>: 文件/文件夹相对source的路径<->保存后相对codePath的路径
            Note:
                此函数不会修改数据库，仅控制复制
                链接均按文件/文件夹处理，不能保留原链接关系
                假定文件、文件夹可拓扑排序
                当且仅当键为文件夹时，值为QString()，且不含source本身
        */
        QMap<QString, QString> copyToCodePath(const QString &source);

        /*
            删除无引用文件
            Args:
                None
            Returns:
                None
        */
        void delUselessFile();

        CodeManager();
        CodeManager(const CodeManager &c) = delete;
        CodeManager& operator=(const CodeManager& c) = delete;
        virtual ~CodeManager();

    public:
        static CodeManager *getSelf();
        static void closeSelf();

        /*
            需要public的工具函数，一般为前提确认函数
        */
        bool researchNameExist(const QString &research_name);
        bool researchIDCodeNameExist(int research_idx, const QString &code_name);
        bool codeOnSystem(int code_idx);
        bool codeIDResultNameExist(int code_idx, const QString &result_name);


        //以下为文件、项目管理接口

        /*
            添加研究信息
            Args:
                name: const QString &: 研究名称
                information: const QString &: 研究信息
            Returns:
                int: 研究编号
        */
        int addResearchInfo(const QString &name, const QString &information);

        /*
            删除研究信息，调用前应保证研究对应的全部代码均已经删除
            Args:
                research_idx: int: 研究编号
            Returns:
                void
        */
        void deleteResearchInfo(int research_idx);

        /*
            添加代码信息
            Args:
                name: const QString &: 代码名称
                information: const QString &: 代码信息
                research_idx: int: 研究编号
            Returns:
                int: 代码ID
        */
        int addCodeInfo(const QString &name, const QString &information, int research_idx);

        /*
            添加代码文件，需要保证当前无代码文件，才能添加
            Args:
                source: const QString &: 代码路径，如为文件夹，则不保存本身信息
                code_idx: int: 代码编号
            Returns:
                None
        */
        void addCodeFile(const QString &source, int code_idx);

        /*
            导出代码（不会对codePath内的代码造成影响，仅复制）
            Args:
                code_idx: int: 代码编号
                destination_path: const QString &: 目标路径，必须存在且为空文件夹，将文件/文件夹（仅内容物）导入到目标路径内
            Returns:
                void
        */
        void exportCodeFile(int code_idx, const QString &destination_path);

        /*
            删除代码
            Args:
                code_idx: int: 代码编号
            Returns:
                void
        */
        void deleteCodeFile(int code_idx);

        /*
            删除代码信息，调用前应保证代码对应的文件、全部的结果均已经删除
            Args:
                code_idx: int: 代码编号
            Returns:
                void
        */
        void deleteCodeInfo(int code_idx);

        /*
            添加结果信息
            Args:
                name: const QString &: 结果名称
                code_idx: int: 代码编号
            Returns:
                int: 结果编号
        */
        int addResultInfo(const QString &name, int code_idx);

        /*
            添加Config文件
            Args:
                source: const QString &: 配置文件路径
                result_idx: int: 结果编号
            Returns:
                None
        */
        void addConfigFile(const QString &source, int result_idx);

        /*
            导出结果中的配置文件（不会对codePath内的代码造成影响，仅复制）
            Args:
                result_idx: int: 结果编号
                destination_path: const QString &: 目标路径，必须存在且为空文件夹，将文件/文件夹（仅内容物）导入到目标路径内
            Returns:
                void
        */
        void exportConfigFile(int result_idx, const QString &destination_path);

        /*
            删除配置文件
            Args:
                result_idx: int: 结果编号
            Returns:
                void
        */
        void deleteConfigFile(int result_idx);

        /*
            添加Log文件
            Args:
                source: const QString &: 配置文件路径
                result_idx: int: 结果编号
            Returns:
                None
        */
        void addLogFile(const QString &source, int result_idx);

        /*
            导出结果中的Log文件（不会对codePath内的代码造成影响，仅复制）
            Args:
                result_idx: int: 结果编号
                destination_path: const QString &: 目标路径，必须存在且为空文件夹，将文件/文件夹（仅内容物）导入到目标路径内
            Returns:
                void
        */
        void exportLogFile(int result_idx, const QString &destination_path);

        /*
            删除Log文件
            Args:
                result_idx: int: 结果编号
            Returns:
                void
        */
        void deleteLogFile(int result_idx);

        /*
            添加一条超参数Key Value，所有的Key和Value均使用字符串
            Args:
                Key: const QString &: 超参数键
                Value: const QString &: 超参数值
                result_idx: int: 结果编号
            Returns:
                int: 超参数编号
        */
        int addHyperParameterKV(const QString &key, const QString &value, int result_idx);

        /*
            删除一条超参数
            Args:
                param_idx: int: 超参数编号
            Returns:
                void
        */
        void deleteHyperParameterKV(int param_idx);

        /*
            删除全部超参数
            Args:
                result_idx: int: 结果编号
            Returns:
                void
        */
        void deleteAllHyperParameterKVOfResult(int result_idx);

        /*
            添加一条结果Key Value，所有的Key和Value均使用字符串
            Args:
                Key: const QString &: 超参数键
                Value: const QString &: 超参数值
                result_idx: int: 结果编号
            Returns:
                int: 结果编号
        */
        int addResultKV(const QString &key, const QString &value, int result_idx);

        /*
            删除一条结果
            Args:
                result_kv_idx: int: 结果kv编号
            Returns:
                void
        */
        void deleteResultKV(int result_kv_idx);

        /*
            删除全部结果
            Args:
                result_idx: int: 结果编号
            Returns:
                void
        */
        void deleteAllResultKVOfResult(int result_idx);

        /*
            删除结果信息，调用前应保证结果对应的文件（Log Config）及超参数、结果列表清空已经删除
            Args:
                result_idx: int: 代码编号
            Returns:
                void
        */
        void deleteResultInfo(int result_idx);

        //以下为向上层提供数据的接口

        /*
            获取全部的研究信息
            Args:
                None
            Returns:
                QList<QVector<QVariant>>: 每一行包含一个QVector，表示一个研究的信息；每行包含多个QVariant，依次代表ID (int), Name (QString), Time (QDateTime), Information (QString)
        */
        QList<QVector<QVariant>> getAllResearchInfo();

        /*
            获取研究下全部代码信息
            Args:
                research_idx: int: 研究编号
            Returns:
                QList<QVector<QVariant>>: 每一行包含一个QVector，表示一个代码的信息；每行包含多个QVariant，依次代表ID (int), Name (QString), Time (QDateTime), Information (QString), SystemType (int), HasFile (bool)
        */
        QList<QVector<QVariant>> getAllCodeInfoOfResearch(int research_idx);

        /*
            获取代码下全部结果信息
            Args:
                code_idx: int: 代码编号
            Returns:
                QList<QVector<QVariant>>: 每一行包含一个QVector，表示一个结果的信息；每行包含多个QVariant，依次代表ID (int), Name (QString), Time (QDateTime), HasConfigFile (bool), HasLogFile (bool)， HyperParameterKV (QMap<QString, QString>), ResultKV (QMap<QString, QString>)
        */
        QList<QVector<QVariant>> getAllResultInfoOfCode(int code_idx);
};

#endif // CODEMANAGER_H
