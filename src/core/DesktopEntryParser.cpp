#include "DesktopEntryParser.h"

#include <QFile>
#include <QTextStream>

QString DesktopEntryParser::unescapeValue(const QString &value) {
    QString out;
    out.reserve(value.size());
    for (int i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size()) {
            QChar next = value[i + 1];
            if (next == 'n') { out += '\n'; ++i; continue; }
            if (next == 't') { out += '\t'; ++i; continue; }
            if (next == 'r') { out += '\r'; ++i; continue; }
            if (next == '\\') { out += '\\'; ++i; continue; }
        }
        out += value[i];
    }
    return out;
}

bool DesktopEntryParser::parseFile(const QString &filePath, DesktopEntry *outEntry, QString *errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = "Could not open file: " + filePath;
        return false;
    }

    DesktopEntry entry;
    bool inDesktopEntrySection = false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith('#'))
            continue;

        if (line.startsWith('[')) {
            inDesktopEntrySection = (line == "[Desktop Entry]");
            continue;
        }

        if (!inDesktopEntrySection)
            continue;

        int eq = line.indexOf('=');
        if (eq < 0)
            continue;

        QString key = line.left(eq).trimmed();
        QString value = unescapeValue(line.mid(eq + 1).trimmed());

        // Ignore localized keys like Name[ru] for this simple editor.
        if (key.contains('['))
            continue;

        if (key == "Type") entry.type = DesktopEntry::typeFromString(value);
        else if (key == "Version") entry.version = value;
        else if (key == "Name") entry.name = value;
        else if (key == "GenericName") entry.genericName = value;
        else if (key == "Comment") entry.comment = value;
        else if (key == "Icon") entry.icon = value;
        else if (key == "Exec") entry.exec = value;
        else if (key == "TryExec") entry.tryExec = value;
        else if (key == "Path") entry.path = value;
        else if (key == "URL") entry.url = value;
        else if (key == "Terminal") entry.terminal = (value == "true");
        else if (key == "StartupNotify") entry.startupNotify = (value == "true");
        else if (key == "NoDisplay") entry.noDisplay = (value == "true");
        else if (key == "Categories") entry.categories = value.split(';', Qt::SkipEmptyParts);
        else if (key == "MimeType") entry.mimeTypes = value.split(';', Qt::SkipEmptyParts);
        else if (key == "Keywords") entry.keywords = value.split(';', Qt::SkipEmptyParts);
    }

    file.close();

    if (entry.name.isEmpty()) {
        if (errorMessage) *errorMessage = "File has no [Desktop Entry] section or Name= key.";
        return false;
    }

    *outEntry = entry;
    return true;
}
