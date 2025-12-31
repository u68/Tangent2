#include "EditorWidget.h"
#include "../editor/SyntaxHighlighter.h"
#include "../editor/CompletionPopup.h"
#include "../theme/SyntaxTheme.h"

#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QTextBlock>
#include <QRegularExpression>
#include <QTimer>

EditorWidget::EditorWidget(QWidget* parent)
    : QPlainTextEdit(parent) {

    // Set font from SyntaxTheme
    SyntaxTheme& theme = SyntaxTheme::instance();
    QFont editorFont(theme.fontFamily(), theme.fontSize());
    setFont(editorFont);

    setTabStopDistance(4 * fontMetrics().horizontalAdvance(' '));
    setLineWrapMode(NoWrap);

    highlighter = new SyntaxHighlighter(document());
    completion = new CompletionPopup(this);
    
    // Line number area
    lineNumberArea = new LineNumberArea(this);
    
    connect(this, &QPlainTextEdit::blockCountChanged, this, &EditorWidget::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &EditorWidget::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &EditorWidget::highlightCurrentLine);
    connect(this, &QPlainTextEdit::textChanged, this, &EditorWidget::onTextChanged);
    
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int EditorWidget::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    // Use the themed font for line number width calculation
    SyntaxTheme& theme = SyntaxTheme::instance();
    QFont lineNumberFont(theme.fontFamily(), theme.fontSize());
    QFontMetrics fm(lineNumberFont);
    int space = 10 + fm.horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void EditorWidget::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void EditorWidget::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void EditorWidget::resizeEvent(QResizeEvent* e) {
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void EditorWidget::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor("#2a2a2a");
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void EditorWidget::lineNumberAreaPaintEvent(QPaintEvent* event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor("#1e1e1e"));

    // Use the same font as the editor for line numbers
    SyntaxTheme& theme = SyntaxTheme::instance();
    QFont lineNumberFont(theme.fontFamily(), theme.fontSize());
    painter.setFont(lineNumberFont);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor("#858585"));
            QFontMetrics fm(lineNumberFont);
            painter.drawText(0, top, lineNumberArea->width() - 5, fm.height(),
                           Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void EditorWidget::setProjectPath(const QString& projectPath) {
    m_projectPath = projectPath;
    highlighter->setProjectPath(projectPath);
    completion->setProjectPath(projectPath);
}

void EditorWidget::refreshHighlighting() {
    highlighter->refreshProjectLabels();
    completion->refreshCompletions();
}

void EditorWidget::reloadSyntaxColors() {
    // Reload font from theme
    SyntaxTheme& theme = SyntaxTheme::instance();
    QFont editorFont(theme.fontFamily(), theme.fontSize());
    setFont(editorFont);
    setTabStopDistance(4 * fontMetrics().horizontalAdvance(' '));
    
    // Reload syntax colors
    highlighter->reloadColorsFromSettings();
    
    // Update line number area width in case font changed
    updateLineNumberAreaWidth(0);
    
    // Force repaint of line number area to use new font
    lineNumberArea->update();
}

bool EditorWidget::openFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Set loading flag to prevent textChanged from marking as modified
    m_isLoading = true;
    setPlainText(content);
    m_isLoading = false;
    
    m_filePath = filePath;
    m_originalContent = content;
    m_isModified = false;
    emit modificationChanged(false);
    
    highlighter->setFileType(filePath);
    
    // Determine file type for auto-indent
    QFileInfo fi(filePath);
    QString ext = fi.suffix().toLower();
    if (ext == "tasm") {
        m_fileType = TASM;
        completion->setFileType(CompletionPopup::TASM);
    } else if (ext == "tml") {
        m_fileType = TML;
        completion->setFileType(CompletionPopup::TML);
    } else {
        m_fileType = Unknown;
        completion->setFileType(CompletionPopup::Unknown);
    }
    
    return true;
}

bool EditorWidget::saveFile() {
    if (m_filePath.isEmpty()) {
        return false;
    }
    return saveFileAs(m_filePath);
}

bool EditorWidget::saveFileAs(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QString content = toPlainText();
    QTextStream out(&file);
    out << content;
    file.close();

    m_filePath = filePath;
    m_originalContent = content;
    m_isModified = false;
    emit modificationChanged(false);
    emit fileSaved(filePath);
    return true;
}

QString EditorWidget::fileName() const {
    if (m_filePath.isEmpty()) {
        return "Untitled";
    }
    return QFileInfo(m_filePath).fileName();
}

QString EditorWidget::getIndentForNewLine(const QString& currentLine) {
    // Get existing indentation
    QString indent;
    for (QChar c : currentLine) {
        if (c == ' ' || c == '\t') {
            indent += c;
        } else {
            break;
        }
    }
    
    QString trimmedLine = currentLine.trimmed();
    
    if (m_fileType == TASM) {
        // For TASM: if current line is a label (ends with :), add one tab
        if (trimmedLine.endsWith(':') && !trimmedLine.startsWith(';')) {
            return indent + "\t";
        }
    } else if (m_fileType == TML) {
        // For TML: if current line ends with {, add one tab
        if (trimmedLine.endsWith('{')) {
            return indent + "\t";
        }
    }
    
    return indent;
}

void EditorWidget::handleAutoIndent() {
    QTextCursor cursor = textCursor();
    
    // Get the current line before the cursor
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString currentLine = cursor.selectedText();
    
    // Reset cursor to where enter was pressed
    cursor = textCursor();
    
    QString indent = getIndentForNewLine(currentLine);
    
    // Insert newline and indent
    cursor.insertText("\n" + indent);
    setTextCursor(cursor);
}

void EditorWidget::keyPressEvent(QKeyEvent* e) {
    // Handle completion popup - Ctrl+Space to manually trigger
    if (e->key() == Qt::Key_Space && e->modifiers() & Qt::ControlModifier) {
        showCompletionPopup();
        return;
    }
    
    // Handle Enter key for auto-indent
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        
        // Check if cursor is between {} brackets (for TML bracket expansion)
        if (m_fileType == TML) {
            int pos = cursor.position();
            QString text = toPlainText();
            if (pos > 0 && pos < text.length()) {
                QChar charBefore = text[pos - 1];
                QChar charAfter = text[pos];
                if (charBefore == '{' && charAfter == '}') {
                    // Get current line's indentation
                    cursor.movePosition(QTextCursor::StartOfBlock);
                    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                    QString currentLine = cursor.selectedText();
                    QString indent;
                    for (QChar c : currentLine) {
                        if (c == ' ' || c == '\t') {
                            indent += c;
                        } else {
                            break;
                        }
                    }
                    
                    // Insert newline with indent, then another newline with closing bracket
                    cursor = textCursor();
                    cursor.insertText("\n" + indent + "\t\n" + indent);
                    // Move cursor back to the indented line
                    cursor.movePosition(QTextCursor::Up);
                    cursor.movePosition(QTextCursor::EndOfBlock);
                    setTextCursor(cursor);
                    return;
                }
            }
        }
        
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString currentLine = cursor.selectedText();
        
        // Reset and do auto-indent
        cursor = textCursor();
        QString trimmedLine = currentLine.trimmed();
        
        // Calculate indent for new line
        QString indent;
        for (QChar c : currentLine) {
            if (c == ' ' || c == '\t') {
                indent += c;
            } else {
                break;
            }
        }
        
        if (m_fileType == TASM) {
            // If line is a label, add indent
            if (trimmedLine.endsWith(':') && !trimmedLine.startsWith(';')) {
                indent += "\t";
            }
        } else if (m_fileType == TML) {
            // If line ends with {, add indent
            if (trimmedLine.endsWith('{')) {
                indent += "\t";
            }
        }
        
        cursor.insertText("\n" + indent);
        setTextCursor(cursor);
        return;
    }
    
    // Handle { in TML to auto-insert closing bracket
    if (m_fileType == TML && e->text() == "{") {
        // Check if we need to auto-insert a closing bracket
        if (shouldAutoCloseBracket()) {
            QTextCursor cursor = textCursor();
            int pos = cursor.position();
            cursor.insertText("{}");
            cursor.setPosition(pos + 1);  // Position cursor between brackets
            setTextCursor(cursor);
            return;
        }
    }
    
    // Handle } in TML to auto-unindent
    if (m_fileType == TML && e->text() == "}") {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString currentLine = cursor.selectedText();
        
        // If line is only whitespace, remove one level of indent
        if (currentLine.trimmed().isEmpty() && !currentLine.isEmpty()) {
            cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            
            // Remove one tab or 4 spaces from the beginning
            if (currentLine.startsWith('\t')) {
                currentLine = currentLine.mid(1);
            } else if (currentLine.startsWith("    ")) {
                currentLine = currentLine.mid(4);
            }
            
            cursor.insertText(currentLine + "}");
            return;
        }
    }
    
    // Handle : in TASM to auto-unindent for labels
    if (m_fileType == TASM && e->text() == ":") {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString currentLine = cursor.selectedText();
        QString trimmed = currentLine.trimmed();
        
        // If creating a label (word followed by colon, with only whitespace before)
        QRegularExpression labelPattern("^\\s+([a-zA-Z_][a-zA-Z0-9_]*)$");
        QRegularExpressionMatch match = labelPattern.match(currentLine);
        if (match.hasMatch()) {
            // Remove leading whitespace and insert the label with colon
            cursor = textCursor();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.insertText(match.captured(1) + ":");
            return;
        }
    }
    
    QPlainTextEdit::keyPressEvent(e);
    
    // Auto-trigger completion popup when typing (after the character is inserted)
    if (m_fileType != Unknown && !e->text().isEmpty()) {
        QChar ch = e->text().at(0);
        // Trigger on letters and underscores (start of identifiers)
        if (ch.isLetter() || ch == '_') {
            QTimer::singleShot(0, this, &EditorWidget::maybeShowCompletion);
        }
    }
}

