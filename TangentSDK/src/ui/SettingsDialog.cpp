#include "SettingsDialog.h"
#include "../editor/SyntaxDefinition.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QMenu>
#include <QFontDatabase>
#include <QInputDialog>
#include <functional>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    
    setWindowTitle("Settings");
    setMinimumSize(650, 550);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget(this);
    
    // General tab
    QWidget* generalTab = new QWidget();
    setupGeneralTab(generalTab);
    m_tabWidget->addTab(generalTab, "General");
    
    // Editor tab (with font and color settings)
    QWidget* editorTab = new QWidget();
    setupEditorTab(editorTab);
    m_tabWidget->addTab(editorTab, "Editor");
    
    // Discord tab
    QWidget* discordTab = new QWidget();
    setupDiscordTab(discordTab);
    m_tabWidget->addTab(discordTab, "Discord");
    
    // Run/Debug tab
    QWidget* runDebugTab = new QWidget();
    setupRunDebugTab(runDebugTab);
    m_tabWidget->addTab(runDebugTab, "Run/Debug");
    
    mainLayout->addWidget(m_tabWidget);
    
    // Button box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onOk);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, 
            this, &SettingsDialog::onApply);
    
    mainLayout->addWidget(buttonBox);
    
    loadSettings();
}

void SettingsDialog::setupGeneralTab(QWidget* tab) {
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Workspace section
    QGroupBox* workspaceGroup = new QGroupBox("Workspace");
    QHBoxLayout* wsLayout = new QHBoxLayout(workspaceGroup);
    
    QLabel* wsLabel = new QLabel("Workspace Folder:");
    m_workspaceEdit = new QLineEdit();
    QPushButton* browseBtn = new QPushButton("Browse...");
    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseWorkspace);
    
    wsLayout->addWidget(wsLabel);
    wsLayout->addWidget(m_workspaceEdit, 1);
    wsLayout->addWidget(browseBtn);
    
    layout->addWidget(workspaceGroup);
    
    // Auto-save section
    QGroupBox* saveGroup = new QGroupBox("Saving");
    QVBoxLayout* saveLayout = new QVBoxLayout(saveGroup);
    
    m_autoSaveCheck = new QCheckBox("Enable Auto-Save (saves on focus loss)");
    saveLayout->addWidget(m_autoSaveCheck);
    
    layout->addWidget(saveGroup);
    
    layout->addStretch();
}

