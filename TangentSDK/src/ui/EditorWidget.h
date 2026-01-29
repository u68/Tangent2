#pragma once
#include <QPlainTextEdit>
#include <QString>

class SyntaxHighlighter;
class CompletionPopup;
class LineNumberArea;

class EditorWidget : public QPlainTextEdit {
    Q_OBJECT
public:
    EditorWidget(QWidget* parent = nullptr);

    bool openFile(const QString& filePath);
    bool saveFile();
    bool saveFileAs(const QString& filePath);
    
    QString currentFilePath() const { return m_filePath; }
    QString fileName() const;
    bool isModified() const { return m_isModified; }
    
    void setProjectPath(const QString& projectPath);
    void refreshHighlighting();
    void reloadSyntaxColors(); // Reload colors from settings
    void updateFilePath(const QString& newPath);
    
    // Line number area
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

signals:
    void fileSaved(const QString& filePath);
    void modificationChanged(bool modified);

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect& rect, int dy);
    void highlightCurrentLine();
    void onTextChanged();

private:
    SyntaxHighlighter* highlighter;
    CompletionPopup* completion;
    LineNumberArea* lineNumberArea;
    QString m_filePath;
    QString m_projectPath;
    bool m_isModified = false;
    bool m_isLoading = false;
    QString m_originalContent;
    
    enum FileType { Unknown, TASM, TML, C };
    FileType m_fileType = Unknown;
    
    void handleAutoIndent();
    QString getIndentForNewLine(const QString& currentLine);
    void showCompletionPopup();
    void maybeShowCompletion();
    bool shouldAutoCloseBracket();
};

// Line number area widget
class LineNumberArea : public QWidget {
public:
    LineNumberArea(EditorWidget* editor) : QWidget(editor), m_editor(editor) {}
    
    QSize sizeHint() const override {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        m_editor->lineNumberAreaPaintEvent(event);
    }

private:
    EditorWidget* m_editor;
};
