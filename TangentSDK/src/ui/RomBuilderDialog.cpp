#include "RomBuilderDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QGroupBox>

RomBuilderDialog::RomBuilderDialog(const QString& workspacePath, QWidget* parent)
    : QDialog(parent)
    , m_workspacePath(workspacePath)
    , m_selectedRomIndex(-1)
{
    setWindowTitle("Build ROM");
    setMinimumSize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Stacked widget for pages
    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);
    
    // Setup pages
    setupProgramSelectionPage();
    setupRomSelectionPage();
    setupSettingsPage();
    
    // Navigation buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_backButton = new QPushButton("Back", this);
    m_nextButton = new QPushButton("Next", this);
    m_cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addWidget(m_backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_nextButton);
    mainLayout->addLayout(buttonLayout);
    
    // Connect navigation
    connect(m_backButton, &QPushButton::clicked, this, &RomBuilderDialog::onBack);
    connect(m_nextButton, &QPushButton::clicked, this, [this]() {
        int currentPage = m_stackedWidget->currentIndex();
        if (currentPage == 0) {
            onNextFromPrograms();
        } else if (currentPage == 1) {
            onNextFromRomSelect();
        } else if (currentPage == 2) {
            onCreate();
        }
    });
    connect(m_cancelButton, &QPushButton::clicked, this, &RomBuilderDialog::onCancel);
    
    // Load data
    loadPrograms();
    loadRomVersions();
    
    // Initial state
    updateNavigationButtons();
}

void RomBuilderDialog::setupProgramSelectionPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    
    QLabel* titleLabel = new QLabel("<b>Step 1: Select Tangent Programs</b>", page);
    layout->addWidget(titleLabel);
    
    QLabel* descLabel = new QLabel("Select which .tp (Tangent Program) files to include in the ROM:", page);
    layout->addWidget(descLabel);
    
    m_programList = new QListWidget(page);
    m_programList->setSelectionMode(QAbstractItemView::NoSelection);
    layout->addWidget(m_programList);
    
    // Select All / Deselect All buttons
    QHBoxLayout* selectLayout = new QHBoxLayout();
    QPushButton* selectAllBtn = new QPushButton("Select All", page);
    QPushButton* deselectAllBtn = new QPushButton("Deselect All", page);
    selectLayout->addWidget(selectAllBtn);
    selectLayout->addWidget(deselectAllBtn);
    selectLayout->addStretch();
    layout->addLayout(selectLayout);
    
    connect(selectAllBtn, &QPushButton::clicked, this, [this]() {
        for (int i = 0; i < m_programList->count(); ++i) {
            QListWidgetItem* item = m_programList->item(i);
            item->setCheckState(Qt::Checked);
        }
    });
    
    connect(deselectAllBtn, &QPushButton::clicked, this, [this]() {
        for (int i = 0; i < m_programList->count(); ++i) {
            QListWidgetItem* item = m_programList->item(i);
            item->setCheckState(Qt::Unchecked);
        }
    });
    
    m_stackedWidget->addWidget(page);
}

void RomBuilderDialog::setupRomSelectionPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    
    QLabel* titleLabel = new QLabel("<b>Step 2: Select Tangent ROM Version</b>", page);
    layout->addWidget(titleLabel);
    
    QLabel* descLabel = new QLabel("Select a base Tangent ROM version or use a custom ROM file:", page);
    layout->addWidget(descLabel);
    
    m_romList = new QListWidget(page);
    m_romList->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_romList);
    
    connect(m_romList, &QListWidget::itemSelectionChanged, this, &RomBuilderDialog::onRomSelectionChanged);
    
    // Custom ROM button
    QHBoxLayout* customLayout = new QHBoxLayout();
    m_customRomButton = new QPushButton("Browse Custom ROM (.bin)...", page);
    customLayout->addWidget(m_customRomButton);
    customLayout->addStretch();
    layout->addLayout(customLayout);
    
    connect(m_customRomButton, &QPushButton::clicked, this, &RomBuilderDialog::onBrowseCustomRom);
    
    m_stackedWidget->addWidget(page);
}