void SettingsDialog::setupEditorTab(QWidget* tab) {
    QVBoxLayout* tabLayout = new QVBoxLayout(tab);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(scrollContent);
    
    // Font section
    QGroupBox* fontGroup = new QGroupBox("Font");
    QHBoxLayout* fontLayout = new QHBoxLayout(fontGroup);
    
    QLabel* fontLabel = new QLabel("Font Family:");
    m_fontCombo = new QComboBox();
    
    // Populate with monospace fonts
    QFontDatabase fontDb;
    QStringList families = fontDb.families();
    for (const QString& family : families) {
        if (fontDb.isFixedPitch(family)) {
            m_fontCombo->addItem(family);
        }
    }
    
    QLabel* sizeLabel = new QLabel("Size:");
    m_fontSizeSpinBox = new QSpinBox();
    m_fontSizeSpinBox->setRange(6, 72);
    m_fontSizeSpinBox->setValue(11);
    
    fontLayout->addWidget(fontLabel);
    fontLayout->addWidget(m_fontCombo, 1);
    fontLayout->addWidget(sizeLabel);
    fontLayout->addWidget(m_fontSizeSpinBox);
    
    // Enable right-click context menu for font settings
    m_fontWidget = fontGroup;
    fontGroup->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fontGroup, &QWidget::customContextMenuRequested, this, &SettingsDialog::onFontContextMenu);
    
    layout->addWidget(fontGroup);
    
    // Code Completion section
    QGroupBox* completionGroup = new QGroupBox("Code Completion");
    QVBoxLayout* completionLayout = new QVBoxLayout(completionGroup);
    
    m_completionEnabledCheck = new QCheckBox("Enable auto-completion popup");
    completionLayout->addWidget(m_completionEnabledCheck);
    
    QHBoxLayout* minCharsLayout = new QHBoxLayout();
    QLabel* minCharsLabel = new QLabel("Minimum characters to trigger:");
    m_completionMinCharsSpinBox = new QSpinBox();
    m_completionMinCharsSpinBox->setRange(1, 10);
    m_completionMinCharsSpinBox->setValue(1);
    minCharsLayout->addWidget(minCharsLabel);
    minCharsLayout->addWidget(m_completionMinCharsSpinBox);
    minCharsLayout->addStretch();
    completionLayout->addLayout(minCharsLayout);
    
    // Enable/disable min chars spinbox based on checkbox
    connect(m_completionEnabledCheck, &QCheckBox::toggled, m_completionMinCharsSpinBox, &QSpinBox::setEnabled);
    
    layout->addWidget(completionGroup);
    
    // Syntax Colors section
    QGroupBox* colorsGroup = new QGroupBox("Syntax Highlighting Colors");
    QVBoxLayout* colorsLayout = new QVBoxLayout(colorsGroup);
    
    QLabel* infoLabel = new QLabel("Click on a color to change it. Check the Bold box to make text bold.");
    infoLabel->setWordWrap(true);
    colorsLayout->addWidget(infoLabel);
    
    m_colorTree = new QTreeWidget();
    m_colorTree->setHeaderLabels({"Setting", "Color", "Bold"});
    m_colorTree->setColumnWidth(0, 200);
    m_colorTree->setColumnWidth(1, 100);
    m_colorTree->setColumnWidth(2, 50);
    m_colorTree->setAlternatingRowColors(false);
    m_colorTree->setStyleSheet(
        "QTreeWidget { background-color: #2d2d2d; }"
        "QTreeWidget::item { padding: 4px; }"
        "QTreeWidget::item:hover { background-color: #3a3a3a; }"
        "QTreeWidget::indicator:checked { background-color: #c42b1c; border: 1px solid #c42b1c; border-radius: 2px; }"
        "QTreeWidget::indicator:unchecked { background-color: transparent; border: 1px solid #888888; border-radius: 2px; }"
    );
    m_colorTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_colorTree, &QTreeWidget::itemClicked, this, &SettingsDialog::onColorItemClicked);
    connect(m_colorTree, &QTreeWidget::itemChanged, this, &SettingsDialog::onBoldToggled);
    connect(m_colorTree, &QTreeWidget::customContextMenuRequested, this, &SettingsDialog::onColorContextMenu);
    
    SyntaxDefinition& def = SyntaxDefinition::instance();
    
    auto addColorItem = [this, &def](QTreeWidgetItem* parent, const QString& langKey, const QString& ruleKey) {
        SyntaxRule rule;
        QString key = langKey + "." + ruleKey;
        
        if (langKey == "common") {
            rule = def.getCommonRule(ruleKey);
        } else if (langKey == "console") {
            rule = def.getConsoleRule(ruleKey);
        } else {
            rule = def.getRule(langKey, ruleKey);
        }
        
        QTreeWidgetItem* item = new QTreeWidgetItem(parent, {rule.displayName});
        item->setData(0, Qt::UserRole, key);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        
        ColorSetting setting;
        setting.langKey = langKey;
        setting.ruleKey = ruleKey;
        setting.color = rule.color;
        setting.bold = rule.bold;
        m_colorSettings[key] = setting;
        
        updateColorPreview(item, rule.color, rule.bold);
    };
    
    // TASM category
    QTreeWidgetItem* tasmCategory = new QTreeWidgetItem(m_colorTree, {"Tangent Assembly (TASM)"});
    tasmCategory->setFlags(tasmCategory->flags() & ~Qt::ItemIsSelectable);
    tasmCategory->setExpanded(true);
    
    addColorItem(tasmCategory, "tasm", "keyword");
    addColorItem(tasmCategory, "tasm", "register");
    addColorItem(tasmCategory, "tasm", "label");
    addColorItem(tasmCategory, "tasm", "comment");
    addColorItem(tasmCategory, "tasm", "address");
    addColorItem(tasmCategory, "tasm", "directive");
    
    // TML category
    QTreeWidgetItem* tmlCategory = new QTreeWidgetItem(m_colorTree, {"Tangent Markup (TML)"});
    tmlCategory->setFlags(tmlCategory->flags() & ~Qt::ItemIsSelectable);
    tmlCategory->setExpanded(true);
    
    addColorItem(tmlCategory, "tml", "keyword");
    addColorItem(tmlCategory, "tml", "field");
    addColorItem(tmlCategory, "tml", "labelRef");
    addColorItem(tmlCategory, "tml", "bracket0");
    addColorItem(tmlCategory, "tml", "bracket1");
    addColorItem(tmlCategory, "tml", "bracket2");
    
    // Common category
    QTreeWidgetItem* commonCategory = new QTreeWidgetItem(m_colorTree, {"Common"});
    commonCategory->setFlags(commonCategory->flags() & ~Qt::ItemIsSelectable);
    commonCategory->setExpanded(true);
    
    addColorItem(commonCategory, "common", "number");
    addColorItem(commonCategory, "common", "string");
    
    // Console category
    QTreeWidgetItem* consoleCategory = new QTreeWidgetItem(m_colorTree, {"Console Output"});
    consoleCategory->setFlags(consoleCategory->flags() & ~Qt::ItemIsSelectable);
    consoleCategory->setExpanded(true);
    
    addColorItem(consoleCategory, "console", "error");
    addColorItem(consoleCategory, "console", "warning");
    addColorItem(consoleCategory, "console", "success");
    addColorItem(consoleCategory, "console", "normal");
    
    colorsLayout->addWidget(m_colorTree);
    
    // Reset button
    QPushButton* resetBtn = new QPushButton("Reset to Defaults");
    connect(resetBtn, &QPushButton::clicked, this, &SettingsDialog::onResetDefaultColors);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(resetBtn);
    colorsLayout->addLayout(btnLayout);
    
    layout->addWidget(colorsGroup, 1);
    
    // Syntax Extensions section
    QGroupBox* extensionsGroup = new QGroupBox("Syntax Extensions");
    QVBoxLayout* extensionsLayout = new QVBoxLayout(extensionsGroup);
    
    QLabel* extInfoLabel = new QLabel(
        "Extensions at the top override extensions below them. "
        "Default should be at the bottom, your settings above it.");
    extInfoLabel->setWordWrap(true);
    extensionsLayout->addWidget(extInfoLabel);
    
    // Extension list with buttons
    QHBoxLayout* extListLayout = new QHBoxLayout();
    
    m_extensionTree = new QTreeWidget();
    m_extensionTree->setHeaderLabels({"Extension", "Enabled"});
    m_extensionTree->setColumnWidth(0, 250);
    m_extensionTree->setColumnWidth(1, 60);
    m_extensionTree->setRootIsDecorated(false);
    m_extensionTree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_extensionTree->setStyleSheet(
        "QTreeWidget { background-color: #2d2d2d; }"
        "QTreeWidget::item { padding: 4px; }"
        "QTreeWidget::item:hover { background-color: #3a3a3a; }"
        "QTreeWidget::indicator:checked { background-color: #c42b1c; border: 1px solid #c42b1c; border-radius: 2px; }"
        "QTreeWidget::indicator:unchecked { background-color: transparent; border: 1px solid #888888; border-radius: 2px; }"
    );
    connect(m_extensionTree, &QTreeWidget::customContextMenuRequested, 
            this, &SettingsDialog::onExtensionContextMenu);
    connect(m_extensionTree, &QTreeWidget::itemChanged,
            this, &SettingsDialog::onExtensionToggle);
    extListLayout->addWidget(m_extensionTree, 1);
    
    // Move up/down buttons
    QVBoxLayout* extBtnLayout = new QVBoxLayout();
    m_extMoveUpBtn = new QPushButton("▲");
    m_extMoveUpBtn->setFixedWidth(30);
    m_extMoveUpBtn->setToolTip("Move Up (Higher Priority)");
    connect(m_extMoveUpBtn, &QPushButton::clicked, this, &SettingsDialog::onExtensionMoveUp);
    
    m_extMoveDownBtn = new QPushButton("▼");
    m_extMoveDownBtn->setFixedWidth(30);
    m_extMoveDownBtn->setToolTip("Move Down (Lower Priority)");
    connect(m_extMoveDownBtn, &QPushButton::clicked, this, &SettingsDialog::onExtensionMoveDown);
    
    extBtnLayout->addWidget(m_extMoveUpBtn);
    extBtnLayout->addWidget(m_extMoveDownBtn);
    extBtnLayout->addStretch();
    extListLayout->addLayout(extBtnLayout);
    
    extensionsLayout->addLayout(extListLayout);
    
    // Extension action buttons
    QHBoxLayout* extActionLayout = new QHBoxLayout();
    
    QPushButton* addExtBtn = new QPushButton("Import Extension...");
    addExtBtn->setToolTip("Import a JSON extension file");
    connect(addExtBtn, &QPushButton::clicked, this, &SettingsDialog::onAddExtension);
    
    QPushButton* createExtBtn = new QPushButton("Create Extension...");
    createExtBtn->setToolTip("Create a new blank extension");
    connect(createExtBtn, &QPushButton::clicked, this, &SettingsDialog::onCreateExtension);
    
    QPushButton* editDefaultBtn = new QPushButton("Edit Default JSON...");
    editDefaultBtn->setToolTip("Open the default syntax definitions file");
    connect(editDefaultBtn, &QPushButton::clicked, this, &SettingsDialog::onEditDefaultJson);
    
    extActionLayout->addWidget(addExtBtn);
    extActionLayout->addWidget(createExtBtn);
    extActionLayout->addWidget(editDefaultBtn);
    extActionLayout->addStretch();
    
    extensionsLayout->addLayout(extActionLayout);
    
    layout->addWidget(extensionsGroup);
    
    // Finish scroll area setup
    scrollArea->setWidget(scrollContent);
    tabLayout->addWidget(scrollArea);
    
    // Load extensions into list
    refreshExtensionList();
}

