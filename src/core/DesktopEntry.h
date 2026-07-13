#pragma once

#include <QString>
#include <QStringList>

// Represents the [Desktop Entry] section of a .desktop file,
// as defined by the freedesktop.org Desktop Entry Specification.
class DesktopEntry {
public:
    enum class Type {
        Application,
        Link,
        Directory
    };

    DesktopEntry() = default;

    Type type = Type::Application;
    QString version = "1.0";
    QString name;
    QString genericName;
    QString comment;
    QString icon;
    QString exec;
    QString tryExec;
    QString path;       // working directory for Exec
    QString url;        // only for Type=Link
    bool terminal = false;
    bool noDisplay = false;
    bool startupNotify = true;
    QStringList categories;
    QStringList mimeTypes;
    QStringList keywords;

    static QString typeToString(Type t);
    static Type typeFromString(const QString &s);

    // Well-known category list from the Desktop Menu Specification.
    static QStringList knownCategories();
};
