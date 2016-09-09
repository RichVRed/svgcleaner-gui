/****************************************************************************
**
** SVG Cleaner could help you to clean up your SVG files
** from unnecessary data.
** Copyright (C) 2012-2016 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#pragma once

#include <QSettings>

namespace CleanerKey {
    namespace Elements {
        extern const QString RemoveComments;
        extern const QString RemoveDeclaration;
        extern const QString RemoveNonSvgElements;
        extern const QString RemoveUnusedDefs;
        extern const QString ConvertBasicShapes;
        extern const QString RemoveTitle;
        extern const QString RemoveDesc;
        extern const QString RemoveMetadata;
        extern const QString RemoveDuplLinearGradient;
        extern const QString RemoveDuplRadialGradient;
    }

    namespace Attributes {
        extern const QString RemoveNonSvgAttributes;
        extern const QString RemoveUnrefIds;
        extern const QString TrimIds;
        extern const QString RemoveTextAttributes;
        extern const QString RemoveDefaultAttributes;
        extern const QString RemoveXmlnsXlinkAttribute;
    }

    namespace Paths {
        extern const QString TrimPaths;
        extern const QString RemoveDuplCmdInPaths;
        extern const QString JoinArcToFlags;
    }

    namespace Output {
        extern const QString PrecisionCoordinate;
        extern const QString PrecisionTransform;
        extern const QString TrimColors;
        extern const QString SimplifyTransforms;
        extern const QString Indent;
    }
}

class CleanerOptions : public QSettings
{
public:
    explicit CleanerOptions(QObject *parent = 0);

    bool flag(const QString &key);
    int integer(const QString &key);
    QString string(const QString &key);

    static QVariant defaultValue(const QString &key);
    static bool defaultFlag(const QString &key);
    static int defaultInt(const QString &key);

    static QStringList genArgs();
};