void SettingsDialog::setupDiscordTab(QWidget* tab) {
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Enable Discord
    m_discordEnabledCheck = new QCheckBox("Enable Discord Rich Presence");
    layout->addWidget(m_discordEnabledCheck);
    
    // Custom text section
    QGroupBox* customGroup = new QGroupBox("Custom Display Text");
    QVBoxLayout* customLayout = new QVBoxLayout(customGroup);
    
    m_discordCustomTextCheck = new QCheckBox("Use custom text format");
    customLayout->addWidget(m_discordCustomTextCheck);
    
    m_discordCustomGroup = new QWidget();
    QVBoxLayout* customFieldsLayout = new QVBoxLayout(m_discordCustomGroup);
    customFieldsLayout->setContentsMargins(0, 0, 0, 0);
    
    // Variable help
    QLabel* helpLabel = new QLabel(
        "<b>Available variables:</b><br>"
        "<code>${file}</code> - Full file name (e.g., main.tasm)<br>"
        "<code>${file.baseName}</code> - File name without extension (e.g., main)<br>"
        "<code>${file.extension}</code> - File extension (e.g., tasm)<br>"
        "<code>${language}</code> - Programming language (Tangent Assembly / Tangent Markup)<br>"
        "<code>${project}</code> - Project folder name"
    );
    helpLabel->setWordWrap(true);
    helpLabel->setTextFormat(Qt::RichText);
    helpLabel->setMinimumHeight(100);
    customFieldsLayout->addWidget(helpLabel);
    
    // Details field (first line)
    QLabel* detailsLabel = new QLabel("Details (Line 1):");
    m_discordDetailsEdit = new QLineEdit();
    m_discordDetailsEdit->setPlaceholderText("e.g., Editing ${file}");
    customFieldsLayout->addWidget(detailsLabel);
    customFieldsLayout->addWidget(m_discordDetailsEdit);
    
    // State field (second line)
    QLabel* stateLabel = new QLabel("State (Line 2):");
    m_discordStateEdit = new QLineEdit();
    m_discordStateEdit->setPlaceholderText("e.g., Working on ${project}");
    customFieldsLayout->addWidget(stateLabel);
    customFieldsLayout->addWidget(m_discordStateEdit);
    
    customLayout->addWidget(m_discordCustomGroup);
    layout->addWidget(customGroup);
    
    // Elapsed time reset
    QGroupBox* elapsedGroup = new QGroupBox("Reset Elapsed Time");
    QVBoxLayout* elapsedLayout = new QVBoxLayout(elapsedGroup);
    
    m_elapsedStartupRadio = new QRadioButton("On IDE startup");
    m_elapsedProjectRadio = new QRadioButton("When opening a new project");
    m_elapsedFileRadio = new QRadioButton("When opening a new file");
    
    elapsedLayout->addWidget(m_elapsedStartupRadio);
    elapsedLayout->addWidget(m_elapsedProjectRadio);
    elapsedLayout->addWidget(m_elapsedFileRadio);
    
    layout->addWidget(elapsedGroup);
    
    layout->addStretch();
    
    // Connect signals
    connect(m_discordCustomTextCheck, &QCheckBox::toggled, m_discordCustomGroup, &QWidget::setEnabled);
    m_discordCustomGroup->setEnabled(false);
}

