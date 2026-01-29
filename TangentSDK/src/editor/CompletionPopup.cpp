#include "CompletionPopup.h"
#include "SyntaxDefinition.h"
#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTextCursor>
#include <QTimer>
#include <QCoreApplication>
#include <QApplication>
#include <QAbstractItemView>
#include <QScreen>
#include <QPainter>
#include <QFontDatabase>
#include <QtConcurrent/QtConcurrent>

// CompletionItemDelegate implementation
CompletionItemDelegate::CompletionItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

void CompletionItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const {
    painter->save();
    
    // Draw background
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor("#094771"));
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor("#2a2d2e"));
    } else {
        painter->fillRect(option.rect, QColor("#252526"));
    }
    
    // Get data
    QString text = index.data(Qt::DisplayRole).toString();
    QString fileInfo = index.data(CompletionPopup::FileInfoRole).toString();
    
    // Setup font
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(11);
    painter->setFont(monoFont);
    
    QRect textRect = option.rect.adjusted(6, 0, -6, 0);
    
    // Draw main text
    painter->setPen(QColor("#cccccc"));
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    
    // Draw file info on the right if present
    if (!fileInfo.isEmpty()) {
        painter->setPen(QColor("#707070"));
        painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, fileInfo);
    }
    
    painter->restore();
}

QSize CompletionItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);
    // Match editor line height (approximately font height + small padding)
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(11);
    QFontMetrics fm(monoFont);
    return QSize(200, fm.height() + 4);
}

CompletionPopup::CompletionPopup(QWidget* parent)
    : QListWidget(parent) {

    // Use ToolTip flag to avoid stealing focus from the editor
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_ShowWithoutActivating);
    
    // Set monospace font
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    monoFont.setPointSize(11);
    setFont(monoFont);
    
    // Style the popup - reduced padding
    setStyleSheet(
        "QListWidget { background-color: #252526; border: 1px solid #454545; }"
        "QListWidget::item { padding: 1px 4px; color: #cccccc; }"
        "QListWidget::item:selected { background-color: #094771; }"
        "QListWidget::item:hover { background-color: #2a2d2e; }"
    );
    
    // Set custom item delegate
    setItemDelegate(new CompletionItemDelegate(this));
    
    setMaximumHeight(200);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Handle both click and double-click for selection
    connect(this, &QListWidget::itemClicked, this, &CompletionPopup::insertCompletion);
    
    m_editor = qobject_cast<QPlainTextEdit*>(parent);
    if (m_editor) {
        m_editor->installEventFilter(this);
    }
}

void CompletionPopup::setFileType(FileType type) {
    m_fileType = type;
    buildCompletionList();
}

void CompletionPopup::setProjectPath(const QString& projectPath) {
    // Avoid re-scanning if unchanged
    if (m_projectPath == projectPath) return;
    m_projectPath = projectPath;

    // Kick off building completion list (fast parts)
    buildCompletionList();

    // Start async fetch of project labels (may be slow for large trees)
    if (!m_projectPath.isEmpty()) {
        if (!m_labelWatcher) {
            m_labelWatcher = new QFutureWatcher<QMap<QString,QString>>(this);
            connect(m_labelWatcher, &QFutureWatcher<QMap<QString,QString>>::finished,
                    this, &CompletionPopup::onLabelMapFinished);
        }

        // If a scan is running, set pending flag
        if (m_labelWatcher->isRunning()) {
            m_pendingLabelRefresh = true;
            return;
        }

        qDebug() << "CompletionPopup: starting label map scan for" << m_projectPath;
        auto future = QtConcurrent::run([path = m_projectPath]() -> QMap<QString,QString> {
            QMap<QString,QString> map;
            // Reuse SyntaxDefinition helper
            QMap<QString, QString> labels = SyntaxDefinition::getProjectLabelsWithFiles(path);
            for (auto it = labels.begin(); it != labels.end(); ++it) {
                map.insert(it.key(), it.value());
            }
            return map;
        });
        m_labelWatcher->setFuture(future);
        qDebug() << "CompletionPopup: label map scan scheduled";
    } else {
        // clear label map if project path empty
        m_labelFileMap.clear();
    }
}

void CompletionPopup::refreshCompletions() {
    buildCompletionList();
}

