#include "SyntaxHighlighter.h"
#include "SyntaxDefinition.h"
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc) {
}

void SyntaxHighlighter::setupFormats() {
    m_formats.clear();
    SyntaxDefinition& def = SyntaxDefinition::instance();
    // Ensure syntax definitions are loaded (dev fallback)
    def.load();
    
    // Get language key from extension
    QString langKey = def.getLanguageKeyByExtension(m_fileExtension);
    if (langKey.isEmpty()) langKey = m_fileExtension;
    
    // Load formats for language-specific rules
    for (const QString& ruleKey : def.getRuleKeys(langKey)) {
        SyntaxRule rule = def.getRule(langKey, ruleKey);
        QTextCharFormat format;
        format.setForeground(rule.color);
        format.setFontWeight(rule.bold ? QFont::Bold : QFont::Normal);
        m_formats[langKey + "." + ruleKey] = format;
    }
    
    // Load common formats
    for (const QString& ruleKey : def.getCommonRuleKeys()) {
        SyntaxRule rule = def.getCommonRule(ruleKey);
        QTextCharFormat format;
        format.setForeground(rule.color);
        format.setFontWeight(rule.bold ? QFont::Bold : QFont::Normal);
        m_formats["common." + ruleKey] = format;
    }
    
    // Setup bracket formats for TML
    for (int i = 0; i < 3; i++) {
        SyntaxRule rule = def.getRule("tml", QString("bracket%1").arg(i));
        m_bracketFormats[i].setForeground(rule.color);
        m_bracketFormats[i].setFontWeight(rule.bold ? QFont::Bold : QFont::Normal);
    }
    
    buildHighlightRules();
}

void SyntaxHighlighter::buildHighlightRules() {
    m_rules.clear();
    SyntaxDefinition& def = SyntaxDefinition::instance();
    QString langKey = def.getLanguageKeyByExtension(m_fileExtension);
    if (langKey.isEmpty()) langKey = m_fileExtension;
    
    // Build rules for each syntax rule in the language
    for (const QString& ruleKey : def.getRuleKeys(langKey)) {
        SyntaxRule rule = def.getRule(langKey, ruleKey);
        QString formatKey = langKey + "." + ruleKey;
        
        // Skip dynamic rules (like labelRef) - handled separately
        if (rule.dynamic) continue;
        
        // Skip bracket rules for TML - handled specially
        if (langKey == "tml" && ruleKey.startsWith("bracket")) continue;
        
        if (!rule.pattern.isEmpty()) {
            HighlightRule hr;
            hr.pattern = QRegularExpression(rule.pattern);
            hr.formatKey = formatKey;
            m_rules.append(hr);
        }
        
        if (!rule.patterns.isEmpty()) {
            for (const QString& pattern : rule.patterns) {
                HighlightRule hr;
                hr.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
                hr.formatKey = formatKey;
                m_rules.append(hr);
            }
        }
        
        if (!rule.keywords.isEmpty()) {
            QString pattern = SyntaxDefinition::buildKeywordPattern(rule.keywords);
            HighlightRule hr;
            hr.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
            hr.formatKey = formatKey;
            m_rules.append(hr);
        }
    }
    
    // Add common rules (numbers, strings)
    for (const QString& ruleKey : def.getCommonRuleKeys()) {
        SyntaxRule rule = def.getCommonRule(ruleKey);
        if (!rule.pattern.isEmpty()) {
            HighlightRule hr;
            hr.pattern = QRegularExpression(rule.pattern);
            hr.formatKey = "common." + ruleKey;
            m_rules.append(hr);
        }
    }
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
    m_fileExtension = extension;
    
    if (extension == "tasm") {
        m_fileType = TASM;
    } else if (extension == "tml") {
        m_fileType = TML;
    } else {
        m_fileType = Unknown;
    }
    
    setupFormats();
    rehighlight();
}

void SyntaxHighlighter::setProjectPath(const QString& projectPath) {
    // Avoid redundant rescans
    if (m_projectPath == projectPath) return;
    m_projectPath = projectPath;
    refreshProjectLabels();
}