void SettingsDialog::setupRunDebugTab(QWidget* tab) {
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Emulator Paths section
    QGroupBox* pathsGroup = new QGroupBox("Paths");
    QFormLayout* pathsLayout = new QFormLayout(pathsGroup);
    
    // CasioEmuMSVC path
    QHBoxLayout* emuLayout = new QHBoxLayout();
    m_emulatorPathEdit = new QLineEdit();
    m_emulatorPathEdit->setPlaceholderText("path/to/CasioEmuMSVC.exe");
    QPushButton* browseEmuBtn = new QPushButton("Browse...");
    connect(browseEmuBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseEmulatorPath);
    emuLayout->addWidget(m_emulatorPathEdit, 1);
    emuLayout->addWidget(browseEmuBtn);
    pathsLayout->addRow("Emulator Executable:", emuLayout);
    
    // Model folder path
    QHBoxLayout* modelLayout = new QHBoxLayout();
    m_modelFolderEdit = new QLineEdit();
    m_modelFolderEdit->setPlaceholderText("path/to/model/folder");
    QPushButton* browseModelBtn = new QPushButton("Browse...");
    connect(browseModelBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseModelFolder);
    modelLayout->addWidget(m_modelFolderEdit, 1);
    modelLayout->addWidget(browseModelBtn);
    pathsLayout->addRow("Model Folder:", modelLayout);
    
    // RP2Ease path
    QHBoxLayout* rp2easeLayout = new QHBoxLayout();
    m_rp2easePathEdit = new QLineEdit();
    m_rp2easePathEdit->setPlaceholderText("path/to/rp2ease.exe");
    QPushButton* browseRP2EaseBtn = new QPushButton("Browse...");
    connect(browseRP2EaseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowseRP2EasePath);
    rp2easeLayout->addWidget(m_rp2easePathEdit, 1);
    rp2easeLayout->addWidget(browseRP2EaseBtn);
    pathsLayout->addRow("RP2Ease Executable:", rp2easeLayout);
    
    layout->addWidget(pathsGroup);
    
    // Chip Password section
    QGroupBox* passwordGroup = new QGroupBox("Chip Password (32 bytes)");
    QVBoxLayout* passwordLayout = new QVBoxLayout(passwordGroup);
    
    QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    
    m_passwordGroup = new QButtonGroup(this);
    m_passwordFFRadio = new QRadioButton("FF (all 0xFF bytes)");
    m_password00Radio = new QRadioButton("00 (all 0x00 bytes)");
    m_passwordCasioRadio = new QRadioButton("Casio Default Password");
    m_passwordCustomRadio = new QRadioButton("Custom");
    m_passwordGroup->addButton(m_passwordFFRadio, 0);
    m_passwordGroup->addButton(m_password00Radio, 1);
    m_passwordGroup->addButton(m_passwordCasioRadio, 2);
    m_passwordGroup->addButton(m_passwordCustomRadio, 3);
    m_passwordFFRadio->setChecked(true);
    
    passwordLayout->addWidget(m_passwordFFRadio);
    passwordLayout->addWidget(m_password00Radio);
    passwordLayout->addWidget(m_passwordCasioRadio);
    passwordLayout->addWidget(m_passwordCustomRadio);
    
    // Custom password input (64 hex chars = 32 bytes)
    m_customPasswordEdit = new QLineEdit();
    m_customPasswordEdit->setPlaceholderText("Enter hex password (max 64 hex characters = 32 bytes)");
    m_customPasswordEdit->setMaxLength(64);
    m_customPasswordEdit->setEnabled(false);
    m_customPasswordEdit->setFont(monoFont);
    // Only allow hex characters
    connect(m_customPasswordEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        QString filtered;
        for (const QChar& c : text) {
            if (c.isDigit() || (c.toLower() >= 'a' && c.toLower() <= 'f')) {
                filtered += c.toUpper();
            }
        }
        if (filtered != text) {
            m_customPasswordEdit->setText(filtered);
        }
    });
    passwordLayout->addWidget(m_customPasswordEdit);
    
    // Enable/disable custom password edit based on radio selection
    connect(m_passwordCustomRadio, &QRadioButton::toggled, m_customPasswordEdit, &QLineEdit::setEnabled);
    
    layout->addWidget(passwordGroup);
    
    // Clock Speed section
    QGroupBox* clockGroup = new QGroupBox("Clock Speed (16 bytes)");
    QVBoxLayout* clockLayout = new QVBoxLayout(clockGroup);
    
    m_clockGroup = new QButtonGroup(this);
    m_clockDefaultRadio = new QRadioButton("Default (2 MHz)");
    m_clockOverclockRadio = new QRadioButton("Overclocked (3.9 MHz with 21 MHz boost)");
    m_clockCustomRadio = new QRadioButton("Custom");
    m_clockGroup->addButton(m_clockDefaultRadio, 0);
    m_clockGroup->addButton(m_clockOverclockRadio, 1);
    m_clockGroup->addButton(m_clockCustomRadio, 2);
    m_clockDefaultRadio->setChecked(true);
    
    clockLayout->addWidget(m_clockDefaultRadio);
    clockLayout->addWidget(m_clockOverclockRadio);
    clockLayout->addWidget(m_clockCustomRadio);
    
    // Custom clock input
    m_customClockEdit = new QLineEdit();
    m_customClockEdit->setPlaceholderText("Enter 32 hex characters (16 bytes)");
    m_customClockEdit->setMaxLength(32);
    m_customClockEdit->setEnabled(false);
    m_customClockEdit->setFont(monoFont);
    // Only allow hex characters
    connect(m_customClockEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        QString filtered;
        for (const QChar& c : text) {
            if (c.isDigit() || (c.toLower() >= 'a' && c.toLower() <= 'f')) {
                filtered += c.toUpper();
            }
        }
        if (filtered != text) {
            m_customClockEdit->setText(filtered);
        }
    });
    clockLayout->addWidget(m_customClockEdit);
    
    // Enable/disable custom clock edit based on radio selection
    connect(m_clockCustomRadio, &QRadioButton::toggled, m_customClockEdit, &QLineEdit::setEnabled);
    
    layout->addWidget(clockGroup);
    
    layout->addStretch();
}

void SettingsDialog::onBrowseRP2EasePath() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select RP2Ease Executable",
                                                     m_rp2easePathEdit->text(),
                                                     "Executable (*.exe);;All Files (*)");
    if (!filePath.isEmpty()) {
        m_rp2easePathEdit->setText(filePath);
    }
}

void SettingsDialog::onBrowseEmulatorPath() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select CasioEmuMSVC Executable",
                                                     m_emulatorPathEdit->text(),
                                                     "Executable (*.exe);;All Files (*)");
    if (!filePath.isEmpty()) {
        m_emulatorPathEdit->setText(filePath);
    }
}

void SettingsDialog::onBrowseModelFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Model Folder",
                                                     m_modelFolderEdit->text());
    if (!dir.isEmpty()) {
        m_modelFolderEdit->setText(dir);
    }
}

void SettingsDialog::onFontContextMenu(const QPoint& pos) {
    QMenu contextMenu(this);
    QAction* resetAction = contextMenu.addAction("Reset Font Settings");
    
    QAction* selectedAction = contextMenu.exec(m_fontWidget->mapToGlobal(pos));
    if (selectedAction == resetAction) {
        onResetFont();
    }
}

void SettingsDialog::onResetFont() {
    // Reset to system default monospace font
    QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    int fontIndex = m_fontCombo->findText(defaultFont.family());
    if (fontIndex >= 0) {
        m_fontCombo->setCurrentIndex(fontIndex);
    }
    m_fontSizeSpinBox->setValue(11);
}

