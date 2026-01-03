#pragma once
#include <QDialog>
#include <QList>
#include <QString>
#include <QMap>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QComboBox;

struct TangentProgram {
    QString name;        // Display name (project name)
    QString filePath;    // Full path to .tp file
    bool selected;       // Whether to include in ROM
};

struct TangentRomVersion {
    QString name;        // Display name (e.g., "V6.7")
    QString filePath;    // Full path to ROM file
    QString startMem;    // 5-digit hex start memory address
    bool isCustom;       // Whether this is a custom ROM
};

class RomBuilderDialog : public QDialog {
    Q_OBJECT
public:
    explicit RomBuilderDialog(const QString& workspacePath, QWidget* parent = nullptr);
    
    // Get the result command line for combiner
    QString getCombinerCommand() const;
    QStringList getSelectedPrograms() const;
    QString getSelectedRomPath() const;
    QString getInjectAddress() const;
    QString getOutputName() const;

private slots:
    void onNextFromPrograms();
    void onNextFromRomSelect();
    void onBack();
    void onBrowseCustomRom();
    void onRomSelectionChanged();
    void onCreate();
    void onCancel();

private:
    void setupProgramSelectionPage();
    void setupRomSelectionPage();
    void setupSettingsPage();
    void loadPrograms();
    void loadRomVersions();
    QString extractStartMemFromFilename(const QString& filename) const;
    QString extractVersionFromFilename(const QString& filename) const;
    void updateNavigationButtons();
    
    QString m_workspacePath;
    
    // Pages
    QStackedWidget* m_stackedWidget;
    
    // Page 1: Program Selection
    QListWidget* m_programList;
    
    // Page 2: ROM Version Selection
    QListWidget* m_romList;
    QPushButton* m_customRomButton;
    QString m_customRomPath;
    
    // Page 3: Settings
    QLineEdit* m_injectAddressEdit;
    QLabel* m_defaultAddressLabel;
    
    // Navigation buttons
    QPushButton* m_backButton;
    QPushButton* m_nextButton;
    QPushButton* m_cancelButton;
    
    // Data
    QList<TangentProgram> m_programs;
    QList<TangentRomVersion> m_romVersions;
    int m_selectedRomIndex;
};
