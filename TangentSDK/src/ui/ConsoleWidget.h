#pragma once
#include <QPlainTextEdit>
#include <QTextCharFormat>

class ConsoleWidget : public QPlainTextEdit {
public:
    ConsoleWidget(QWidget* parent = nullptr);
    
    void appendOutput(const QString& text);
    void appendError(const QString& text);
    void appendWarning(const QString& text);
    void appendSuccess(const QString& text);
    void clear();
    
    // Reload colors and font from SyntaxTheme
    void reloadFromTheme();
    
private:
    void appendColoredText(const QString& text, const QColor& color);
    
    QTextCharFormat m_errorFormat;
    QTextCharFormat m_warningFormat;
    QTextCharFormat m_successFormat;
    QTextCharFormat m_normalFormat;
};
