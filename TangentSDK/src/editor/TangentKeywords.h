#pragma once
#include <QStringList>
#include <QString>
#include <QSet>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QMap>
#include <QFileInfo>

// Shared keywords for TASM and TML files
// Used by both SyntaxHighlighter and CompletionPopup

namespace TangentKeywords {

// TASM instructions/keywords
inline QStringList tasmKeywords() {
    return {
        "mov", "add", "sub", "mul", "div", "cmp", "and", "or", "xor", "not", "shl", "shr",
        "b", "bl", "beq", "bne", "blt", "bgt", "ble", "bge", "bles",
        "push", "pop", "call", "ret", "nop", "halt",
        "ldr", "str", "ldm", "stm", "swp",
        "adc", "sbc", "neg", "abs",
        "tst", "teq", "bic", "mvn"
    };
}

// TASM registers
inline QStringList tasmRegisters() {
    QStringList regs;
    // r0-r15
    for (int i = 0; i <= 15; i++) {
        regs << QString("r%1").arg(i);
    }
    // er0, er2, er4, er6, er8, er10, er12, er14
    for (int i = 0; i <= 14; i += 2) {
        regs << QString("er%1").arg(i);
    }
    // xr0, xr4, xr8, xr12
    for (int i = 0; i <= 12; i += 4) {
        regs << QString("xr%1").arg(i);
    }
    // qr0, qr8
    regs << "qr0" << "qr8";
    return regs;
}

// TML keywords/elements
inline QStringList tmlKeywords() {
    return {
        "root", "text", "button", "input", "label", "panel", "container",
        "image", "list", "grid", "scroll", "checkbox", "radio", "slider",
        "dropdown", "menu", "menuitem", "toolbar", "statusbar",
        "window", "dialog", "popup", "tooltip", "tab", "tabpanel"
    };
}

// TML fields/properties
inline QStringList tmlFields() {
    return {
        "x", "y", "width", "height", "colour", "color", "background", "foreground",
        "font", "fontsize", "fontweight", "padding", "margin", "border",
        "visible", "enabled", "text", "value", "id", "name", "class", "style",
        "onclick", "onchange", "onhover", "onfocus", "onblur",
        "align", "valign", "wrap", "overflow", "opacity", "zindex"
    };
}

// Extract labels from a .tasm file
inline QStringList extractLabelsFromFile(const QString& filePath) {
    QStringList labels;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QRegularExpression labelExpr("^\\s*([a-zA-Z_][a-zA-Z0-9_]*):");
        while (!in.atEnd()) {
            QString line = in.readLine();
            QRegularExpressionMatch match = labelExpr.match(line);
            if (match.hasMatch()) {
                labels << match.captured(1);
            }
        }
        file.close();
    }
    return labels;
}

// Get all labels from all .tasm files in a project folder
inline QStringList getProjectLabels(const QString& projectPath) {
    QSet<QString> labelSet;
    
    if (projectPath.isEmpty()) return {};
    
    QDirIterator it(projectPath, QStringList() << "*.tasm", 
                    QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        // Skip files in build folder
        if (filePath.contains("/build/")) continue;
        
        QStringList fileLabels = extractLabelsFromFile(filePath);
        for (const QString& label : fileLabels) {
            labelSet.insert(label);
        }
    }
    
    return labelSet.values();
}

// Get all labels with their source file names (for completion popup)
inline QMap<QString, QString> getProjectLabelsWithFiles(const QString& projectPath) {
    QMap<QString, QString> labelMap;
    
    if (projectPath.isEmpty()) return labelMap;
    
    QDirIterator it(projectPath, QStringList() << "*.tasm", 
                    QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        // Skip files in build folder
        if (filePath.contains("/build/")) continue;
        
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        
        QStringList fileLabels = extractLabelsFromFile(filePath);
        for (const QString& label : fileLabels) {
            // If label already exists from another file, append the file name
            if (labelMap.contains(label)) {
                QString existing = labelMap[label];
                if (!existing.contains(fileName)) {
                    labelMap[label] = existing + ", " + fileName;
                }
            } else {
                labelMap[label] = fileName;
            }
        }
    }
    
    return labelMap;
}

// Build regex pattern for keywords
inline QString buildKeywordPattern(const QStringList& keywords) {
    return "\\b(" + keywords.join("|") + ")\\b";
}

} // namespace TangentKeywords
