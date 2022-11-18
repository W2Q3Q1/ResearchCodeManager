#ifndef RESEARCHCODEMANAGER_H
#define RESEARCHCODEMANAGER_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class ResearchCodeManager; }
QT_END_NAMESPACE

class ResearchCodeManager : public QMainWindow
{
        Q_OBJECT

    private:
        enum ResearchTreeType{
            RESEARCH = QTreeWidgetItem::UserType,
            CODE,
            RESULT,
            NONETYPE
        };
        enum ResearchDataType{
            RESEARCH_IDX = Qt::UserRole,
            RESEARCH_NAME,
            RESEARCH_TIME,
            RESEARCH_INFORMATION
        };
        enum CodeDataType{
            CODE_IDX = Qt::UserRole,
            CODE_NAME,
            CODE_TIME,
            CODE_INFORMATION,
            CODE_SYSTEMTYPE,
            CODE_HAS_FILE,
            CODE_RESEARCH_IDX
        };
        enum ResultDataType{
            RESULT_IDX = Qt::UserRole,
            RESULT_NAME,
            RESULT_TIME,
            RESULT_HAS_CONFIG_FILE,
            RESULT_HAS_LOG_FILE,
            RESULT_HYPER_PARAMETER_KV,
            RESULT_RESULT_KV,
            RESULT_CODE_IDX
        };

        /*
            刷新ResearchTreeWidget，并选中对应的项目
            如果：checked_type == NONETYPE or checked_idx not exist，则自动选中第一个
            Args:
                checked_type: ResearchTreeType: 被选中的项类型
                checked_idx: int: 被选中的项的ID
            Returns:
                None
        */
        void refreshResearchTreeWidget(ResearchCodeManager::ResearchTreeType checked_type, int checked_idx);
        void showEvent(QShowEvent* event) override;
        void hideEvent(QHideEvent* event) override;

    public:
        ResearchCodeManager(QWidget *parent = nullptr);
        virtual ~ResearchCodeManager();

    private slots:
        void researchTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
        void showResearchTreeWidgetMenu(const QPoint &pos);

        void addResearchTriggered();
        void deleteResearchTriggered();
        void addCodeTriggered();
        void deleteCodeTriggered();
        void addResultTriggered();
        void deleteResultTriggered();
        /*
            重载函数，相当于checked_type == NONETYPE or checked_idx not exist
        */
        void refreshResearchTreeWidget();
        void exportCodeClicked();
    private:
        Ui::ResearchCodeManager *ui;
        QMenu *researchItemMenu;
        QMenu *codeItemMenu;
        QMenu *resultItemMenu;
        QMenu *whiteSpaceMenu;
        QAction *addResearchAction;
        QAction *deleteResearchAction;
        QAction *addCodeAction;
        QAction *deleteCodeAction;
        QAction *addResultAction;
        QAction *deleteResultAction;
        QAction *refreshResearchTreeAction;
        ResearchCodeManager::ResearchTreeType checked_type_before_hide;
        int checked_idx_before_hide;
};
#endif // RESEARCHCODEMANAGER_H