void EditorWidget::showCompletionPopup() {
    if (completion) {
        completion->showAtCursor(cursorRect());
    }
}

void EditorWidget::maybeShowCompletion() {
    if (!completion || m_fileType == Unknown) return;
    
    // Get current word prefix
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    QString prefix = cursor.selectedText();
    
    // Only show completion if we have at least 2 characters
    if (prefix.length() >= 2) {
        completion->showAtCursor(cursorRect());
    }
}

bool EditorWidget::shouldAutoCloseBracket() {
    // Count open and close brackets from cursor position to end of document
    // at the same nesting level
    QString text = toPlainText();
    QTextCursor cursor = textCursor();
    int cursorPos = cursor.position();
    
    // Count bracket balance from cursor to end
    int depth = 0;
    bool foundMatchingClose = false;
    
    for (int i = cursorPos; i < text.length(); i++) {
        QChar ch = text[i];
        if (ch == '{') {
            depth++;
        } else if (ch == '}') {
            if (depth == 0) {
                // Found a closing bracket at our level
                foundMatchingClose = true;
                break;
            }
            depth--;
        }
    }
    
    // If no matching close bracket found at same depth, we should auto-close
    return !foundMatchingClose;
}

void EditorWidget::onTextChanged() {
    // Ignore changes during file loading
    if (m_isLoading) {
        return;
    }
    
    // Check if content actually differs from original
    bool currentlyModified = (toPlainText() != m_originalContent);
    
    if (currentlyModified != m_isModified) {
        m_isModified = currentlyModified;
        emit modificationChanged(m_isModified);
    }
}