QString SettingsDialog::getPasswordHexString() const {
    if (m_passwordCustomRadio->isChecked()) {
        // Custom password - pad to 64 hex chars (32 bytes)
        QString password = m_customPasswordEdit->text().toUpper();
        while (password.length() < 64) {
            password += "0";
        }
        return password;
    } else if (m_password00Radio->isChecked()) {
        return QString("00").repeated(32); // 32 bytes = 64 hex chars
    } else if (m_passwordCasioRadio->isChecked()) {
        // Casio password: 37 31 43 41 53 47 61 59 71 67 79 73 FF FF FF FF 07 23 13 11 19 17 29 23 FF FF FF FF FF FF FF FF
        return "373143415347615971677973FFFFFFFF0723131119172923FFFFFFFFFFFFFFFF";
    } else {
        // FF password (default)
        return QString("FF").repeated(32); // 32 bytes = 64 hex chars
    }
}

QString SettingsDialog::getClockspeedHexString() const {
    if (m_clockCustomRadio->isChecked()) {
        // Custom clockspeed - pad to 32 hex chars (16 bytes)
        QString clock = m_customClockEdit->text().toUpper();
        while (clock.length() < 32) {
            clock += "0";
        }
        return clock;
    } else if (m_clockOverclockRadio->isChecked()) {
        // Overclock: 75 02 1E 0C 44 00 32 37 3B 21 10 00 7C 00 00 00
        return "75021E0C440032373B2110007C000000";
    } else {
        // Default (2MHz): EE FE 66 50 CE 75 25 F7 1F FF 64 58 7A 12 16 82
        return "EEFE6650CE7525F71FFF64587A121682";
    }
}

void SettingsDialog::loadSettings() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    // General
    m_workspaceEdit->setText(settings.value("workspacePath").toString());
    m_autoSaveCheck->setChecked(settings.value("autoSave", false).toBool());
    
    // Completion
    m_completionEnabledCheck->setChecked(settings.value("completion/enabled", true).toBool());
    m_completionMinCharsSpinBox->setValue(settings.value("completion/minChars", 1).toInt());
    m_completionMinCharsSpinBox->setEnabled(m_completionEnabledCheck->isChecked());
    
    // Editor - Font (load from QSettings)
    QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QString fontFamily = settings.value("editor/fontFamily", defaultFont.family()).toString();
    int fontSize = settings.value("editor/fontSize", 11).toInt();
    
    int fontIndex = m_fontCombo->findText(fontFamily);
    if (fontIndex >= 0) {
        m_fontCombo->setCurrentIndex(fontIndex);
    }
    m_fontSizeSpinBox->setValue(fontSize);
    
    // Editor - Colors (already loaded from SyntaxDefinition in setupEditorTab)
    // Update color tree with current values
    std::function<void(QTreeWidgetItem*)> updateTreeColors = [this, &updateTreeColors](QTreeWidgetItem* item) {
        QString key = item->data(0, Qt::UserRole).toString();
        if (!key.isEmpty() && m_colorSettings.contains(key)) {
            updateColorPreview(item, m_colorSettings[key].color, m_colorSettings[key].bold);
        }
        for (int i = 0; i < item->childCount(); ++i) {
            updateTreeColors(item->child(i));
        }
    };
    
    for (int i = 0; i < m_colorTree->topLevelItemCount(); ++i) {
        updateTreeColors(m_colorTree->topLevelItem(i));
    }
    
    // Discord
    m_discordEnabledCheck->setChecked(settings.value("discord/enabled", false).toBool());
    m_discordCustomTextCheck->setChecked(settings.value("discord/customText", false).toBool());
    m_discordDetailsEdit->setText(settings.value("discord/details", "Editing ${file}").toString());
    m_discordStateEdit->setText(settings.value("discord/state", "Working on ${project}").toString());
    
    int elapsedReset = settings.value("discord/elapsedReset", 0).toInt();
    switch (elapsedReset) {
        case 0: m_elapsedStartupRadio->setChecked(true); break;
        case 1: m_elapsedProjectRadio->setChecked(true); break;
        case 2: m_elapsedFileRadio->setChecked(true); break;
        default: m_elapsedStartupRadio->setChecked(true); break;
    }
    
    m_discordCustomGroup->setEnabled(m_discordCustomTextCheck->isChecked());
    
    // Run/Debug
    m_emulatorPathEdit->setText(settings.value("runDebug/emulatorPath").toString());
    m_modelFolderEdit->setText(settings.value("runDebug/modelFolder").toString());
    m_rp2easePathEdit->setText(settings.value("runDebug/rp2easePath").toString());
    
    m_customPasswordEdit->setText(settings.value("runDebug/customPasswordHex").toString());
    
    int passwordPreset = settings.value("runDebug/passwordPreset", 0).toInt();
    switch (passwordPreset) {
        case 0: m_passwordFFRadio->setChecked(true); break;
        case 1: m_password00Radio->setChecked(true); break;
        case 2: m_passwordCasioRadio->setChecked(true); break;
        case 3: m_passwordCustomRadio->setChecked(true); break;
        default: m_passwordFFRadio->setChecked(true); break;
    }
    m_customPasswordEdit->setEnabled(m_passwordCustomRadio->isChecked());
    
    int clockPreset = settings.value("runDebug/clockPreset", 0).toInt();
    switch (clockPreset) {
        case 0: m_clockDefaultRadio->setChecked(true); break;
        case 1: m_clockOverclockRadio->setChecked(true); break;
        case 2: m_clockCustomRadio->setChecked(true); break;
        default: m_clockDefaultRadio->setChecked(true); break;
    }
    m_customClockEdit->setText(settings.value("runDebug/customClockHex").toString());
    m_customClockEdit->setEnabled(m_clockCustomRadio->isChecked());
}