void CompletionPopup::buildCompletionList() {
    m_allCompletions.clear();
    
    SyntaxDefinition& def = SyntaxDefinition::instance();
    // Ensure definitions are loaded (robust for dev runs where files may be in repo)
    def.load();
    // If completions are missing, try to explicitly load default.json from repository paths
    auto tryLoadDefaultFromRepo = [&def]() -> bool {
        // Search upward from current working directory and application dir
        QStringList roots;
        roots << QDir::currentPath();
        roots << QCoreApplication::applicationDirPath();
        for (const QString& root : roots) {
            QDir walker(root);
            for (int i = 0; i < 8; ++i) {
                QString candidate = walker.absoluteFilePath("TangentSDK/src/resources/syntax/default.json");
                if (QFile::exists(candidate)) {
                    def.loadFromJsonFile(candidate, false);
                    return true;
                }
                candidate = walker.absoluteFilePath("src/resources/syntax/default.json");
                if (QFile::exists(candidate)) {
                    def.loadFromJsonFile(candidate, false);
                    return true;
                }
                walker.cdUp();
            }
        }
        return false;
    };
    
    if (m_fileType == TASM) {
        QStringList comps = def.getAllCompletions("tasm");
        if (comps.isEmpty()) {
            if (tryLoadDefaultFromRepo()) comps = def.getAllCompletions("tasm");
        }
        m_allCompletions << comps;
    } else if (m_fileType == TML) {
        QStringList comps = def.getAllCompletions("tml");
        if (comps.isEmpty()) {
            if (tryLoadDefaultFromRepo()) comps = def.getAllCompletions("tml");
        }
        m_allCompletions << comps;
    } else if (m_fileType == C) {
        QStringList comps = def.getAllCompletions("c");
        if (comps.isEmpty()) {
            if (tryLoadDefaultFromRepo()) comps = def.getAllCompletions("c");
        }
        m_allCompletions << comps;
    }

    // If label map is already available, include its keys
    if (!m_labelFileMap.isEmpty()) {
        m_allCompletions << m_labelFileMap.keys();
    }

    m_allCompletions.removeDuplicates();
    m_allCompletions.sort(Qt::CaseInsensitive);
}

QString CompletionPopup::getCurrentWordPrefix() {
    if (!m_editor) return QString();
    
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    return cursor.selectedText();
}

void CompletionPopup::addCompletionItem(const QString& text, const QString& fileInfo) {
    QListWidgetItem* item = new QListWidgetItem(text);
    if (!fileInfo.isEmpty()) {
        item->setData(FileInfoRole, fileInfo);
    }
    addItem(item);
}

void CompletionPopup::updateFilter(const QString& prefix) {
    clear();
    
    QString lowerPrefix = prefix.toLower();
    for (const QString& item : m_allCompletions) {
        if (item.toLower().startsWith(lowerPrefix)) {
            QString fileInfo = m_labelFileMap.value(item, QString());
            addCompletionItem(item, fileInfo);
        }
    }
    
    if (count() > 0) {
        setCurrentRow(0);
    }
}

void CompletionPopup::showAtCursor(const QRect& r) {
    if (!m_editor) return;

    buildCompletionList();
    QString prefix = getCurrentWordPrefix();
    updateFilter(prefix);

    if (count() == 0 && !prefix.isEmpty()) {
        // No matches found, don't show
        hide();
        return;
    }

    if (count() == 0) {
        // Show all completions if prefix is empty
        for (const QString& item : m_allCompletions) {
            QString fileInfo = m_labelFileMap.value(item, QString());
            addCompletionItem(item, fileInfo);
        }
    }

    if (count() > 0) {
        setCurrentRow(0);

        // Position below cursor in global coordinates
        QPoint pos = m_editor->mapToGlobal(r.bottomLeft());

        // Make sure popup doesn't go off screen
        QRect screenRect = QApplication::primaryScreen()->availableGeometry();

        // Resize to fit content
        int itemHeight = sizeHintForRow(0);
        if (itemHeight <= 0) itemHeight = 22; // fallback matching delegate
        int visibleItems = qMin(count(), 10);
        int popupHeight = itemHeight * visibleItems + 4;
        int popupWidth = qMax(280, sizeHintForColumn(0) + 100); // wider for file info

        // Adjust if popup would go off screen
        if (pos.x() + popupWidth > screenRect.right()) {
            pos.setX(screenRect.right() - popupWidth);
        }
        if (pos.y() + popupHeight > screenRect.bottom()) {
            // Show above the cursor instead
            pos = m_editor->mapToGlobal(r.topLeft());
            pos.setY(pos.y() - popupHeight);
        }

        setFixedHeight(popupHeight);
        setFixedWidth(popupWidth);
        move(pos);

        show();
        raise();
    } else {
        hide();
    }
}

