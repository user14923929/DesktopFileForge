#include "DesktopEntryWriter.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>

QString DesktopEntryWriter::escapeValue(const QString &value) {
    QString out = value;
    out.replace("\\", "\\\\");
    out.replace("\n", "\\n");
    out.replace("\t", "\\t");
    out.replace("\r", "\\r");
    return out;
}

QString DesktopEntryWriter::toString(const DesktopEntry &entry) {
    QString out;
    QTextStream stream(&out);

    stream << "[Desktop Entry]\n";
    stream << "Type=" << DesktopEntry::typeToString(entry.type) << "\n";
    stream << "Version=" << entry.version << "\n";
    stream << "Name=" << escapeValue(entry.name) << "\n";

    if (!entry.genericName.isEmpty())
        stream << "GenericName=" << escapeValue(entry.genericName) << "\n";

    if (!entry.comment.isEmpty())
        stream << "Comment=" << escapeValue(entry.comment) << "\n";

    if (!entry.icon.isEmpty())
        stream << "Icon=" << escapeValue(entry.icon) << "\n";

    if (entry.type == DesktopEntry::Type::Application) {
        if (!entry.exec.isEmpty())
            stream << "Exec=" << escapeValue(entry.exec) << "\n";
        if (!entry.tryExec.isEmpty())
            stream << "TryExec=" << escapeValue(entry.tryExec) << "\n";
        if (!entry.path.isEmpty())
            stream << "Path=" << escapeValue(entry.path) << "\n";
        stream << "Terminal=" << (entry.terminal ? "true" : "false") << "\n";
        stream << "StartupNotify=" << (entry.startupNotify ? "true" : "false") << "\n";

        if (!entry.mimeTypes.isEmpty())
            stream << "MimeType=" << entry.mimeTypes.join(';') << ";\n";
    } else if (entry.type == DesktopEntry::Type::Link) {
        if (!entry.url.isEmpty())
            stream << "URL=" << escapeValue(entry.url) << "\n";
    }

    if (!entry.categories.isEmpty())
        stream << "Categories=" << entry.categories.join(';') << ";\n";

    if (!entry.keywords.isEmpty())
        stream << "Keywords=" << entry.keywords.join(';') << ";\n";

    stream << "NoDisplay=" << (entry.noDisplay ? "true" : "false") << "\n";

    return out;
}

bool DesktopEntryWriter::writeToFile(const DesktopEntry &entry, const QString &filePath, QString *errorMessage) {
    QFileInfo info(filePath);
    QDir dir = info.dir();
    if (!dir.exists() && !dir.mkpath(".")) {
        if (errorMessage) *errorMessage = "Could not create directory: " + dir.path();
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = "Could not open file for writing: " + filePath;
        return false;
    }

    QTextStream out(&file);
    out << toString(entry);
    file.close();

    // .desktop files should be executable for some launchers/file managers.
    file.setPermissions(file.permissions() | QFileDevice::ExeOwner | QFileDevice::ExeGroup | QFileDevice::ExeOther);

    return true;
}