void SettingsDialog::saveSettings() {
    QSettings settings("TangentSDK", "TangentSDK");
    
    // General
    QString oldPath = settings.value("workspacePath").toString();
    QString newPath = m_workspaceEdit->text();
    
    if (oldPath != newPath) {
        settings.setValue("workspacePath", newPath);
        emit workspacePathChanged(newPath);
    }
    
    settings.setValue("autoSave", m_autoSaveCheck->isChecked());
    
    // Completion
    settings.setValue("completion/enabled", m_completionEnabledCheck->isChecked());
    settings.setValue("completion/minChars", m_completionMinCharsSpinBox->value());
    
    // Editor - Font
    settings.setValue("editor/fontFamily", m_fontCombo->currentText());
    settings.setValue("editor/fontSize", m_fontSizeSpinBox->value());
    
    // Editor - Colors: save to SyntaxDefinition
    SyntaxDefinition& def = SyntaxDefinition::instance();
    for (auto it = m_colorSettings.begin(); it != m_colorSettings.end(); ++it) {
        const ColorSetting& setting = it.value();
        if (setting.langKey == "common") {
            def.setCommonRuleColor(setting.ruleKey, setting.color);
            def.setCommonRuleBold(setting.ruleKey, setting.bold);
        } else if (setting.langKey == "console") {
            def.setConsoleRuleColor(setting.ruleKey, setting.color);
            def.setConsoleRuleBold(setting.ruleKey, setting.bold);
        } else {
            def.setRuleColor(setting.langKey, setting.ruleKey, setting.color);
            def.setRuleBold(setting.langKey, setting.ruleKey, setting.bold);
        }
    }
    def.save();
    
    // Discord
    settings.setValue("discord/enabled", m_discordEnabledCheck->isChecked());
    settings.setValue("discord/customText", m_discordCustomTextCheck->isChecked());
    settings.setValue("discord/details", m_discordDetailsEdit->text());
    settings.setValue("discord/state", m_discordStateEdit->text());
    
    int elapsedReset = 0;
    if (m_elapsedProjectRadio->isChecked()) elapsedReset = 1;
    else if (m_elapsedFileRadio->isChecked()) elapsedReset = 2;
    settings.setValue("discord/elapsedReset", elapsedReset);
    
    // Run/Debug
    settings.setValue("runDebug/emulatorPath", m_emulatorPathEdit->text());
    settings.setValue("runDebug/modelFolder", m_modelFolderEdit->text());
    settings.setValue("runDebug/rp2easePath", m_rp2easePathEdit->text());
    settings.setValue("runDebug/customPasswordHex", m_customPasswordEdit->text());
    
    int passwordPreset = 0;
    if (m_password00Radio->isChecked()) passwordPreset = 1;
    else if (m_passwordCasioRadio->isChecked()) passwordPreset = 2;
    else if (m_passwordCustomRadio->isChecked()) passwordPreset = 3;
    settings.setValue("runDebug/passwordPreset", passwordPreset);
    
    int clockPreset = 0;
    if (m_clockOverclockRadio->isChecked()) clockPreset = 1;
    else if (m_clockCustomRadio->isChecked()) clockPreset = 2;
    settings.setValue("runDebug/clockPreset", clockPreset);
    settings.setValue("runDebug/customClockHex", m_customClockEdit->text());
    
    emit settingsChanged();
    emit colorsChanged();
    emit discordSettingsChanged();
}

void SettingsDialog::updateColorPreview(QTreeWidgetItem* item, const QColor& color, bool bold) {
    // Create a pixmap with dark border for better visibility
    QPixmap pixmap(24, 24);
    pixmap.fill(QColor("#0a0a0a")); // Dark border background
    
    QPainter painter(&pixmap);
    painter.fillRect(2, 2, 20, 20, color);
    painter.end();
    
    item->setIcon(1, QIcon(pixmap));
    item->setText(1, color.name());
    
    // Block signals to prevent triggering onBoldToggled
    item->treeWidget()->blockSignals(true);
    item->setCheckState(2, bold ? Qt::Checked : Qt::Unchecked);
    item->treeWidget()->blockSignals(false);
}

void SettingsDialog::onBrowseWorkspace() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Workspace Folder",
                                                     m_workspaceEdit->text());
    if (!dir.isEmpty()) {
        m_workspaceEdit->setText(dir);
    }
}

void SettingsDialog::onColorItemClicked(QTreeWidgetItem* item, int column) {
    QString key = item->data(0, Qt::UserRole).toString();
    if (key.isEmpty()) return;
    
    // Only handle color column clicks (column 1)
    if (column != 1) return;
    
    if (!m_colorSettings.contains(key)) return;
    
    QColor currentColor = m_colorSettings[key].color;
    QColor newColor = QColorDialog::getColor(currentColor, this, "Select Color",
                                              QColorDialog::ShowAlphaChannel);
    
    if (newColor.isValid()) {
        m_colorSettings[key].color = newColor;
        updateColorPreview(item, newColor, m_colorSettings[key].bold);
    }
}

void SettingsDialog::onBoldToggled(QTreeWidgetItem* item, int column) {
    if (column != 2) return;
    
    QString key = item->data(0, Qt::UserRole).toString();
    if (key.isEmpty()) return;
    
    if (!m_colorSettings.contains(key)) return;
    
    bool bold = (item->checkState(2) == Qt::Checked);
    m_colorSettings[key].bold = bold;
}

void SettingsDialog::onColorContextMenu(const QPoint& pos) {
    QTreeWidgetItem* item = m_colorTree->itemAt(pos);
    if (!item) return;
    
    QString key = item->data(0, Qt::UserRole).toString();
    if (key.isEmpty()) return; // Category items don't have keys
    
    if (!m_colorSettings.contains(key)) return;
    
    QMenu contextMenu(this);
    QAction* resetAction = contextMenu.addAction("Reset Color");
    
    QAction* selectedAction = contextMenu.exec(m_colorTree->viewport()->mapToGlobal(pos));
    if (selectedAction == resetAction) {
        // Reload defaults from SyntaxDefinition
        ColorSetting& setting = m_colorSettings[key];
        SyntaxDefinition& def = SyntaxDefinition::instance();
        
        // Reset in SyntaxDefinition
        if (setting.langKey == "common") {
            def.resetCommonRule(setting.ruleKey);
            SyntaxRule rule = def.getCommonRule(setting.ruleKey);
            setting.color = rule.color;
            setting.bold = rule.bold;
        } else if (setting.langKey == "console") {
            def.resetConsoleRule(setting.ruleKey);
            SyntaxRule rule = def.getConsoleRule(setting.ruleKey);
            setting.color = rule.color;
            setting.bold = rule.bold;
        } else {
            def.resetRule(setting.langKey, setting.ruleKey);
            SyntaxRule rule = def.getRule(setting.langKey, setting.ruleKey);
            setting.color = rule.color;
            setting.bold = rule.bold;
        }
        
        updateColorPreview(item, setting.color, setting.bold);
    }
}

