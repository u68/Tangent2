#include "TabEditorWidget.h"
#include "EditorWidget.h"

#include <QFileInfo>
#include <QMessageBox>

TabEditorWidget::TabEditorWidget(QWidget* parent)
    : QTabWidget(parent) {
    setTabsClosable(false); // we use custom close buttons per-tab
    setMovable(true);
    setDocumentMode(true);

    // Use our SharpTabBar (controls height) and make tabs flat rectangular
    setTabBar(new SharpTabBar(this));

    setStyleSheet(
        "QTabWidget::pane {"
        "    border: none;"
        "    top: -1px;"
        "}"
        "QTabBar {"
        "    background: transparent;"
        "    spacing: 0px;"
        "}"
        "QTabBar::tab {"
        "    height: 22px;"
        "    padding: 0 10px;"                      /* center titles vertically */
        "    border: 1px solid #3b3b3b;"
        "    margin-right: 0px;"                    /* remove gaps between tabs */
        "    background-color: #2b2b2b;"
        "    color: #bfbfbf;"
        "    border-top-left-radius: 0px;"
        "    border-top-right-radius: 0px;"
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #1e1e1e;"
        "    color: #ffffff;"
        "    margin-top: 0px;"
        "    border-top: 1px solid #6a6a6a;"        /* always show 1px top border on selected */
        "}"
        "QTabBar::tab:!selected {"
        "    background-color: #262626;"
        "    color: #9a9a9a;"
        "}"
        "QTabBar::tab:hover {"
        "    background-color: #333333;"
        "}"
        "QTabBar::tab:selected:hover {"
        "    background-color: #1e1e1e;"            /* ensure hovering selected doesn't change look */
        "    border-top: 1px solid #6a6a6a;"
        "}"
        "QTabBar::close-button {"
        "    image: none;"
        "}"
    );

    connect(this, &QTabWidget::tabCloseRequested, this, &TabEditorWidget::closeTab);
    connect(this, &QTabWidget::currentChanged, this, &TabEditorWidget::onTabChanged);
}

EditorWidget* TabEditorWidget::openFile(const QString& filePath) {
    // Check if file is already open
    int existingTab = findTabByPath(filePath);
    if (existingTab != -1) {
        setCurrentIndex(existingTab);
        return qobject_cast<EditorWidget*>(widget(existingTab));
    }

    // Create new editor and open the file
    EditorWidget* editor = new EditorWidget(this);
    
    // Set project path before opening file
    if (!m_projectPath.isEmpty()) {
        editor->setProjectPath(m_projectPath);
    }
    
    if (!editor->openFile(filePath)) {
        delete editor;
        QMessageBox::warning(this, "Error", "Could not open file: " + filePath);
        return nullptr;
    }

    // Connect modification signal
    connect(editor, &EditorWidget::modificationChanged, this, &TabEditorWidget::onEditorModificationChanged);

    // Add the tab
    QString fileName = QFileInfo(filePath).fileName();
    int tabIndex = addTab(editor, fileName);
    setCurrentIndex(tabIndex);

    // Create a small close button with a simple glyph and no heavy hover overlay
    QToolButton* closeBtn = new QToolButton(this);
    closeBtn->setText(QString::fromUtf8("✕"));
    closeBtn->setAutoRaise(true);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QToolButton { color: #bfbfbf; background: transparent; border: none; padding: 0 6px; }"
        "QToolButton:hover { color: #ff5555; }"
    );
    // Close the tab that contains this file when clicked
    connect(closeBtn, &QToolButton::clicked, this, [this, filePath]() {
        int idx = findTabByPath(filePath);
        if (idx != -1) closeTab(idx);
    });
    if (tabBar()) tabBar()->setTabButton(tabIndex, QTabBar::RightSide, closeBtn);

    return editor;
}

EditorWidget* TabEditorWidget::currentEditor() const {
    return qobject_cast<EditorWidget*>(currentWidget());
}

bool TabEditorWidget::saveCurrentFile() {
    EditorWidget* editor = currentEditor();
    if (editor) {
        bool result = editor->saveFile();
        if (result) {
            updateTabTitle(currentIndex());
        }
        return result;
    }
    return false;
}

bool TabEditorWidget::saveAllFiles() {
    bool allSaved = true;
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor && editor->isModified()) {
            if (!editor->saveFile()) {
                allSaved = false;
            } else {
                updateTabTitle(i);
            }
        }
    }
    return allSaved;
}

void TabEditorWidget::closeTab(int index) {
    EditorWidget* editor = qobject_cast<EditorWidget*>(widget(index));
    if (!editor) return;

    // Check for unsaved changes
    if (editor->isModified()) {
        if (!promptSaveChanges(index)) {
            return; // User cancelled
        }
    }

    removeTab(index);
    editor->deleteLater();
}

void TabEditorWidget::onTabChanged(int index) {
    EditorWidget* editor = qobject_cast<EditorWidget*>(widget(index));
    if (editor) {
        emit currentFileChanged(editor->currentFilePath());
    } else {
        emit currentFileChanged(QString());
    }
}

void TabEditorWidget::onEditorModificationChanged(bool modified) {
    EditorWidget* editor = qobject_cast<EditorWidget*>(sender());
    if (editor) {
        int index = indexOf(editor);
        if (index != -1) {
            updateTabTitle(index);
            // Emit signal if this is the current editor
            if (index == currentIndex()) {
                emit currentEditorModificationChanged(modified);
            }
        }
    }
}

void TabEditorWidget::updateTabTitle(int index) {
    EditorWidget* editor = qobject_cast<EditorWidget*>(widget(index));
    if (editor) {
        QString title = editor->fileName();
        if (editor->isModified()) {
            title = "* " + title;
        }
        setTabText(index, title);
    }
}