void SyntaxHighlighter::refreshProjectLabels() {
    // If no project path, clear and rehighlight immediately
    if (m_projectPath.isEmpty()) {
        m_projectLabels.clear();
        rehighlight();
        return;
    }

    // If a scan is already running, mark a pending refresh and return
    if (m_labelWatcher && m_labelWatcher->isRunning()) {
        m_pendingLabelRefresh = true;
        return;
    }

    // Lazily create watcher
    if (!m_labelWatcher) {
        m_labelWatcher = new QFutureWatcher<QStringList>(this);
        connect(m_labelWatcher, &QFutureWatcher<QStringList>::finished, this, &SyntaxHighlighter::onLabelScanFinished);
    }

    const QString projectPathCopy = m_projectPath;

    // Run the scan in a background thread
    qDebug() << "SyntaxHighlighter: starting label scan for" << projectPathCopy;
    auto future = QtConcurrent::run([projectPathCopy]() -> QStringList {
        QStringList labels;
        QDirIterator it(projectPathCopy, QStringList() << "*.tasm", QDir::Files, QDirIterator::Subdirectories);
        QRegularExpression labelExpr("^\\s*([a-zA-Z_][a-zA-Z0-9_]*):");
        while (it.hasNext()) {
            QString filePath = it.next();
            // Skip files in build folder
            if (filePath.contains("/build/") || filePath.contains("\\build\\")) continue;
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    QRegularExpressionMatch match = labelExpr.match(line);
                    if (match.hasMatch()) {
                        QString label = match.captured(1);
                        if (!labels.contains(label)) labels.append(label);
                    }
                }
                file.close();
            }
        }
        // Keep list sorted and unique
        labels.removeDuplicates();
        labels.sort(Qt::CaseInsensitive);
        return labels;
    });

    m_labelWatcher->setFuture(future);
    qDebug() << "SyntaxHighlighter: label scan scheduled";
}

void SyntaxHighlighter::onLabelScanFinished() {
    if (!m_labelWatcher) return;

    qDebug() << "SyntaxHighlighter: label scan finished";

    // If the future was canceled, ignore the result
    if (m_labelWatcher->isCanceled()) {
        qDebug() << "SyntaxHighlighter: label scan was canceled";
        if (m_pendingLabelRefresh) {
            m_pendingLabelRefresh = false;
            refreshProjectLabels();
        }
        return;
    }

    QStringList newLabels = m_labelWatcher->result();

    m_projectLabels = newLabels;
    rehighlight();

    // If a refresh was requested while scanning, run it now
    if (m_pendingLabelRefresh) {
        m_pendingLabelRefresh = false;
        refreshProjectLabels();
    }
}

SyntaxHighlighter::~SyntaxHighlighter() {
    // Ensure any running scan finishes/cancels before destruction to avoid wait condition errors
    if (m_labelWatcher) {
        if (m_labelWatcher->isRunning()) {
            m_labelWatcher->cancel();
            m_labelWatcher->waitForFinished();
        }
        // disconnect signals to be safe
        m_labelWatcher->disconnect(this);
        m_labelWatcher->deleteLater();
        m_labelWatcher = nullptr;
    }
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
            // Generic fallback: apply compiled rules from JSON for other languages
            for (const HighlightRule& rule : m_rules) {
                QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
                while (it.hasNext()) {
                    QRegularExpressionMatch match = it.next();
                    if (m_formats.contains(rule.formatKey)) {
                        setFormat(match.capturedStart(), match.capturedLength(), m_formats[rule.formatKey]);
                    }
                }
            }
            break;
    }
}

void SyntaxHighlighter::highlightTASM(const QString& text) {
    // Find comment start first (everything after ; is a comment)
    int commentStart = -1;
    QRegularExpression commentExpr(";.*$");
    QRegularExpressionMatch commentMatch = commentExpr.match(text);
    if (commentMatch.hasMatch()) {
        commentStart = commentMatch.capturedStart();
        setFormat(commentStart, commentMatch.capturedLength(), m_formats["tasm.comment"]);
    }
    
    // Only highlight non-comment portions
    QString activeText = (commentStart >= 0) ? text.left(commentStart) : text;
    
    // Apply all rules
    for (const HighlightRule& rule : m_rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(activeText);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            if (m_formats.contains(rule.formatKey)) {
                setFormat(match.capturedStart(), match.capturedLength(), m_formats[rule.formatKey]);
            }
        }
    }
}

void SyntaxHighlighter::highlightTML(const QString& text) {
    // Apply all non-bracket rules
    for (const HighlightRule& rule : m_rules) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            if (m_formats.contains(rule.formatKey)) {
                setFormat(match.capturedStart(), match.capturedLength(), m_formats[rule.formatKey]);
            }
        }
    }
    
    // Handle bracket nesting with colors
    int bracketDepth = previousBlockState();
    if (bracketDepth < 0) bracketDepth = 0;
    
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == '{') {
            setFormat(i, 1, m_bracketFormats[bracketDepth % 3]);
            bracketDepth++;
        } else if (text[i] == '}') {
            if (bracketDepth > 0) bracketDepth--;
            setFormat(i, 1, m_bracketFormats[bracketDepth % 3]);
        }
    }
    
    setCurrentBlockState(bracketDepth);
    
    // Highlight project labels (from .tasm files)
    if (!m_projectLabels.isEmpty()) {
        QString labelPattern = SyntaxDefinition::buildKeywordPattern(m_projectLabels);
        QRegularExpression labelExpr(labelPattern);
        QRegularExpressionMatchIterator labelIt = labelExpr.globalMatch(text);
        while (labelIt.hasNext()) {
            QRegularExpressionMatch match = labelIt.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_formats["tml.labelRef"]);
        }
    }
}