// If label map is still loading, we will receive onLabelMapFinished and update the list then
void CompletionPopup::insertCompletion() {
    QListWidgetItem* item = currentItem();
    if (!item || !m_editor) {
        hide();
        return;
    }
    
    QString completion = item->text();
    QString prefix = getCurrentWordPrefix();
    
    QTextCursor cursor = m_editor->textCursor();
    // Select the current word prefix
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    // Replace with completion
    cursor.insertText(completion);
    
    m_editor->setTextCursor(cursor);
    hide();
}

void CompletionPopup::onLabelMapFinished() {
    if (!m_labelWatcher) return;

    if (m_labelWatcher->isCanceled()) {
        if (m_pendingLabelRefresh) {
            m_pendingLabelRefresh = false;
            // Restart scan
            setProjectPath(m_projectPath);
        }
        return;
    }

    QMap<QString, QString> result = m_labelWatcher->result();

    m_labelFileMap = result;

    // Rebuild completions to include labels
    buildCompletionList();

    // If popup is visible, refresh displayed items
    if (isVisible()) {
        QString prefix = getCurrentWordPrefix();
        updateFilter(prefix);
    }

    if (m_pendingLabelRefresh) {
        m_pendingLabelRefresh = false;
        // Restart scan
        setProjectPath(m_projectPath);
    }
}

CompletionPopup::~CompletionPopup() {
    if (m_labelWatcher) {
        if (m_labelWatcher->isRunning()) {
            m_labelWatcher->cancel();
            m_labelWatcher->waitForFinished();
        }
        m_labelWatcher->disconnect(this);
        m_labelWatcher->deleteLater();
        m_labelWatcher = nullptr;
    }
}

void CompletionPopup::keyPressEvent(QKeyEvent* event) {
    // Handle key presses when the popup itself has focus
    switch (event->key()) {
        case Qt::Key_Escape:
            hide();
            if (m_editor) m_editor->setFocus();
            return;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Tab:
            insertCompletion();
            return;
        case Qt::Key_Up: {
            int row = currentRow();
            if (row > 0) setCurrentRow(row - 1);
            return;
        }
        case Qt::Key_Down: {
            int row = currentRow();
            if (row < count() - 1) setCurrentRow(row + 1);
            return;
        }
        default:
            QListWidget::keyPressEvent(event);
            return;
    }
}

bool CompletionPopup::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_editor && isVisible()) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            
            // Handle navigation keys - intercept these
            if (keyEvent->key() == Qt::Key_Up) {
                int row = currentRow();
                if (row > 0) {
                    setCurrentRow(row - 1);
                }
                return true; // consume the event
            }
            
            if (keyEvent->key() == Qt::Key_Down) {
                int row = currentRow();
                if (row < count() - 1) {
                    setCurrentRow(row + 1);
                }
                return true; // consume the event
            }
            
            if (keyEvent->key() == Qt::Key_Return || 
                keyEvent->key() == Qt::Key_Enter ||
                keyEvent->key() == Qt::Key_Tab) {
                insertCompletion();
                return true; // consume the event
            }
            
            if (keyEvent->key() == Qt::Key_Escape) {
                hide();
                return true; // consume the event
            }
            
            // For character input, let editor handle it then update filter
            if (!keyEvent->text().isEmpty() || 
                keyEvent->key() == Qt::Key_Backspace ||
                keyEvent->key() == Qt::Key_Delete) {
                // Let the editor handle this key first
                QTimer::singleShot(0, this, [this]() {
                    if (isVisible() && m_editor) {
                        QString prefix = getCurrentWordPrefix();
                        if (prefix.isEmpty()) {
                            hide();
                        } else {
                            updateFilter(prefix);
                            if (count() == 0) {
                                hide();
                            }
                        }
                    }
                });
                return false; // let editor handle the key
            }
            
            // For other navigation keys (left, right, etc.), hide popup
            if (keyEvent->key() == Qt::Key_Left ||
                keyEvent->key() == Qt::Key_Right ||
                keyEvent->key() == Qt::Key_Home ||
                keyEvent->key() == Qt::Key_End) {
                hide();
                return false; // let editor handle
            }
        }
        
        // Hide popup if editor loses focus
        if (event->type() == QEvent::FocusOut) {
            QTimer::singleShot(100, this, [this]() {
                if (m_editor && !m_editor->hasFocus()) {
                    hide();
                }
            });
        }
    }
    return QListWidget::eventFilter(obj, event);
}