int TabEditorWidget::findTabByPath(const QString& filePath) {
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor && editor->currentFilePath() == filePath) {
            return i;
        }
    }
    return -1;
}

bool TabEditorWidget::hasUnsavedChanges() const {
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor && editor->isModified()) {
            return true;
        }
    }
    return false;
}

bool TabEditorWidget::hasUnsavedChanges(const QString& filePath) const {
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor && editor->currentFilePath() == filePath) {
            return editor->isModified();
        }
    }
    return false;
}

bool TabEditorWidget::promptSaveChanges(int tabIndex) {
    EditorWidget* editor = qobject_cast<EditorWidget*>(widget(tabIndex));
    if (!editor || !editor->isModified()) {
        return true;
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Save Changes?");
    msgBox.setText(QString("Do you want to save the changes to '%1'?").arg(editor->fileName()));
    msgBox.setInformativeText("Your changes will be lost if you don't save them.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setIcon(QMessageBox::Question);
    
    int ret = msgBox.exec();
    
    switch (ret) {
        case QMessageBox::Save:
            return editor->saveFile();
        case QMessageBox::Discard:
            // Reload file from disk to discard changes
            editor->openFile(editor->currentFilePath());
            return true;
        case QMessageBox::Cancel:
        default:
            return false;
    }
}

bool TabEditorWidget::promptSaveAllChanges() {
    if (!hasUnsavedChanges()) {
        return true;
    }
    
    // Count unsaved files
    QStringList unsavedFiles;
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor && editor->isModified()) {
            unsavedFiles << editor->fileName();
        }
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Save Changes?");
    
    if (unsavedFiles.count() == 1) {
        msgBox.setText(QString("Do you want to save the changes to '%1'?").arg(unsavedFiles.first()));
    } else {
        msgBox.setText(QString("Do you want to save the changes to %1 files?").arg(unsavedFiles.count()));
        msgBox.setDetailedText("Unsaved files:\n" + unsavedFiles.join("\n"));
    }
    
    msgBox.setInformativeText("Your changes will be lost if you don't save them.");
    msgBox.setStandardButtons(QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::SaveAll);
    msgBox.setIcon(QMessageBox::Question);
    
    int ret = msgBox.exec();
    
    switch (ret) {
        case QMessageBox::SaveAll:
            return saveAllFiles();
        case QMessageBox::Discard:
            return true;
        case QMessageBox::Cancel:
        default:
            return false;
    }
}

bool TabEditorWidget::checkAndPromptSave(const QString& filePath) {
    int tabIndex = findTabByPath(filePath);
    if (tabIndex == -1) {
        return true; // File not open
    }
    
    EditorWidget* editor = qobject_cast<EditorWidget*>(widget(tabIndex));
    if (!editor || !editor->isModified()) {
        return true;
    }
    
    return promptSaveChanges(tabIndex);
}

bool TabEditorWidget::closeAllTabs() {
    // Prompt to save all changes
    if (!promptSaveAllChanges()) {
        return false;
    }
    
    while (count() > 0) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(0));
        if (editor) {
            removeTab(0);
            editor->deleteLater();
        }
    }
    return true;
}

void TabEditorWidget::closeTabByPath(const QString& filePath) {
    int index = findTabByPath(filePath);
    if (index != -1) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(index));
        if (editor) {
            removeTab(index);
            editor->deleteLater();
        }
    }
}

bool TabEditorWidget::renameOpenFile(const QString& oldPath, const QString& newPath) {
    int index = findTabByPath(oldPath);
    if (index == -1) return false;

    EditorWidget* editor = qobject_cast<EditorWidget*>(widget(index));
    if (!editor) return false;

    // Update editor internal path and highlighter
    editor->updateFilePath(newPath);

    // Update tab title
    updateTabTitle(index);

    // Replace close button lambda to reference the new path
    if (tabBar()) {
        QWidget* existing = tabBar()->tabButton(index, QTabBar::RightSide);
        if (existing) existing->deleteLater();

        QToolButton* closeBtn = new QToolButton(this);
        closeBtn->setText(QString::fromUtf8("✕"));
        closeBtn->setAutoRaise(true);
        closeBtn->setCursor(Qt::PointingHandCursor);
        closeBtn->setStyleSheet(
            "QToolButton { color: #bfbfbf; background: transparent; border: none; padding: 0 6px; }"
            "QToolButton:hover { color: #ff5555; }"
        );
        connect(closeBtn, &QToolButton::clicked, this, [this, newPath]() {
            int idx = findTabByPath(newPath);
            if (idx != -1) closeTab(idx);
        });
        tabBar()->setTabButton(index, QTabBar::RightSide, closeBtn);
    }

    return true;
}

bool TabEditorWidget::saveFile(const QString& filePath) {
    int index = findTabByPath(filePath);
    if (index != -1) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(index));
        if (editor) {
            bool result = editor->saveFile();
            if (result) {
                updateTabTitle(index);
            }
            return result;
        }
    }
    return false;
}

void TabEditorWidget::setProjectPath(const QString& projectPath) {
    m_projectPath = projectPath;
    
    // Update all open editors
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor) {
            editor->setProjectPath(projectPath);
        }
    }
}

void TabEditorWidget::refreshAllHighlighting() {
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor) {
            editor->refreshHighlighting();
        }
    }
}

void TabEditorWidget::reloadAllSyntaxColors() {
    for (int i = 0; i < count(); ++i) {
        EditorWidget* editor = qobject_cast<EditorWidget*>(widget(i));
        if (editor) {
            editor->reloadSyntaxColors();
        }
    }
}
