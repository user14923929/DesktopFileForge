#include "DesktopEntryValidator.h"

#include <QFileInfo>
#include <QStandardPaths>
#include <QIcon>
#include <QRegularExpression>

DesktopEntryValidator::Result DesktopEntryValidator::validate(const DesktopEntry &entry) {
    Result result;

    // --- Required fields ---
    if (entry.name.trimmed().isEmpty()) {
        result.errors << "Name is required.";
    }

    if (entry.type == DesktopEntry::Type::Application) {
        if (entry.exec.trimmed().isEmpty()) {
            result.errors << "Exec is required for Type=Application.";
        } else {
            // Extract the binary (first token, ignoring field codes like %f, %U).
            QString binary = entry.exec.section(' ', 0, 0);
            bool foundInPath = !QStandardPaths::findExecutable(binary).isEmpty();
            bool isAbsoluteAndExists = QFileInfo(binary).isAbsolute() && QFileInfo(binary).exists();
            if (!foundInPath && !isAbsoluteAndExists) {
                result.warnings << QString("Executable '%1' was not found in PATH or as an absolute path.").arg(binary);
            }
        }
    } else if (entry.type == DesktopEntry::Type::Link) {
        if (entry.url.trimmed().isEmpty()) {
            result.errors << "URL is required for Type=Link.";
        }
    }

    // --- Icon ---
    if (entry.icon.isEmpty()) {
        result.warnings << "No icon set; the entry will use a generic placeholder icon.";
    } else if (QFileInfo(entry.icon).isAbsolute()) {
        if (!QFileInfo(entry.icon).exists()) {
            result.warnings << QString("Icon file '%1' does not exist.").arg(entry.icon);
        }
    } else if (!QIcon::hasThemeIcon(entry.icon)) {
        result.warnings << QString("Icon name '%1' was not found in the current icon theme.").arg(entry.icon);
    }

    // --- Categories ---
    if (entry.categories.isEmpty()) {
        result.warnings << "No categories set; the entry may not appear in application menus.";
    } else {
        const QStringList known = DesktopEntry::knownCategories();
        for (const QString &cat : entry.categories) {
            if (!known.contains(cat)) {
                result.warnings << QString("Category '%1' is not a well-known freedesktop.org category.").arg(cat);
            }
        }
    }

    // --- Naming convention for Categories/MimeType (must not contain spaces) ---
    static const QRegularExpression hasSpace("\\s");
    for (const QString &cat : entry.categories) {
        if (hasSpace.match(cat).hasMatch()) {
            result.errors << QString("Category '%1' must not contain spaces.").arg(cat);
        }
    }

    result.isValid = result.errors.isEmpty();
    return result;
}