void RomBuilderDialog::setupSettingsPage() {
    QWidget* page = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(page);
    
    QLabel* titleLabel = new QLabel("<b>Step 3: Configure Settings</b>", page);
    layout->addWidget(titleLabel);
    
    // Inject Address
    QGroupBox* injectGroup = new QGroupBox("Inject Address", page);
    QVBoxLayout* injectLayout = new QVBoxLayout(injectGroup);
    
    QLabel* injectDesc = new QLabel("Enter the memory address where programs will be injected (5 hex digits):", injectGroup);
    injectLayout->addWidget(injectDesc);
    
    QHBoxLayout* addrLayout = new QHBoxLayout();
    QLabel* prefixLabel = new QLabel("0x", injectGroup);
    m_injectAddressEdit = new QLineEdit(injectGroup);
    m_injectAddressEdit->setMaxLength(5);
    m_injectAddressEdit->setPlaceholderText("e.g., 06ACE");
    m_injectAddressEdit->setFixedWidth(100);
    
    // Only allow hex characters
    QRegularExpression hexRegex("[0-9A-Fa-f]{0,5}");
    // Note: In a real implementation, you'd want a QRegularExpressionValidator here
    
    addrLayout->addWidget(prefixLabel);
    addrLayout->addWidget(m_injectAddressEdit);
    addrLayout->addStretch();
    injectLayout->addLayout(addrLayout);
    
    m_defaultAddressLabel = new QLabel("", injectGroup);
    m_defaultAddressLabel->setStyleSheet("color: #888888;");
    injectLayout->addWidget(m_defaultAddressLabel);
    
    layout->addWidget(injectGroup);
    
    // Summary
    QGroupBox* summaryGroup = new QGroupBox("Summary", page);
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroup);
    QLabel* summaryLabel = new QLabel("The combined ROM will be saved with a timestamp in the build output folder.", summaryGroup);
    summaryLayout->addWidget(summaryLabel);
    layout->addWidget(summaryGroup);
    
    layout->addStretch();
    
    m_stackedWidget->addWidget(page);
}

