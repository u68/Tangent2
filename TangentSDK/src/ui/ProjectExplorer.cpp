#include "ProjectExplorer.h"
#include "TabEditorWidget.h"
#include <QFileSystemModel>
#include <QFileInfo>
#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

ProjectExplorer::ProjectExplorer(QWidget* parent)
    : QTreeView(parent) {

    model = new QFileSystemModel(this);
    model->setRootPath("");
    model->setReadOnly(false);

    setModel(model);
    setRootIndex(model->index(QDir::currentPath()));

    // Hide Size, Type, and Date Modified columns - only show Name
    setColumnHidden(1, true);  // Size
    setColumnHidden(2, true);  // Type
    setColumnHidden(3, true);  // Date Modified

    // Hide the header since we only have one column now
    header()->hide();

    // Reduce indentation
    setIndentation(12);

    // Expand on single click (not double click)
    setExpandsOnDoubleClick(false);

    // Enable editing
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Enable drag and drop
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);

    // Selection behavior
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &QTreeView::clicked, this, &ProjectExplorer::onItemClicked);
}

void ProjectExplorer::setRootPath(const QString& path) {
    m_rootPath = path;
    model->setRootPath(path);
    setRootIndex(model->index(path));
}

void ProjectExplorer::expandToPath(const QString& path) {
    QModelIndex index = model->index(path);
    if (index.isValid()) {
        expand(index);
        setCurrentIndex(index);
        scrollTo(index);
    }
}

QString ProjectExplorer::getCurrentProjectPath() const {
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return QString();
    }
    
    QString selectedPath = model->filePath(index);
    QFileInfo info(selectedPath);
    
    // If it's a file, get its directory
    if (info.isFile()) {
        selectedPath = info.absolutePath();
    }
    
    // Walk up from selected path to find the project folder (direct child of workspace)
    QDir dir(selectedPath);
    while (dir.absolutePath() != m_rootPath && dir.cdUp()) {
        QString parent = dir.absolutePath();
        if (parent == m_rootPath) {
            // The directory before going to workspace root is the project folder
            return selectedPath;
        }
        selectedPath = parent;
    }
    
    // If selected path is the workspace root itself, return empty
    if (selectedPath == m_rootPath) {
        return QString();
    }
    
    return selectedPath;
}

void ProjectExplorer::refresh() {
    // Force refresh of the file system model
    QString rootPath = m_rootPath;
    model->setRootPath("");
    model->setRootPath(rootPath);
}

QString ProjectExplorer::currentSelectedPath() const {
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        QString path = model->filePath(index);
        QFileInfo info(path);
        if (info.isFile()) {
            return info.absolutePath();
        }
        return path;
    }
    return m_rootPath;
}

void ProjectExplorer::createNewFile() {
    QString parentPath = currentSelectedPath();
    startInlineEdit(parentPath, true);
}

void ProjectExplorer::createNewFolder() {
    QString parentPath = currentSelectedPath();
    startInlineEdit(parentPath, false);
}

void ProjectExplorer::createNewFileInRoot() {
    startInlineEdit(m_rootPath, true);
}

void ProjectExplorer::createNewFolderInRoot() {
    startInlineEdit(m_rootPath, false);
}

void ProjectExplorer::startInlineEdit(const QString& parentPath, bool isFile) {
    m_creatingFile = isFile;
    m_creatingFolder = !isFile;

    // Create a temporary item by actually creating a temp file/folder
    QString tempName = isFile ? "NewFile.txt" : "NewFolder";
    QString fullPath = parentPath + "/" + tempName;
    
    // Find unique name
    int counter = 1;
    while (QFileInfo::exists(fullPath)) {
        tempName = isFile ? QString("NewFile%1.txt").arg(counter) : QString("NewFolder%1").arg(counter);
        fullPath = parentPath + "/" + tempName;
        counter++;
    }

    // Create the file/folder
    if (isFile) {
        QFile file(fullPath);
        file.open(QIODevice::WriteOnly);
        file.close();
    } else {
        QDir().mkdir(fullPath);
    }

    // Select and edit the new item
    QModelIndex newIndex = model->index(fullPath);
    setCurrentIndex(newIndex);
    scrollTo(newIndex);
    
    // Use the built-in edit
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    edit(newIndex);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    m_editingIndex = newIndex;
}

