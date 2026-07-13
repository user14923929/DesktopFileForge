#pragma once

#include <QMainWindow>
#include "core/DesktopEntry.h"

class QLineEdit;
class QComboBox;
class QCheckBox;
class QPlainTextEdit;
class QListWidget;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onPickIcon();
    void onPickExecutable();
    void onValidate();
    void onSave();
    void onOpenExisting();
    void updatePreview();

private:
    DesktopEntry collectEntry() const;
    void applyEntry(const DesktopEntry &entry);
    void buildUi();

    QComboBox *m_typeCombo;
    QLineEdit *m_nameEdit;
    QLineEdit *m_genericNameEdit;
    QLineEdit *m_commentEdit;
    QLineEdit *m_iconEdit;
    QLineEdit *m_execEdit;
    QLineEdit *m_pathEdit;
    QLineEdit *m_urlEdit;
    QCheckBox *m_terminalCheck;
    QCheckBox *m_startupNotifyCheck;
    QCheckBox *m_noDisplayCheck;
    QListWidget *m_categoriesList;
    QLineEdit *m_keywordsEdit;
    QLineEdit *m_mimeTypesEdit;

    QLabel *m_previewIcon;
    QLabel *m_previewName;
    QPlainTextEdit *m_previewSource;

    QPushButton *m_saveUserButton;
    QPushButton *m_saveSystemButton;
};
