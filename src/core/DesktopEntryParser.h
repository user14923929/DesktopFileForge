#pragma once

#include "DesktopEntry.h"
#include <QString>

// Parses an existing .desktop file into a DesktopEntry, for editing.
class DesktopEntryParser {
public:
    // Returns true on success. On failure, errorMessage is set and
    // outEntry is left unmodified.
    static bool parseFile(const QString &filePath, DesktopEntry *outEntry, QString *errorMessage = nullptr);

private:
    static QString unescapeValue(const QString &value);
};
