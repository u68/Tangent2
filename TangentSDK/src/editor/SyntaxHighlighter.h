#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QColor>

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    enum FileType { Unknown, TASM, TML };
    
    SyntaxHighlighter(QTextDocument* doc);
    
    void setFileType(const QString& filePath);
    void setFileTypeFromExtension(const QString& extension);
    void setProjectPath(const QString& projectPath);
    void refreshProjectLabels();
    
    // Load colors from settings and re-apply
    void reloadColorsFromSettings();

protected:
    void highlightBlock(const QString& text) override;

private:
    FileType m_fileType = Unknown;
    QString m_projectPath;
    QStringList m_projectLabels;
    
    // TASM formats
    QTextCharFormat tasmKeyword;      // mov, add, sub, etc
    QTextCharFormat tasmRegister;     // r0-r15, er0-er14, etc
    QTextCharFormat tasmLabel;        // word:
    QTextCharFormat tasmComment;      // ; comment
    QTextCharFormat tasmAddress;      // #address
    QTextCharFormat tasmDirective;    // @import, @define, etc
    
    // TML formats
    QTextCharFormat tmlKeyword;       // root, text, button, etc
    QTextCharFormat tmlBracket[3];    // { } with nesting colors
    QTextCharFormat tmlField;         // x, width, colour, etc
    QTextCharFormat tmlLabelRef;      // references to TASM labels
    
    // Common formats
    QTextCharFormat numberFormat;     // 123
    QTextCharFormat stringFormat;     // "string"
    
    void highlightTASM(const QString& text);
    void highlightTML(const QString& text);
    void setupFormats();
    void applyFormat(QTextCharFormat& format, const QString& key);
};
