#include <QTabBar>
#include <QPainter>
#include <QToolButton>
#include <QPushButton>
#include <QMouseEvent>

class SharpTabBar : public QTabBar {
    Q_OBJECT
public:
    SharpTabBar(QWidget* parent = nullptr) : QTabBar(parent) {}
protected:
    QSize tabSizeHint(int index) const override {
        QSize s = QTabBar::tabSizeHint(index);
        s.setHeight(22);
        return s;
    }
};
#pragma once
#include <QTabWidget>
#include <QMap>

class EditorWidget;

class TabEditorWidget : public QTabWidget {
    Q_OBJECT
public:
    TabEditorWidget(QWidget* parent = nullptr);

    EditorWidget* openFile(const QString& filePath);
    EditorWidget* currentEditor() const;
    bool saveCurrentFile();
    bool saveAllFiles();
    bool closeAllTabs();
    void closeTabByPath(const QString& filePath);
    bool saveFile(const QString& filePath);
    void setProjectPath(const QString& projectPath);
    void refreshAllHighlighting();
    void reloadAllSyntaxColors(); // Reload colors on all editors
    bool renameOpenFile(const QString& oldPath, const QString& newPath);
    
    // Check for unsaved changes
    bool hasUnsavedChanges() const;
    bool hasUnsavedChanges(const QString& filePath) const;
    bool promptSaveChanges(int tabIndex); // Returns false if cancelled
    bool promptSaveAllChanges(); // Returns false if cancelled
    
    // For file operations - prompts if needed
    bool checkAndPromptSave(const QString& filePath); // Returns false if cancelled

signals:
    void currentFileChanged(const QString& filePath);
    void currentEditorModificationChanged(bool modified);

public slots:
    void closeTab(int index);

private slots:
    void onTabChanged(int index);
    void onEditorModificationChanged(bool modified);

private:
    QMap<QString, int> fileToTab;  // Maps file path to tab index
    QString m_projectPath;
    
    int findTabByPath(const QString& filePath);
    void updateTabTitle(int index);
};