void ProjectExplorer::contextMenuEvent(QContextMenuEvent* event) {
    QModelIndex index = indexAt(event->pos());
    
    QMenu menu(this);
    
    QAction* newFileAction = menu.addAction("New File");
    QAction* newFolderAction = menu.addAction("New Folder");
    menu.addSeparator();
    
    QAction* renameAction = nullptr;
    QAction* deleteAction = nullptr;
    QAction* copyAction = nullptr;
    QAction* cutAction = nullptr;
    
    if (index.isValid()) {
        renameAction = menu.addAction("Rename");
        deleteAction = menu.addAction("Delete");
        menu.addSeparator();
        copyAction = menu.addAction("Copy");
        cutAction = menu.addAction("Cut");
    }
    
    QAction* pasteAction = menu.addAction("Paste");
    pasteAction->setEnabled(!m_clipboard.isEmpty());
    
    connect(newFileAction, &QAction::triggered, this, &ProjectExplorer::createNewFile);
    connect(newFolderAction, &QAction::triggered, this, &ProjectExplorer::createNewFolder);
    
    if (renameAction) connect(renameAction, &QAction::triggered, this, &ProjectExplorer::renameSelected);
    if (deleteAction) connect(deleteAction, &QAction::triggered, this, &ProjectExplorer::deleteSelected);
    if (copyAction) connect(copyAction, &QAction::triggered, this, &ProjectExplorer::copySelected);
    if (cutAction) connect(cutAction, &QAction::triggered, this, &ProjectExplorer::cutSelected);
    connect(pasteAction, &QAction::triggered, this, &ProjectExplorer::pasteSelected);
    
    menu.exec(event->globalPos());
}

void ProjectExplorer::onItemClicked(const QModelIndex& index) {
    QString filePath = model->filePath(index);
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile()) {
        emit fileClicked(filePath);
    } else if (fileInfo.isDir()) {
        // Toggle expand/collapse on single click for folders
        if (isExpanded(index)) {
            collapse(index);
        } else {
            expand(index);
        }
    }
}

void ProjectExplorer::renameSelected() {
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        setEditTriggers(QAbstractItemView::AllEditTriggers);
        edit(index);
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void ProjectExplorer::deleteSelected() {
    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    
    QString path = model->filePath(index);
    QFileInfo info(path);

    // Check for unsaved changes
    if (!checkUnsavedAndPrompt(path)) {
        return;
    }
    
    QString message = info.isDir() 
        ? QString("Are you sure you want to delete the folder '%1' and all its contents?").arg(info.fileName())
        : QString("Are you sure you want to delete '%1'?").arg(info.fileName());
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete", message,
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // Close the tab if file is open
        if (m_tabEditor) {
            m_tabEditor->closeTabByPath(path);
        }
        emit fileDeleted(path);
        
        if (info.isDir()) {
            QDir(path).removeRecursively();
        } else {
            QFile::remove(path);
        }
    }
}

void ProjectExplorer::copySelected() {
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        m_clipboard = model->filePath(index);
        m_isCut = false;
    }
}

void ProjectExplorer::cutSelected() {
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        m_clipboard = model->filePath(index);
        m_isCut = true;
    }
}

void ProjectExplorer::pasteSelected() {
    if (m_clipboard.isEmpty()) return;
    
    QString destDir = currentSelectedPath();
    QFileInfo srcInfo(m_clipboard);
    QString destPath = destDir + "/" + srcInfo.fileName();
    
    // Handle name conflicts
    int counter = 1;
    while (QFileInfo::exists(destPath)) {
        QString baseName = srcInfo.completeBaseName();
        QString suffix = srcInfo.suffix();
        if (srcInfo.isDir()) {
            destPath = destDir + "/" + baseName + QString("_%1").arg(counter);
        } else {
            destPath = destDir + "/" + baseName + QString("_%1.").arg(counter) + suffix;
        }
        counter++;
    }
    
    if (srcInfo.isDir()) {
        // Copy directory recursively
        QDir().mkpath(destPath);
        QDir srcDir(m_clipboard);
        for (const QString& file : srcDir.entryList(QDir::Files)) {
            QFile::copy(m_clipboard + "/" + file, destPath + "/" + file);
        }
        for (const QString& dir : srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QDir().mkpath(destPath + "/" + dir);
        }
        if (m_isCut) {
            QDir(m_clipboard).removeRecursively();
        }
    } else {
        QFile::copy(m_clipboard, destPath);
        if (m_isCut) {
            QFile::remove(m_clipboard);
        }
    }
    
    if (m_isCut) {
        m_clipboard.clear();
    }
}

