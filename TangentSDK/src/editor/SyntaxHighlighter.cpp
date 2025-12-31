#include "SyntaxHighlighter.h"
#include "TangentKeywords.h"
#include "../theme/SyntaxTheme.h"
#include <QRegularExpression>
#include <QFileInfo>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc) {
    setupFormats();
}

void SyntaxHighlighter::applyFormat(QTextCharFormat& format, const QString& key) {
    SyntaxTheme::ColorEntry entry = SyntaxTheme::instance().getColor(key);
    format.setForeground(entry.color);
    format.setFontWeight(entry.bold ? QFont::Bold : QFont::Normal);
}

void SyntaxHighlighter::setupFormats() {
    // TASM formats - load from SyntaxTheme
    applyFormat(tasmKeyword, "syntax.tasm.keyword");
    applyFormat(tasmRegister, "syntax.tasm.register");
    applyFormat(tasmLabel, "syntax.tasm.label");
    applyFormat(tasmComment, "syntax.tasm.comment");
    applyFormat(tasmAddress, "syntax.tasm.address");
    applyFormat(tasmDirective, "syntax.tasm.directive");
    
    // TML formats
    applyFormat(tmlKeyword, "syntax.tml.keyword");
    applyFormat(tmlBracket[0], "syntax.tml.bracket0");
    applyFormat(tmlBracket[1], "syntax.tml.bracket1");
    applyFormat(tmlBracket[2], "syntax.tml.bracket2");
    applyFormat(tmlField, "syntax.tml.field");
    applyFormat(tmlLabelRef, "syntax.tml.labelRef");
    
    // Common formats
    applyFormat(numberFormat, "syntax.number");
    applyFormat(stringFormat, "syntax.string");
}

void SyntaxHighlighter::reloadColorsFromSettings() {
    setupFormats();
    rehighlight();
}
void SyntaxHighlighter::setFileType(const QString& filePath) {
    QFileInfo info(filePath);
    setFileTypeFromExtension(info.suffix().toLower());
}

void SyntaxHighlighter::setFileTypeFromExtension(const QString& extension) {
    if (extension == "tasm") {
        m_fileType = TASM;
    } else if (extension == "tml") {
        m_fileType = TML;
    } else {
        m_fileType = Unknown;
    }
    rehighlight();
}

void SyntaxHighlighter::setProjectPath(const QString& projectPath) {
    m_projectPath = projectPath;
    refreshProjectLabels();
}

void SyntaxHighlighter::refreshProjectLabels() {
    m_projectLabels = TangentKeywords::getProjectLabels(m_projectPath);
    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString& text) {
    switch (m_fileType) {
        case TASM:
            highlightTASM(text);
            break;
        case TML:
            highlightTML(text);
            break;
        default:
            break;
    }
}