void RomBuilderDialog::loadPrograms() {
    m_programs.clear();
    m_programList->clear();
    
    if (m_workspacePath.isEmpty()) return;
    
    // Search for all .tp files in the workspace (in build folders)
    QDirIterator it(m_workspacePath, QStringList() << "*.tp", 
                    QDir::Files, QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        
        // Determine project name from path
        QString relativePath = filePath.mid(m_workspacePath.length() + 1);
        QStringList pathParts = relativePath.split('/');
        QString projectName = pathParts.isEmpty() ? fileInfo.baseName() : pathParts.first();
        
        TangentProgram prog;
        prog.name = projectName + "/" + fileInfo.fileName();
        prog.filePath = filePath;
        prog.selected = false;
        m_programs.append(prog);
        
        QListWidgetItem* item = new QListWidgetItem(prog.name, m_programList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, filePath);
    }
    
    if (m_programs.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("No .tp files found. Build your projects first.", m_programList);
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
}

void RomBuilderDialog::loadRomVersions() {
    m_romVersions.clear();
    m_romList->clear();
    
    // Get ROM directory path (relative to executable)
    QString appDir = QCoreApplication::applicationDirPath();
    QString romsPath = appDir + "/roms";
    
    // Also check src/resources/roms for development
    QStringList romPaths;
    romPaths << romsPath;
    romPaths << appDir + "/../src/resources/roms";
    romPaths << appDir + "/../../src/resources/roms";
    
    QMap<QString, TangentRomVersion> versionMap;
    
    for (const QString& romPath : romPaths) {
        QDir romDir(romPath);
        if (!romDir.exists()) continue;
        
        QStringList romFiles = romDir.entryList(QStringList() << "Tangent-V*.bin", QDir::Files);
        
        for (const QString& romFile : romFiles) {
            QString version = extractVersionFromFilename(romFile);
            QString startMem = extractStartMemFromFilename(romFile);
            
            if (version.isEmpty()) continue;
            
            // Only add if we haven't seen this version yet
            if (!versionMap.contains(version)) {
                TangentRomVersion rom;
                rom.name = "Tangent " + version;
                rom.filePath = romDir.absoluteFilePath(romFile);
                rom.startMem = startMem;
                rom.isCustom = false;
                versionMap[version] = rom;
            }
        }
    }
    
    // Sort versions and add to list
    QStringList sortedVersions = versionMap.keys();
    sortedVersions.sort();
    
    for (const QString& ver : sortedVersions) {
        m_romVersions.append(versionMap[ver]);
        QString displayText = versionMap[ver].name;
        if (!versionMap[ver].startMem.isEmpty()) {
            displayText += " (Start: 0x" + versionMap[ver].startMem + ")";
        }
        QListWidgetItem* item = new QListWidgetItem(displayText, m_romList);
        item->setData(Qt::UserRole, m_romVersions.size() - 1);
    }
    
    if (m_romVersions.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("No ROM versions found. Add ROMs to the roms folder.", m_romList);
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
}

QString RomBuilderDialog::extractVersionFromFilename(const QString& filename) const {
    // Pattern: Tangent-VX.X-XXXXX.bin
    QRegularExpression versionRegex("Tangent-(V[0-9]+\\.[0-9]+)-");
    QRegularExpressionMatch match = versionRegex.match(filename);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return QString();
}

QString RomBuilderDialog::extractStartMemFromFilename(const QString& filename) const {
    // Pattern: Tangent-VX.X-XXXXX.bin where XXXXX is 5 hex digits
    QRegularExpression startMemRegex("-([0-9A-Fa-f]{5})\\.bin$");
    QRegularExpressionMatch match = startMemRegex.match(filename);
    if (match.hasMatch()) {
        return match.captured(1).toUpper();
    }
    return QString();
}

void RomBuilderDialog::onNextFromPrograms() {
    // Check that at least one program is selected
    QStringList selectedPrograms = getSelectedPrograms();
    if (selectedPrograms.isEmpty()) {
        QMessageBox::warning(this, "No Programs Selected", 
            "Please select at least one Tangent Program (.tp) file to include in the ROM.");
        return;
    }
    
    m_stackedWidget->setCurrentIndex(1);
    updateNavigationButtons();
}

void RomBuilderDialog::onNextFromRomSelect() {
    // Check that a ROM is selected
    if (m_selectedRomIndex < 0 && m_customRomPath.isEmpty()) {
        QMessageBox::warning(this, "No ROM Selected", 
            "Please select a Tangent ROM version or browse for a custom ROM file.");
        return;
    }
    
    // Set default inject address based on selected ROM
    if (m_selectedRomIndex >= 0 && m_selectedRomIndex < m_romVersions.size()) {
        QString startMem = m_romVersions[m_selectedRomIndex].startMem;
        if (!startMem.isEmpty()) {
            m_injectAddressEdit->setText(startMem);
            m_defaultAddressLabel->setText("Default from ROM: 0x" + startMem);
        }
    } else if (!m_customRomPath.isEmpty()) {
        // Try to extract start mem from custom ROM filename
        QString startMem = extractStartMemFromFilename(QFileInfo(m_customRomPath).fileName());
        if (!startMem.isEmpty()) {
            m_injectAddressEdit->setText(startMem);
            m_defaultAddressLabel->setText("Detected from filename: 0x" + startMem);
        } else {
            m_defaultAddressLabel->setText("Enter the inject address manually");
        }
    }
    
    m_stackedWidget->setCurrentIndex(2);
    updateNavigationButtons();
}

void RomBuilderDialog::onBack() {
    int currentPage = m_stackedWidget->currentIndex();
    if (currentPage > 0) {
        m_stackedWidget->setCurrentIndex(currentPage - 1);
        updateNavigationButtons();
    }
}

void RomBuilderDialog::onBrowseCustomRom() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Custom ROM", 
        QString(), "Binary Files (*.bin);;All Files (*)");
    
    if (filePath.isEmpty()) return;
    
    m_customRomPath = filePath;
    m_romList->clearSelection();
    m_selectedRomIndex = -1;
    
    // Add custom ROM to the list temporarily
    QString fileName = QFileInfo(filePath).fileName();
    
    // Check if custom item already exists
    for (int i = 0; i < m_romList->count(); ++i) {
        QListWidgetItem* item = m_romList->item(i);
        if (item->text().startsWith("Custom: ")) {
            item->setText("Custom: " + fileName);
            item->setSelected(true);
            return;
        }
    }
    
    // Add new custom item
    QListWidgetItem* customItem = new QListWidgetItem("Custom: " + fileName, m_romList);
    customItem->setData(Qt::UserRole, -1); // Mark as custom
    customItem->setSelected(true);
}

