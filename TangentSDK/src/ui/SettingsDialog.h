#pragma once
#include <QDialog>
#include <QSettings>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QTreeWidget>
#include <QColorDialog>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QMap>
#include <QButtonGroup>
#include "../theme/SyntaxTheme.h"

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    
    // Getters for settings
    QString workspacePath() const;
    bool autoSaveEnabled() const;
    
    // Discord settings
    bool discordEnabled() const;
    bool discordCustomTextEnabled() const;
    QString discordDetailsFormat() const;
    QString discordStateFormat() const;
    int discordElapsedTimeReset() const; // 0=startup, 1=new project, 2=new file
    
signals:
    void settingsChanged();
    void workspacePathChanged(const QString& newPath);
    void colorsChanged();
    void discordSettingsChanged();

private slots:
    void onBrowseWorkspace();
    void onColorItemClicked(QTreeWidgetItem* item, int column);
    void onColorContextMenu(const QPoint& pos);
    void onFontContextMenu(const QPoint& pos);
    void onBoldToggled(QTreeWidgetItem* item, int column);
    void onResetDefaultColors();
    void onResetFont();
    void onBrowseEmulatorPath();
    void onBrowseModelFolder();
    void onBrowseRP2EasePath();
    void onApply();
    void onOk();

private:
    void setupGeneralTab(QWidget* tab);
    void setupEditorTab(QWidget* tab);
    void setupDiscordTab(QWidget* tab);
    void setupRunDebugTab(QWidget* tab);
    void loadSettings();
    void saveSettings();
    void updateColorPreview(QTreeWidgetItem* item, const QColor& color, bool bold);
    QString getPasswordHexString() const;
    QString getClockspeedHexString() const;
    
    // General settings
    QLineEdit* m_workspaceEdit;
    QCheckBox* m_autoSaveCheck;
    
    // Editor settings
    QComboBox* m_fontCombo;
    QSpinBox* m_fontSizeSpinBox;
    QWidget* m_fontWidget;
    QTreeWidget* m_colorTree;
    QMap<QString, SyntaxTheme::ColorEntry> m_colors;
    
    // Discord settings
    QCheckBox* m_discordEnabledCheck;
    QCheckBox* m_discordCustomTextCheck;
    QLineEdit* m_discordDetailsEdit;
    QLineEdit* m_discordStateEdit;
    QRadioButton* m_elapsedStartupRadio;
    QRadioButton* m_elapsedProjectRadio;
    QRadioButton* m_elapsedFileRadio;
    QWidget* m_discordCustomGroup;
    
    QTabWidget* m_tabWidget;
    
    // Run/Debug settings
    QLineEdit* m_emulatorPathEdit;
    QLineEdit* m_modelFolderEdit;
    QLineEdit* m_rp2easePathEdit;
    QLineEdit* m_customPasswordEdit;
    QRadioButton* m_passwordFFRadio;
    QRadioButton* m_password00Radio;
    QRadioButton* m_passwordCasioRadio;
    QRadioButton* m_passwordCustomRadio;
    QButtonGroup* m_passwordGroup;
    QRadioButton* m_clockDefaultRadio;
    QRadioButton* m_clockOverclockRadio;
    QRadioButton* m_clockCustomRadio;
    QLineEdit* m_customClockEdit;
    QButtonGroup* m_clockGroup;
};
