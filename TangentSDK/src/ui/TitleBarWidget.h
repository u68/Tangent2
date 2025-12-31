#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPoint>

class QDockWidget;

// Custom title bar for dock widgets
class TitleBarWidget : public QWidget {
    Q_OBJECT
public:
    TitleBarWidget(const QString& title, QDockWidget* dockWidget, QWidget* parent = nullptr);
    void setTitle(const QString& title);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onCloseClicked();
    void onFloatClicked();

private:
    QLabel* m_titleLabel;
    QPushButton* m_closeButton;
    QPushButton* m_floatButton;
    QDockWidget* m_dockWidget;
    QPoint m_dragStartPos;
    bool m_isDragging = false;
};

// Container widget that wraps content with a title bar
class DockContentWidget : public QWidget {
    Q_OBJECT
public:
    DockContentWidget(const QString& title, QWidget* content, QDockWidget* dockWidget, QWidget* parent = nullptr);
    
private:
    TitleBarWidget* m_titleBar;
    QWidget* m_content;
};
