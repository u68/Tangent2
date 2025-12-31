#pragma once
#include <QListWidget>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QStyledItemDelegate>

class QPlainTextEdit;

// Custom delegate for completion items with file info
class CompletionItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    CompletionItemDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

class CompletionPopup : public QListWidget {
    Q_OBJECT
public:
    enum FileType { Unknown, TASM, TML };
    
    // Custom roles for item data
    enum ItemRole {
        FileInfoRole = Qt::UserRole + 1
    };
    
    CompletionPopup(QWidget* parent);

    void showAtCursor(const QRect& cursorRect);
    void setFileType(FileType type);
    void setProjectPath(const QString& projectPath);
    void refreshCompletions();
    void updateFilter(const QString& prefix);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    FileType m_fileType = Unknown;
    QString m_projectPath;
    QStringList m_allCompletions;
    QMap<QString, QString> m_labelFileMap;  // label -> filename
    QPlainTextEdit* m_editor = nullptr;
    
    void buildCompletionList();
    void insertCompletion();
    QString getCurrentWordPrefix();
    void addCompletionItem(const QString& text, const QString& fileInfo = QString());
};
