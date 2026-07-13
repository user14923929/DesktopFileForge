#pragma once

#include "DesktopEntry.h"
#include <QStringList>

// Validates a DesktopEntry against the parts of the Desktop Entry
// Specification that matter most for a working, well-behaved entry.
class DesktopEntryValidator {
public:
    struct Result {
        bool isValid = true;
        QStringList errors;   // must fix
        QStringList warnings; // should fix, not blocking
    };

    static Result validate(const DesktopEntry &entry);
};
