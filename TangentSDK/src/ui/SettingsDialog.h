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
#include <QListWidget>
#include <QInputDialog>
#include "../editor/SyntaxDefinition.h"

// Struct to track color settings in the UI
struct ColorSetting {
    QString langKey;    // e.g., "tasm", "tml", "common", "console"
    QString ruleKey;    // e.g., "keyword", "comment", etc.
    QColor color;
    bool bold;
};

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    
    // Getters for settings
    QString workspacePath() const;
    bool autoSaveEnabled() const;
    
    // Completion settings
    bool completionEnabled() const;
    int completionMinChars() const;
    
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
    
    // Extension management slots
    void onAddExtension();
    void onCreateExtension();
    void onEditDefaultJson();
    void onExtensionMoveUp();
    void onExtensionMoveDown();
    void onExtensionToggle(QTreeWidgetItem* item, int column);
    void onExtensionContextMenu(const QPoint& pos);
    void onExtensionDelete();

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
    void refreshExtensionList();
    bool checkExtensionOrderWarning(int fromIndex, int toIndex);
    
    // General settings
    QLineEdit* m_workspaceEdit;
    QCheckBox* m_autoSaveCheck;
    
    // Completion settings
    QCheckBox* m_completionEnabledCheck;
    QSpinBox* m_completionMinCharsSpinBox;
    
    // Editor settings
    QComboBox* m_fontCombo;
    QSpinBox* m_fontSizeSpinBox;
    QWidget* m_fontWidget;
    QTreeWidget* m_colorTree;
    QMap<QString, ColorSetting> m_colorSettings;  // key -> color setting
    
    // Extension management
    QTreeWidget* m_extensionTree;
    QPushButton* m_extMoveUpBtn;
    QPushButton* m_extMoveDownBtn;
    
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