void SettingsDialog::onResetDefaultColors() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Reset Colors", 
        "Are you sure you want to reset all colors to their defaults?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Reset all customizations in SyntaxDefinition
        SyntaxDefinition& def = SyntaxDefinition::instance();
        def.resetAllToDefaults();
        
        // Reload colors from SyntaxDefinition
        m_colorSettings.clear();
        
        // Reload all color settings
        auto reloadColorItem = [this, &def](const QString& langKey, const QString& ruleKey) {
            QString key = langKey + "." + ruleKey;
            SyntaxRule rule;
            
            if (langKey == "common") {
                rule = def.getCommonRule(ruleKey);
            } else if (langKey == "console") {
                rule = def.getConsoleRule(ruleKey);
            } else {
                rule = def.getRule(langKey, ruleKey);
            }
            
            ColorSetting setting;
            setting.langKey = langKey;
            setting.ruleKey = ruleKey;
            setting.color = rule.color;
            setting.bold = rule.bold;
            m_colorSettings[key] = setting;
        };
        
        // Reload TASM
        reloadColorItem("tasm", "keyword");
        reloadColorItem("tasm", "register");
        reloadColorItem("tasm", "label");
        reloadColorItem("tasm", "comment");
        reloadColorItem("tasm", "address");
        reloadColorItem("tasm", "directive");
        
        // Reload TML
        reloadColorItem("tml", "keyword");
        reloadColorItem("tml", "field");
        reloadColorItem("tml", "labelRef");
        reloadColorItem("tml", "bracket0");
        reloadColorItem("tml", "bracket1");
        reloadColorItem("tml", "bracket2");
        
        // Reload common
        reloadColorItem("common", "number");
        reloadColorItem("common", "string");
        
        // Reload console
        reloadColorItem("console", "error");
        reloadColorItem("console", "warning");
        reloadColorItem("console", "success");
        reloadColorItem("console", "normal");
        
        // Update tree
        std::function<void(QTreeWidgetItem*)> updateTreeColors = [this, &updateTreeColors](QTreeWidgetItem* item) {
            QString key = item->data(0, Qt::UserRole).toString();
            if (!key.isEmpty() && m_colorSettings.contains(key)) {
                updateColorPreview(item, m_colorSettings[key].color, m_colorSettings[key].bold);
            }
            for (int i = 0; i < item->childCount(); ++i) {
                updateTreeColors(item->child(i));
            }
        };
        
        for (int i = 0; i < m_colorTree->topLevelItemCount(); ++i) {
            updateTreeColors(m_colorTree->topLevelItem(i));
        }
    }
}

void SettingsDialog::onApply() {
    saveSettings();
}

void SettingsDialog::onOk() {
    saveSettings();
    accept();
}

QString SettingsDialog::workspacePath() const {
    return m_workspaceEdit->text();
}

bool SettingsDialog::autoSaveEnabled() const {
    return m_autoSaveCheck->isChecked();
}

bool SettingsDialog::completionEnabled() const {
    return m_completionEnabledCheck->isChecked();
}

int SettingsDialog::completionMinChars() const {
    return m_completionMinCharsSpinBox->value();
}

bool SettingsDialog::discordEnabled() const {
    return m_discordEnabledCheck->isChecked();
}

bool SettingsDialog::discordCustomTextEnabled() const {
    return m_discordCustomTextCheck->isChecked();
}

QString SettingsDialog::discordDetailsFormat() const {
    return m_discordDetailsEdit->text();
}

QString SettingsDialog::discordStateFormat() const {
    return m_discordStateEdit->text();
}

int SettingsDialog::discordElapsedTimeReset() const {
    if (m_elapsedProjectRadio->isChecked()) return 1;
    if (m_elapsedFileRadio->isChecked()) return 2;
    return 0;
}

// Extension management methods
void SettingsDialog::refreshExtensionList() {
    m_extensionTree->blockSignals(true);
    m_extensionTree->clear();
    
    SyntaxDefinition& def = SyntaxDefinition::instance();
    QList<SyntaxExtension> extensions = def.getExtensions();
    
    // Display in normal order: lowest priority (default) at TOP, highest priority at BOTTOM
    // Items at the bottom override items above them
    for (int i = 0; i < extensions.size(); ++i) {
        const auto& ext = extensions[i];
        
        QString displayName = ext.name;
        if (ext.isDefault) {
            displayName += " (Built-in)";
        } else if (ext.isEdited) {
            displayName += " (Your Settings)";
        }
        
        QTreeWidgetItem* item = new QTreeWidgetItem(m_extensionTree, {displayName});
        item->setData(0, Qt::UserRole, ext.id);
        item->setData(0, Qt::UserRole + 1, ext.isDefault);
        item->setData(0, Qt::UserRole + 2, ext.isEdited);
        item->setData(0, Qt::UserRole + 3, i);  // Store original index
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(1, ext.enabled ? Qt::Checked : Qt::Unchecked);
        
        // Add visual indicators
        QString tooltip;
        if (ext.isDefault) {
            tooltip = "Built-in default syntax definitions.\nLowest priority - gets overridden by everything below.\nCannot be deleted.";
        } else if (ext.isEdited) {
            tooltip = "Your color and style customizations from the settings dialog.\nCannot be deleted.";
        } else {
            tooltip = "Custom extension: " + ext.filePath;
        }
        item->setToolTip(0, tooltip);
    }
    
    m_extensionTree->blockSignals(false);
}

bool SettingsDialog::checkExtensionOrderWarning(int fromIndex, int toIndex) {
    // In the UI: TOP = highest priority (overrides), BOTTOM = lowest priority (default)
    // We need to warn if:
    // - Edited is being moved BELOW Default (moving edited down = lower priority = would be overridden)
    // - Default is being moved ABOVE Edited (moving default up = higher priority = would override)
    
    QTreeWidgetItem* movingItem = m_extensionTree->topLevelItem(fromIndex);
    if (!movingItem) return true;
    
    bool isDefault = movingItem->data(0, Qt::UserRole + 1).toBool();
    bool isEdited = movingItem->data(0, Qt::UserRole + 2).toBool();
    
    // Moving edited DOWN (to higher index = lower visual position = lower priority)
    if (isEdited && toIndex > fromIndex) {
        // Check if default is between current position and target
        for (int i = fromIndex + 1; i <= toIndex; ++i) {
            QTreeWidgetItem* item = m_extensionTree->topLevelItem(i);
            if (item && item->data(0, Qt::UserRole + 1).toBool()) {
                // Edited would move below Default - warn!
                QMessageBox::StandardButton reply = QMessageBox::warning(
                    this, "Warning",
                    "Moving Editor Settings below Default will cause your color customizations "
                    "to be overridden by the default values.",
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel);
                return reply == QMessageBox::Ok;
            }
        }
    }
    
    // Moving default UP (to lower index = higher visual position = higher priority)
    if (isDefault && toIndex < fromIndex) {
        // Check if edited is between target and current position
        for (int i = toIndex; i < fromIndex; ++i) {
            QTreeWidgetItem* item = m_extensionTree->topLevelItem(i);
            if (item && item->data(0, Qt::UserRole + 2).toBool()) {
                // Default would move above Edited - warn!
                QMessageBox::StandardButton reply = QMessageBox::warning(
                    this, "Warning",
                    "Moving Default above Editor Settings will cause the default values "
                    "to override your color customizations.",
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel);
                return reply == QMessageBox::Ok;
            }
        }
    }
    
    return true;
}

