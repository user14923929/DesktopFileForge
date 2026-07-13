#pragma once

#include "DesktopEntry.h"
#include <QString>

// Serializes a DesktopEntry into the INI-like .desktop file format.
class DesktopEntryWriter {
public:
    // Returns the full file contents as a string.
    static QString toString(const DesktopEntry &entry);

    // Writes the entry to disk at the given path.
    // Returns true on success; errorMessage is set on failure.
    static bool writeToFile(const DesktopEntry &entry, const QString &filePath, QString *errorMessage = nullptr);

private:
    // Escapes special characters per the Desktop Entry Specification
    // (backslash, newline, tab, carriage return).
    static QString escapeValue(const QString &value);
};