void SyntaxHighlighter::highlightTASM(const QString& text) {
    // Comments (everything after ;) - do first so other rules don't override
    QRegularExpression commentExpr(";.*$");
    QRegularExpressionMatchIterator commentIt = commentExpr.globalMatch(text);
    int commentStart = -1;
    while (commentIt.hasNext()) {
        QRegularExpressionMatch match = commentIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmComment);
        commentStart = match.capturedStart();
    }
    
    // Only highlight non-comment portions
    QString activeText = (commentStart >= 0) ? text.left(commentStart) : text;
    
    // Directives (@import, @define, etc. at start of line)
    QRegularExpression directiveExpr("^\\s*(@[a-zA-Z_][a-zA-Z0-9_]*)");
    QRegularExpressionMatch directiveMatch = directiveExpr.match(activeText);
    if (directiveMatch.hasMatch()) {
        setFormat(directiveMatch.capturedStart(1), directiveMatch.capturedLength(1), tasmDirective);
    }
    
    // Labels (word followed by :)
    QRegularExpression labelExpr("\\b([a-zA-Z_][a-zA-Z0-9_]*):");
    QRegularExpressionMatchIterator labelIt = labelExpr.globalMatch(activeText);
    while (labelIt.hasNext()) {
        QRegularExpressionMatch match = labelIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmLabel);
    }
    
    // TASM keywords/instructions using shared keywords
    QString keywordPattern = TangentKeywords::buildKeywordPattern(TangentKeywords::tasmKeywords());
    QRegularExpression keywordExpr(keywordPattern, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator keywordIt = keywordExpr.globalMatch(activeText);
    while (keywordIt.hasNext()) {
        QRegularExpressionMatch match = keywordIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmKeyword);
    }
    
    // Registers: r0-r15
    QRegularExpression regExpr("\\b(r(1[0-5]|[0-9]))\\b", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator regIt = regExpr.globalMatch(activeText);
    while (regIt.hasNext()) {
        QRegularExpressionMatch match = regIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmRegister);
    }
    
    // Extended registers: er0, er2, er4, er6, er8, er10, er12, er14
    QRegularExpression erExpr("\\b(er(14|12|10|[02468]))\\b", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator erIt = erExpr.globalMatch(activeText);
    while (erIt.hasNext()) {
        QRegularExpressionMatch match = erIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmRegister);
    }
    
    // XR registers: xr0, xr4, xr8, xr12
    QRegularExpression xrExpr("\\b(xr(12|[048]))\\b", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator xrIt = xrExpr.globalMatch(activeText);
    while (xrIt.hasNext()) {
        QRegularExpressionMatch match = xrIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmRegister);
    }
    
    // QR registers: qr0, qr8
    QRegularExpression qrExpr("\\b(qr[08])\\b", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator qrIt = qrExpr.globalMatch(activeText);
    while (qrIt.hasNext()) {
        QRegularExpressionMatch match = qrIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmRegister);
    }
    
    // Addresses (starting with #)
    QRegularExpression addrExpr("#[a-zA-Z0-9_]+");
    QRegularExpressionMatchIterator addrIt = addrExpr.globalMatch(activeText);
    while (addrIt.hasNext()) {
        QRegularExpressionMatch match = addrIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tasmAddress);
    }
    
    // Numbers (decimal and hex)
    QRegularExpression numExpr("\\b(0x[0-9a-fA-F]+|[0-9]+)\\b");
    QRegularExpressionMatchIterator numIt = numExpr.globalMatch(activeText);
    while (numIt.hasNext()) {
        QRegularExpressionMatch match = numIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), numberFormat);
    }
    
    // Strings
    QRegularExpression strExpr("\"[^\"]*\"");
    QRegularExpressionMatchIterator strIt = strExpr.globalMatch(activeText);
    while (strIt.hasNext()) {
        QRegularExpressionMatch match = strIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), stringFormat);
    }
}

void SyntaxHighlighter::highlightTML(const QString& text) {
    // TML keywords/elements using shared keywords
    QString keywordPattern = TangentKeywords::buildKeywordPattern(TangentKeywords::tmlKeywords());
    QRegularExpression keywordExpr(keywordPattern, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator keywordIt = keywordExpr.globalMatch(text);
    while (keywordIt.hasNext()) {
        QRegularExpressionMatch match = keywordIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tmlKeyword);
    }
    
    // Curly brackets with nesting colors
    // Get the bracket depth from the previous block
    int bracketDepth = previousBlockState();
    if (bracketDepth < 0) bracketDepth = 0;
    
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == '{') {
            setFormat(i, 1, tmlBracket[bracketDepth % 3]);
            bracketDepth++;
        } else if (text[i] == '}') {
            if (bracketDepth > 0) bracketDepth--;
            setFormat(i, 1, tmlBracket[bracketDepth % 3]);
        }
    }
    
    // Store the bracket depth for the next block
    setCurrentBlockState(bracketDepth);
    
    // TML fields/properties using shared keywords
    QString fieldPattern = TangentKeywords::buildKeywordPattern(TangentKeywords::tmlFields());
    QRegularExpression fieldExpr(fieldPattern, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator fieldIt = fieldExpr.globalMatch(text);
    while (fieldIt.hasNext()) {
        QRegularExpressionMatch match = fieldIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), tmlField);
    }
    
    // Highlight project labels (from .tasm files)
    if (!m_projectLabels.isEmpty()) {
        QString labelPattern = TangentKeywords::buildKeywordPattern(m_projectLabels);
        QRegularExpression labelExpr(labelPattern);
        QRegularExpressionMatchIterator labelIt = labelExpr.globalMatch(text);
        while (labelIt.hasNext()) {
            QRegularExpressionMatch match = labelIt.next();
            setFormat(match.capturedStart(), match.capturedLength(), tmlLabelRef);
        }
    }
    
    // Numbers
    QRegularExpression numExpr("\\b(0x[0-9a-fA-F]+|[0-9]+\\.?[0-9]*)\\b");
    QRegularExpressionMatchIterator numIt = numExpr.globalMatch(text);
    while (numIt.hasNext()) {
        QRegularExpressionMatch match = numIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), numberFormat);
    }
    
    // Strings (both single and double quotes)
    QRegularExpression strExpr("(\"[^\"]*\"|'[^']*')");
    QRegularExpressionMatchIterator strIt = strExpr.globalMatch(text);
    while (strIt.hasNext()) {
        QRegularExpressionMatch match = strIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), stringFormat);
    }
}
