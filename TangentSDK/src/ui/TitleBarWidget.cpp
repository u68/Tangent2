#include "TitleBarWidget.h"
#include <QDockWidget>
#include <QStyle>
#include <QFrame>
#include <QPalette>

TitleBarWidget::TitleBarWidget(const QString& title, QDockWidget* dockWidget, QWidget* parent)
    : QWidget(parent), m_dockWidget(dockWidget) {
    
    setFixedHeight(22);
    setAutoFillBackground(true);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 2, 4, 2);
    layout->setSpacing(4);
    
    // Title label
    m_titleLabel = new QLabel(title);
    m_titleLabel->setStyleSheet("QLabel { color: #cccccc; font-size: 11px; }");
    layout->addWidget(m_titleLabel);
    
    layout->addStretch();
    
    // Float/undock button - square and visible
    m_floatButton = new QPushButton("⧉");
    m_floatButton->setFixedSize(18, 18);
    m_floatButton->setToolTip("Float / Dock");
    m_floatButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #3a3a3a; "
        "  border: 1px solid #555555; "
        "  border-radius: 2px; "
        "  color: #cccccc; "
        "  font-size: 11px; "
        "} "
        "QPushButton:hover { "
        "  background-color: #505050; "
        "  border: 1px solid #666666; "
        "}"
    );
    connect(m_floatButton, &QPushButton::clicked, this, &TitleBarWidget::onFloatClicked);
    layout->addWidget(m_floatButton);
    
    // Close button - square and visible
    m_closeButton = new QPushButton("✕");
    m_closeButton->setFixedSize(18, 18);
    m_closeButton->setToolTip("Close");
    m_closeButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #3a3a3a; "
        "  border: 1px solid #555555; "
        "  border-radius: 2px; "
        "  color: #cccccc; "
        "  font-size: 11px; "
        "} "
        "QPushButton:hover { "
        "  background-color: #c42b1c; "
        "  border: 1px solid #c42b1c; "
        "}"
    );
    connect(m_closeButton, &QPushButton::clicked, this, &TitleBarWidget::onCloseClicked);
    layout->addWidget(m_closeButton);
    
    // Set background color lighter than widget content
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0x3a, 0x3a, 0x3a));
    setPalette(pal);
}

void TitleBarWidget::setTitle(const QString& title) {
    m_titleLabel->setText(title);
}

void TitleBarWidget::onCloseClicked() {
    if (m_dockWidget) {
        m_dockWidget->close();
    }
}

void TitleBarWidget::onFloatClicked() {
    if (m_dockWidget) {
        m_dockWidget->setFloating(!m_dockWidget->isFloating());
    }
}

void TitleBarWidget::mousePressEvent(QMouseEvent* event) {
    // Only handle dragging if already floating
    if (event->button() == Qt::LeftButton && m_dockWidget && m_dockWidget->isFloating()) {
        m_dragStartPos = event->globalPosition().toPoint();
        m_isDragging = true;
    }
    QWidget::mousePressEvent(event);
}

void TitleBarWidget::mouseMoveEvent(QMouseEvent* event) {
    // Only allow dragging when already floating
    if (m_isDragging && m_dockWidget && m_dockWidget->isFloating() && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->globalPosition().toPoint() - m_dragStartPos;
        m_dockWidget->move(m_dockWidget->pos() + delta);
        m_dragStartPos = event->globalPosition().toPoint();
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBarWidget::mouseReleaseEvent(QMouseEvent* event) {
    m_isDragging = false;
    QWidget::mouseReleaseEvent(event);
}

// DockContentWidget implementation
DockContentWidget::DockContentWidget(const QString& title, QWidget* content, QDockWidget* dockWidget, QWidget* parent)
    : QWidget(parent), m_content(content) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Title bar
    m_titleBar = new TitleBarWidget(title, dockWidget, this);
    layout->addWidget(m_titleBar);
    
    // Separator line
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("QFrame { background-color: #454545; max-height: 1px; }");
    layout->addWidget(separator);
    
    // Content
    if (content) {
        content->setParent(this);
        layout->addWidget(content);
    }
}