void SettingsDialog::onExtensionMoveUp() {
    QTreeWidgetItem* currentItem = m_extensionTree->currentItem();
    if (!currentItem) return;
    
    int currentRow = m_extensionTree->indexOfTopLevelItem(currentItem);
    if (currentRow <= 0) return;
    
    if (!checkExtensionOrderWarning(currentRow, currentRow - 1)) return;
    
    // Get the current extensions - UI order now matches internal order
    SyntaxDefinition& def = SyntaxDefinition::instance();
    QList<SyntaxExtension> extensions = def.getExtensions();
    
    // Swap with the item above (lower priority)
    if (currentRow > 0 && currentRow < extensions.size()) {
        extensions.swapItemsAt(currentRow, currentRow - 1);
        def.setExtensions(extensions);
        refreshExtensionList();
        m_extensionTree->setCurrentItem(m_extensionTree->topLevelItem(currentRow - 1));
        emit colorsChanged();
    }
}

void SettingsDialog::onExtensionMoveDown() {
    QTreeWidgetItem* currentItem = m_extensionTree->currentItem();
    if (!currentItem) return;
    
    int currentRow = m_extensionTree->indexOfTopLevelItem(currentItem);
    if (currentRow < 0 || currentRow >= m_extensionTree->topLevelItemCount() - 1) return;
    
    if (!checkExtensionOrderWarning(currentRow, currentRow + 1)) return;
    
    // Get the current extensions - UI order now matches internal order
    SyntaxDefinition& def = SyntaxDefinition::instance();
    QList<SyntaxExtension> extensions = def.getExtensions();
    
    // Swap with the item below (higher priority)
    if (currentRow >= 0 && currentRow < extensions.size() - 1) {
        extensions.swapItemsAt(currentRow, currentRow + 1);
        def.setExtensions(extensions);
        refreshExtensionList();
        m_extensionTree->setCurrentItem(m_extensionTree->topLevelItem(currentRow + 1));
        emit colorsChanged();
    }
}

void SettingsDialog::onExtensionToggle(QTreeWidgetItem* item, int column) {
    if (!item) return;
    if (column != 1) return;  // Only handle clicks on the Enabled column
    
    QString id = item->data(0, Qt::UserRole).toString();
    bool enabled = (item->checkState(1) == Qt::Checked);
    
    SyntaxDefinition& def = SyntaxDefinition::instance();
    def.setExtensionEnabled(id, enabled);
    emit colorsChanged();
}

void SettingsDialog::onAddExtension() {
    QString filePath = QFileDialog::getOpenFileName(
        this, "Import Syntax Extension",
        QString(),
        "JSON Files (*.json);;All Files (*)");
    
    if (!filePath.isEmpty()) {
        SyntaxDefinition& def = SyntaxDefinition::instance();
        def.addExtension(filePath);
        refreshExtensionList();
        emit colorsChanged();
    }
}

void SettingsDialog::onCreateExtension() {
    bool ok;
    QString name = QInputDialog::getText(
        this, "Create Extension",
        "Extension Name:",
        QLineEdit::Normal,
        "My Extension",
        &ok);
    
    if (ok && !name.isEmpty()) {
        SyntaxDefinition& def = SyntaxDefinition::instance();
        QString filePath = def.createNewExtension(name);
        refreshExtensionList();
        
        // Offer to open the file for editing
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Extension Created",
            "Extension created successfully.\n\nWould you like to open it for editing?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            SyntaxDefinition::openExtensionInEditor(filePath);
        }
    }
}

void SettingsDialog::onEditDefaultJson() {
    SyntaxDefinition& def = SyntaxDefinition::instance();
    QString filePath = def.getDefaultFilePath();
    SyntaxDefinition::openExtensionInEditor(filePath);
}

void SettingsDialog::onExtensionContextMenu(const QPoint& pos) {
    QTreeWidgetItem* item = m_extensionTree->itemAt(pos);
    if (!item) return;
    
    QString id = item->data(0, Qt::UserRole).toString();
    bool isDefault = item->data(0, Qt::UserRole + 1).toBool();
    bool isEdited = item->data(0, Qt::UserRole + 2).toBool();
    
    QMenu menu(this);
    
    QAction* editAction = menu.addAction("Edit JSON...");
    connect(editAction, &QAction::triggered, this, [this, id]() {
        SyntaxDefinition& def = SyntaxDefinition::instance();
        QString filePath = def.getExtensionPath(id);
        if (!filePath.isEmpty()) {
            SyntaxDefinition::openExtensionInEditor(filePath);
        }
    });
    
    if (!isDefault && !isEdited) {
        menu.addSeparator();
        QAction* deleteAction = menu.addAction("Delete Extension");
        connect(deleteAction, &QAction::triggered, this, &SettingsDialog::onExtensionDelete);
    }
    
    menu.exec(m_extensionTree->viewport()->mapToGlobal(pos));
}

void SettingsDialog::onExtensionDelete() {
    QTreeWidgetItem* item = m_extensionTree->currentItem();
    if (!item) return;
    
    QString id = item->data(0, Qt::UserRole).toString();
    bool isDefault = item->data(0, Qt::UserRole + 1).toBool();
    bool isEdited = item->data(0, Qt::UserRole + 2).toBool();
    
    if (isDefault || isEdited) {
        QMessageBox::warning(this, "Cannot Delete",
            "Built-in extensions cannot be deleted.");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Extension",
        "Are you sure you want to delete this extension?\n\nThis will also delete the JSON file.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        SyntaxDefinition& def = SyntaxDefinition::instance();
        def.removeExtension(id);
        refreshExtensionList();
        emit colorsChanged();
    }
}