void RomBuilderDialog::onRomSelectionChanged() {
    QList<QListWidgetItem*> selected = m_romList->selectedItems();
    if (selected.isEmpty()) {
        m_selectedRomIndex = -1;
        return;
    }
    
    int index = selected.first()->data(Qt::UserRole).toInt();
    if (index >= 0) {
        m_selectedRomIndex = index;
        m_customRomPath.clear();
    }
}

void RomBuilderDialog::onCreate() {
    // Validate inject address
    QString injectAddr = m_injectAddressEdit->text().trimmed().toUpper();
    if (injectAddr.length() != 5) {
        QMessageBox::warning(this, "Invalid Address", 
            "Inject address must be exactly 5 hex digits.");
        return;
    }
    
    // Validate hex characters
    QRegularExpression hexCheck("^[0-9A-F]{5}$");
    if (!hexCheck.match(injectAddr).hasMatch()) {
        QMessageBox::warning(this, "Invalid Address", 
            "Inject address must contain only hex digits (0-9, A-F).");
        return;
    }
    
    accept();
}

void RomBuilderDialog::onCancel() {
    reject();
}

void RomBuilderDialog::updateNavigationButtons() {
    int currentPage = m_stackedWidget->currentIndex();
    
    m_backButton->setEnabled(currentPage > 0);
    
    if (currentPage == 2) {
        m_nextButton->setText("Create");
    } else {
        m_nextButton->setText("Next");
    }
}

QStringList RomBuilderDialog::getSelectedPrograms() const {
    QStringList selected;
    for (int i = 0; i < m_programList->count(); ++i) {
        QListWidgetItem* item = m_programList->item(i);
        if (item->checkState() == Qt::Checked) {
            selected << item->data(Qt::UserRole).toString();
        }
    }
    return selected;
}

QString RomBuilderDialog::getSelectedRomPath() const {
    if (!m_customRomPath.isEmpty()) {
        return m_customRomPath;
    }
    
    if (m_selectedRomIndex >= 0 && m_selectedRomIndex < m_romVersions.size()) {
        return m_romVersions[m_selectedRomIndex].filePath;
    }
    
    return QString();
}

QString RomBuilderDialog::getInjectAddress() const {
    return m_injectAddressEdit->text().trimmed().toUpper();
}

QString RomBuilderDialog::getOutputName() const {
    // Generate output name based on ROM version and timestamp
    QString baseName = "Tangent";
    
    if (m_selectedRomIndex >= 0 && m_selectedRomIndex < m_romVersions.size()) {
        baseName = QString(m_romVersions[m_selectedRomIndex].name).replace(" ", "-");
    } else if (!m_customRomPath.isEmpty()) {
        baseName = "Tangent-Custom";
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
    return baseName + "-" + timestamp;
}

QString RomBuilderDialog::getCombinerCommand() const {
    QStringList args;
    
    // Add selected programs
    QStringList programs = getSelectedPrograms();
    args << programs;
    
    // Add output flags
    args << "-bh";
    args << "-o" << getOutputName();
    args << "-i" << getInjectAddress();
    
    // Note: Password and clockspeed will be added by MainWindow from settings
    
    return args.join(" ");
}
