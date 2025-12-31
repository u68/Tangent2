#include "ConsoleWidget.h"
#include "../theme/SyntaxTheme.h"
#include <QScrollBar>
#include <QFont>
#include <QFontDatabase>
#include <QTextCursor>

ConsoleWidget::ConsoleWidget(QWidget* parent)
    : QPlainTextEdit(parent) {

    setReadOnly(true);
    
    // Set monospace font from theme
    reloadFromTheme();
    
    // Style
    setStyleSheet("QPlainTextEdit { background-color: #1e1e1e; color: #cccccc; }");
}

void ConsoleWidget::reloadFromTheme() {
    SyntaxTheme& theme = SyntaxTheme::instance();
    
    // Set font
    QFont font(theme.fontFamily(), theme.fontSize());
    setFont(font);
    
    // Setup text formats for different output types from theme
    SyntaxTheme::ColorEntry normalEntry = theme.getColor("console.normal");
    m_normalFormat.setForeground(normalEntry.color);
    m_normalFormat.setFontWeight(normalEntry.bold ? QFont::Bold : QFont::Normal);
    
    SyntaxTheme::ColorEntry errorEntry = theme.getColor("console.error");
    m_errorFormat.setForeground(errorEntry.color);
    m_errorFormat.setFontWeight(errorEntry.bold ? QFont::Bold : QFont::Normal);
    
    SyntaxTheme::ColorEntry warningEntry = theme.getColor("console.warning");
    m_warningFormat.setForeground(warningEntry.color);
    m_warningFormat.setFontWeight(warningEntry.bold ? QFont::Bold : QFont::Normal);
    
    SyntaxTheme::ColorEntry successEntry = theme.getColor("console.success");
    m_successFormat.setForeground(successEntry.color);
    m_successFormat.setFontWeight(successEntry.bold ? QFont::Bold : QFont::Normal);
}

void ConsoleWidget::appendColoredText(const QString& text, const QColor& color) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    
    QTextCharFormat format;
    format.setForeground(color);
    
    cursor.insertText(text, format);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ConsoleWidget::appendOutput(const QString& text) {
    appendColoredText(text, QColor("#cccccc"));
}

void ConsoleWidget::appendError(const QString& text) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text, m_errorFormat);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ConsoleWidget::appendWarning(const QString& text) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text, m_warningFormat);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ConsoleWidget::appendSuccess(const QString& text) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text, m_successFormat);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ConsoleWidget::clear() {
    QPlainTextEdit::clear();
}
