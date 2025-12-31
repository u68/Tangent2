#include "ConsoleWidget.h"
#include "../editor/SyntaxDefinition.h"
#include <QScrollBar>
#include <QFont>
#include <QFontDatabase>
#include <QTextCursor>
#include <QSettings>

ConsoleWidget::ConsoleWidget(QWidget* parent)
    : QPlainTextEdit(parent) {

    setReadOnly(true);
    
    // Set monospace font from theme
    reloadFromTheme();
    
    // Style
    setStyleSheet("QPlainTextEdit { background-color: #1e1e1e; color: #cccccc; }");
}

void ConsoleWidget::reloadFromTheme() {
    SyntaxDefinition& def = SyntaxDefinition::instance();
    
    // Set font from settings
    QSettings settings("TangentSDK", "TangentSDK");
    QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QString fontFamily = settings.value("editor/fontFamily", defaultFont.family()).toString();
    int fontSize = settings.value("editor/fontSize", 11).toInt();
    QFont font(fontFamily, fontSize);
    setFont(font);
    
    // Setup text formats for different output types from SyntaxDefinition
    SyntaxRule normalRule = def.getConsoleRule("normal");
    m_normalFormat.setForeground(normalRule.color);
    m_normalFormat.setFontWeight(normalRule.bold ? QFont::Bold : QFont::Normal);
    
    SyntaxRule errorRule = def.getConsoleRule("error");
    m_errorFormat.setForeground(errorRule.color);
    m_errorFormat.setFontWeight(errorRule.bold ? QFont::Bold : QFont::Normal);
    
    SyntaxRule warningRule = def.getConsoleRule("warning");
    m_warningFormat.setForeground(warningRule.color);
    m_warningFormat.setFontWeight(warningRule.bold ? QFont::Bold : QFont::Normal);
    
    SyntaxRule successRule = def.getConsoleRule("success");
    m_successFormat.setForeground(successRule.color);
    m_successFormat.setFontWeight(successRule.bold ? QFont::Bold : QFont::Normal);
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
