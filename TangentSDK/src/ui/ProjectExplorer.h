#pragma once
#include <QTreeView>
#include <QLineEdit>

class QFileSystemModel;
class QMenu;
class TabEditorWidget;

class ProjectExplorer : public QTreeView {
    Q_OBJECT
public:
    ProjectExplorer(QWidget* parent = nullptr);
    void setRootPath(const QString& path);
    void setTabEditor(TabEditorWidget* editor) { m_tabEditor = editor; }
    QString currentSelectedPath() const;
    QString rootPath() const { return m_rootPath; }
    void expandToPath(const QString& path);
    QString getCurrentProjectPath() const;
    void refresh();

public slots:
    void createNewFile();
    void createNewFolder();
    void createNewFileInRoot();
    void createNewFolderInRoot();

signals:
    void fileClicked(const QString& filePath);
    void fileDeleted(const QString& filePath);
    void fileMoved(const QString& oldPath, const QString& newPath);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onItemClicked(const QModelIndex& index);
    void onRenameFinished();
    void onFileRenamed(const QString& path, const QString& oldName, const QString& newName);
    void deleteSelected();
    void renameSelected();
    void copySelected();
    void cutSelected();
    void pasteSelected();

private:
    QFileSystemModel* model;
    TabEditorWidget* m_tabEditor = nullptr;
    QString m_rootPath;
    QLineEdit* m_editingWidget = nullptr;
    bool m_creatingFile = false;
    bool m_creatingFolder = false;
    QString m_clipboard;
    bool m_isCut = false;
    QModelIndex m_editingIndex;
    QString m_oldEditingPath;

    void startInlineEdit(const QString& parentPath, bool isFile);
    void finishInlineEdit();
    bool moveFileOrFolder(const QString& sourcePath, const QString& destDir);
    bool checkUnsavedAndPrompt(const QString& filePath);
};