void ProjectExplorer::onRenameFinished() {
    // This is called when inline editing is finished
}

void ProjectExplorer::finishInlineEdit() {
    m_creatingFile = false;
    m_creatingFolder = false;
}

void ProjectExplorer::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete) {
        deleteSelected();
        return;
    } else if (event->matches(QKeySequence::Copy)) {
        copySelected();
        return;
    } else if (event->matches(QKeySequence::Cut)) {
        cutSelected();
        return;
    } else if (event->matches(QKeySequence::Paste)) {
        pasteSelected();
        return;
    } else if (event->key() == Qt::Key_F2) {
        renameSelected();
        return;
    }
    QTreeView::keyPressEvent(event);
}

void ProjectExplorer::mousePressEvent(QMouseEvent* event) {
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {
        clearSelection();
        setCurrentIndex(QModelIndex());
    }
    QTreeView::mousePressEvent(event);
}

void ProjectExplorer::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        QTreeView::dragEnterEvent(event);
    }
}

void ProjectExplorer::dragMoveEvent(QDragMoveEvent* event) {
    QModelIndex index = indexAt(event->position().toPoint());
    if (index.isValid() || !index.isValid()) {
        event->acceptProposedAction();
    }
    QTreeView::dragMoveEvent(event);
}

void ProjectExplorer::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (!mimeData->hasUrls()) {
        QTreeView::dropEvent(event);
        return;
    }

    QModelIndex targetIndex = indexAt(event->position().toPoint());
    QString destDir;
    
    if (targetIndex.isValid()) {
        QString targetPath = model->filePath(targetIndex);
        QFileInfo targetInfo(targetPath);
        if (targetInfo.isDir()) {
            destDir = targetPath;
        } else {
            destDir = targetInfo.absolutePath();
        }
    } else {
        destDir = m_rootPath;
    }

    for (const QUrl& url : mimeData->urls()) {
        QString sourcePath = url.toLocalFile();
        if (!sourcePath.isEmpty() && sourcePath != destDir) {
            if (!moveFileOrFolder(sourcePath, destDir)) {
                // Move was cancelled
                event->ignore();
                return;
            }
        }
    }

    event->acceptProposedAction();
}

bool ProjectExplorer::checkUnsavedAndPrompt(const QString& filePath) {
    if (!m_tabEditor) return true;
    
    QFileInfo info(filePath);
    
    if (info.isFile()) {
        // Check single file
        return m_tabEditor->checkAndPromptSave(filePath);
    } else if (info.isDir()) {
        // Check all files in directory recursively
        QDirIterator it(filePath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString childPath = it.next();
            if (!m_tabEditor->checkAndPromptSave(childPath)) {
                return false; // User cancelled
            }
        }
    }
    
    return true;
}

bool ProjectExplorer::moveFileOrFolder(const QString& sourcePath, const QString& destDir) {
    // Check for unsaved changes before moving
    if (!checkUnsavedAndPrompt(sourcePath)) {
        return false;
    }

    QFileInfo srcInfo(sourcePath);
    QString destPath = destDir + "/" + srcInfo.fileName();

    // Handle name conflicts
    int counter = 1;
    while (QFileInfo::exists(destPath)) {
        QString baseName = srcInfo.completeBaseName();
        QString suffix = srcInfo.suffix();
        if (srcInfo.isDir()) {
            destPath = destDir + "/" + baseName + QString("_%1").arg(counter);
        } else {
            destPath = destDir + "/" + baseName + QString("_%1.").arg(counter) + suffix;
        }
        counter++;
    }

    // Close the tab before moving
    if (m_tabEditor && srcInfo.isFile()) {
        m_tabEditor->closeTabByPath(sourcePath);
    }
    
    emit fileMoved(sourcePath, destPath);

    if (srcInfo.isDir()) {
        QDir().rename(sourcePath, destPath);
    } else {
        QFile::rename(sourcePath, destPath);
    }
    
    return true;
}
