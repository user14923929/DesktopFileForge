#include "DesktopEntry.h"

QString DesktopEntry::typeToString(Type t) {
    switch (t) {
        case Type::Application: return "Application";
        case Type::Link:        return "Link";
        case Type::Directory:   return "Directory";
    }
    return "Application";
}

DesktopEntry::Type DesktopEntry::typeFromString(const QString &s) {
    if (s == "Link") return Type::Link;
    if (s == "Directory") return Type::Directory;
    return Type::Application;
}

QStringList DesktopEntry::knownCategories() {
    // Main categories from the Desktop Menu Specification (subset commonly used).
    return {
        "AudioVideo", "Audio", "Video", "Development", "Education",
        "Game", "Graphics", "Network", "Office", "Science",
        "Settings", "System", "Utility", "Qt", "KDE", "GNOME"
    };
}
