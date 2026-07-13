#include "MainWindow.h"
#include "core/DesktopEntryWriter.h"
#include "core/DesktopEntryParser.h"
#include "core/DesktopEntryValidator.h"

#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QIcon>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    buildUi();
    setWindowTitle("DesktopFileForge");
    resize(880, 620);
    updatePreview();
}

void MainWindow::buildUi() {
    auto *central = new QWidget(this);
    auto *outerLayout = new QHBoxLayout(central);

    auto *splitter = new QSplitter(Qt::Horizontal, central);

    // ---- Left: form ----
    auto *formWidget = new QWidget(splitter);
    auto *formLayout = new QFormLayout(formWidget);

    m_typeCombo = new QComboBox(formWidget);
    m_typeCombo->addItems({"Application", "Link", "Directory"});
    formLayout->addRow("Type", m_typeCombo);

    m_nameEdit = new QLineEdit(formWidget);
    m_nameEdit->setPlaceholderText("My Cool App");
    formLayout->addRow("Name*", m_nameEdit);

    m_genericNameEdit = new QLineEdit(formWidget);
    m_genericNameEdit->setPlaceholderText("Text Editor");
    formLayout->addRow("Generic name", m_genericNameEdit);

    m_commentEdit = new QLineEdit(formWidget);
    m_commentEdit->setPlaceholderText("A short description shown as a tooltip");
    formLayout->addRow("Comment", m_commentEdit);

    auto *iconRow = new QWidget(formWidget);
    auto *iconRowLayout = new QHBoxLayout(iconRow);
    iconRowLayout->setContentsMargins(0, 0, 0, 0);
    m_iconEdit = new QLineEdit(iconRow);
    m_iconEdit->setPlaceholderText("icon-name or /path/to/icon.png");
    auto *iconPickButton = new QPushButton("Browse...", iconRow);
    iconRowLayout->addWidget(m_iconEdit);
    iconRowLayout->addWidget(iconPickButton);
    formLayout->addRow("Icon", iconRow);

    auto *execRow = new QWidget(formWidget);
    auto *execRowLayout = new QHBoxLayout(execRow);
    execRowLayout->setContentsMargins(0, 0, 0, 0);
    m_execEdit = new QLineEdit(execRow);
    m_execEdit->setPlaceholderText("/usr/bin/myapp %f");
    auto *execPickButton = new QPushButton("Browse...", execRow);
    execRowLayout->addWidget(m_execEdit);
    execRowLayout->addWidget(execPickButton);
    formLayout->addRow("Exec", execRow);

    m_pathEdit = new QLineEdit(formWidget);
    m_pathEdit->setPlaceholderText("Working directory (optional)");
    formLayout->addRow("Path", m_pathEdit);

    m_urlEdit = new QLineEdit(formWidget);
    m_urlEdit->setPlaceholderText("https://example.com (Type=Link only)");
    formLayout->addRow("URL", m_urlEdit);

    m_terminalCheck = new QCheckBox("Run in terminal", formWidget);
    formLayout->addRow("", m_terminalCheck);

    m_startupNotifyCheck = new QCheckBox("Startup notification", formWidget);
    m_startupNotifyCheck->setChecked(true);
    formLayout->addRow("", m_startupNotifyCheck);

    m_noDisplayCheck = new QCheckBox("Hide from menus (NoDisplay)", formWidget);
    formLayout->addRow("", m_noDisplayCheck);

    m_categoriesList = new QListWidget(formWidget);
    m_categoriesList->setSelectionMode(QAbstractItemView::NoSelection);
    m_categoriesList->setMaximumHeight(140);
    for (const QString &cat : DesktopEntry::knownCategories()) {
        auto *item = new QListWidgetItem(cat, m_categoriesList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
    formLayout->addRow("Categories", m_categoriesList);

    m_keywordsEdit = new QLineEdit(formWidget);
    m_keywordsEdit->setPlaceholderText("comma, separated, keywords");
    formLayout->addRow("Keywords", m_keywordsEdit);

    m_mimeTypesEdit = new QLineEdit(formWidget);
    m_mimeTypesEdit->setPlaceholderText("text/plain, application/json");
    formLayout->addRow("MIME types", m_mimeTypesEdit);

    // ---- Right: preview + actions ----
    auto *rightWidget = new QWidget(splitter);
    auto *rightLayout = new QVBoxLayout(rightWidget);

    auto *previewGroup = new QGroupBox("Live preview", rightWidget);
    auto *previewLayout = new QVBoxLayout(previewGroup);
    auto *previewHeaderRow = new QHBoxLayout();
    m_previewIcon = new QLabel(previewGroup);
    m_previewIcon->setFixedSize(48, 48);
    m_previewIcon->setScaledContents(true);
    m_previewName = new QLabel(previewGroup);
    m_previewName->setStyleSheet("font-size: 16px; font-weight: bold;");
    previewHeaderRow->addWidget(m_previewIcon);
    previewHeaderRow->addWidget(m_previewName, 1);
    previewLayout->addLayout(previewHeaderRow);

    m_previewSource = new QPlainTextEdit(previewGroup);
    m_previewSource->setReadOnly(true);
    m_previewSource->setFont(QFont("monospace"));
    previewLayout->addWidget(m_previewSource, 1);

    rightLayout->addWidget(previewGroup, 1);

    auto *actionsGroup = new QGroupBox("Actions", rightWidget);
    auto *actionsLayout = new QVBoxLayout(actionsGroup);

    auto *validateButton = new QPushButton("Validate", actionsGroup);
    actionsLayout->addWidget(validateButton);

    auto *openExistingButton = new QPushButton("Open existing .desktop...", actionsGroup);
    actionsLayout->addWidget(openExistingButton);

    m_saveUserButton = new QPushButton("Save to ~/.local/share/applications", actionsGroup);
    actionsLayout->addWidget(m_saveUserButton);

    m_saveSystemButton = new QPushButton("Save as... (custom path)", actionsGroup);
    actionsLayout->addWidget(m_saveSystemButton);

    rightLayout->addWidget(actionsGroup);

    splitter->addWidget(formWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    outerLayout->addWidget(splitter);
    setCentralWidget(central);

    // ---- Signals ----
    connect(iconPickButton, &QPushButton::clicked, this, &MainWindow::onPickIcon);
    connect(execPickButton, &QPushButton::clicked, this, &MainWindow::onPickExecutable);
    connect(validateButton, &QPushButton::clicked, this, &MainWindow::onValidate);
    connect(openExistingButton, &QPushButton::clicked, this, &MainWindow::onOpenExisting);
    connect(m_saveUserButton, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(m_saveSystemButton, &QPushButton::clicked, this, &MainWindow::onSave);

    connect(m_nameEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_iconEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_typeCombo, &QComboBox::currentTextChanged, this, &MainWindow::updatePreview);
    connect(m_genericNameEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_commentEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_execEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_pathEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_urlEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_terminalCheck, &QCheckBox::toggled, this, &MainWindow::updatePreview);
    connect(m_startupNotifyCheck, &QCheckBox::toggled, this, &MainWindow::updatePreview);
    connect(m_noDisplayCheck, &QCheckBox::toggled, this, &MainWindow::updatePreview);
    connect(m_keywordsEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_mimeTypesEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_categoriesList, &QListWidget::itemChanged, this, &MainWindow::updatePreview);
}

DesktopEntry MainWindow::collectEntry() const {
    DesktopEntry entry;
    entry.type = DesktopEntry::typeFromString(m_typeCombo->currentText());
    entry.name = m_nameEdit->text().trimmed();
    entry.genericName = m_genericNameEdit->text().trimmed();
    entry.comment = m_commentEdit->text().trimmed();
    entry.icon = m_iconEdit->text().trimmed();
    entry.exec = m_execEdit->text().trimmed();
    entry.path = m_pathEdit->text().trimmed();
    entry.url = m_urlEdit->text().trimmed();
    entry.terminal = m_terminalCheck->isChecked();
    entry.startupNotify = m_startupNotifyCheck->isChecked();
    entry.noDisplay = m_noDisplayCheck->isChecked();

    QStringList cats;
    for (int i = 0; i < m_categoriesList->count(); ++i) {
        auto *item = m_categoriesList->item(i);
        if (item->checkState() == Qt::Checked)
            cats << item->text();
    }
    entry.categories = cats;

    if (!m_keywordsEdit->text().trimmed().isEmpty()) {
        for (QString kw : m_keywordsEdit->text().split(',', Qt::SkipEmptyParts))
            entry.keywords << kw.trimmed();
    }
    if (!m_mimeTypesEdit->text().trimmed().isEmpty()) {
        for (QString mt : m_mimeTypesEdit->text().split(',', Qt::SkipEmptyParts))
            entry.mimeTypes << mt.trimmed();
    }

    return entry;
}

void MainWindow::applyEntry(const DesktopEntry &entry) {
    m_typeCombo->setCurrentText(DesktopEntry::typeToString(entry.type));
    m_nameEdit->setText(entry.name);
    m_genericNameEdit->setText(entry.genericName);
    m_commentEdit->setText(entry.comment);
    m_iconEdit->setText(entry.icon);
    m_execEdit->setText(entry.exec);
    m_pathEdit->setText(entry.path);
    m_urlEdit->setText(entry.url);
    m_terminalCheck->setChecked(entry.terminal);
    m_startupNotifyCheck->setChecked(entry.startupNotify);
    m_noDisplayCheck->setChecked(entry.noDisplay);
    m_keywordsEdit->setText(entry.keywords.join(", "));
    m_mimeTypesEdit->setText(entry.mimeTypes.join(", "));

    for (int i = 0; i < m_categoriesList->count(); ++i) {
        auto *item = m_categoriesList->item(i);
        item->setCheckState(entry.categories.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
    }

    updatePreview();
}

void MainWindow::updatePreview() {
    DesktopEntry entry = collectEntry();

    m_previewName->setText(entry.name.isEmpty() ? "(no name yet)" : entry.name);

    QIcon icon;
    if (!entry.icon.isEmpty()) {
        if (QFileInfo(entry.icon).isAbsolute() && QFileInfo(entry.icon).exists())
            icon = QIcon(entry.icon);
        else
            icon = QIcon::fromTheme(entry.icon);
    }
    if (icon.isNull())
        icon = QIcon::fromTheme("application-x-executable");
    m_previewIcon->setPixmap(icon.pixmap(48, 48));

    m_previewSource->setPlainText(DesktopEntryWriter::toString(entry));
}

void MainWindow::onPickIcon() {
    QString file = QFileDialog::getOpenFileName(this, "Choose icon file", QDir::homePath(),
                                                 "Images (*.png *.svg *.xpm *.ico)");
    if (!file.isEmpty())
        m_iconEdit->setText(file);
}

void MainWindow::onPickExecutable() {
    QString file = QFileDialog::getOpenFileName(this, "Choose executable", "/usr/bin");
    if (!file.isEmpty())
        m_execEdit->setText(file);
}

void MainWindow::onValidate() {
    DesktopEntry entry = collectEntry();
    auto result = DesktopEntryValidator::validate(entry);

    QString message;
    if (!result.errors.isEmpty()) {
        message += "Errors (must fix):\n";
        for (const auto &e : result.errors) message += "  • " + e + "\n";
    }
    if (!result.warnings.isEmpty()) {
        if (!message.isEmpty()) message += "\n";
        message += "Warnings:\n";
        for (const auto &w : result.warnings) message += "  • " + w + "\n";
    }
    if (message.isEmpty())
        message = "Looks good! No issues found.";

    if (result.isValid)
        QMessageBox::information(this, "Validation", message);
    else
        QMessageBox::warning(this, "Validation", message);
}

void MainWindow::onOpenExisting() {
    QString file = QFileDialog::getOpenFileName(
        this, "Open .desktop file",
        QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation),
        "Desktop entries (*.desktop)");
    if (file.isEmpty())
        return;

    DesktopEntry entry;
    QString error;
    if (!DesktopEntryParser::parseFile(file, &entry, &error)) {
        QMessageBox::critical(this, "Open failed", error);
        return;
    }
    applyEntry(entry);
}

void MainWindow::onSave() {
    auto *sender = qobject_cast<QPushButton *>(this->sender());
    DesktopEntry entry = collectEntry();

    auto result = DesktopEntryValidator::validate(entry);
    if (!result.isValid) {
        QString msg = "Cannot save — fix these errors first:\n";
        for (const auto &e : result.errors) msg += "  • " + e + "\n";
        QMessageBox::critical(this, "Validation failed", msg);
        return;
    }

    QString targetPath;
    if (sender == m_saveUserButton) {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
        QString fileName = entry.name.isEmpty() ? "app" : entry.name;
        fileName.replace(QRegularExpression("[^A-Za-z0-9_.-]"), "-");
        targetPath = dir + "/" + fileName + ".desktop";
    } else {
        targetPath = QFileDialog::getSaveFileName(
            this, "Save .desktop file", QDir::homePath() + "/" + entry.name + ".desktop",
            "Desktop entries (*.desktop)");
        if (targetPath.isEmpty())
            return;
    }

    QString error;
    if (!DesktopEntryWriter::writeToFile(entry, targetPath, &error)) {
        QMessageBox::critical(this, "Save failed", error);
        return;
    }

    QString warningNote;
    if (!result.warnings.isEmpty()) {
        warningNote = "\n\nNote: some warnings were not blocking:\n";
        for (const auto &w : result.warnings) warningNote += "  • " + w + "\n";
    }

    QMessageBox::information(this, "Saved", "Saved to:\n" + targetPath + warningNote);
}
