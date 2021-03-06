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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryDir>
#include <QDebug>

// NOTE: this code is mostly horrible, but it does the job.

// We serialize html files generated by svgcleaner's docgen and then compress them.

struct CliData {
    QString docgen;
    QString docdir;
    QString outdoc;
    QString stylesheet;
};

bool preStartChecks();
CliData parseArgs();
bool isExeExist(const QString &name, const QStringList &args);
QByteArray readFile(const QString &path);
bool runProcess(const QString &path, const QStringList &args);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("docgen");

    if (!preStartChecks()) {
        return 1;
    }

    const auto data = parseArgs();

    QTemporaryDir dir;
    if (!dir.isValid()) {
        qDebug() << "Failed to create temp dir.";
        return 1;
    }

    const QString workDir = dir.path();

    {
        QStringList args;
        args << "--docdir=" + data.docdir << "--for-gui" << "--outdir=" + workDir;

        qDebug() << "Run svgcleaner's docgen";
        if (!runProcess(data.docgen, args)) {
            return 1;
        }
    }

    for (const QFileInfo &fi : QDir(workDir).entryInfoList({ "*.rst" })) {
        const QString outPath = workDir + "/" + fi.completeBaseName() + ".html";
        qDebug() << "Gen HTML from" << fi.fileName();
        if (!runProcess("rst2html.py", { "--stylesheet=" + data.stylesheet,
                                         fi.absoluteFilePath(), outPath })) {
            return 1;
        }
    }

    qDebug() << "Gen doc file";

    QByteArray outputData;
    QDataStream stream(&outputData, QFile::WriteOnly);
    for (const QFileInfo &fi : QDir(workDir).entryInfoList({ "*.html" })) {
        QByteArray ba = readFile(fi.absoluteFilePath());
        ba.replace("border=\"1\"", "border=\"0\"");
        stream << fi.completeBaseName();
        stream << ba;
    }


    QFile file(data.outdoc);
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "could not create output file";
        return 1;
    }

    outputData = qCompress(outputData);

    if (file.write(outputData) != outputData.size()) {
        qDebug() << "failed to write output data";
        return 1;
    }

    return 0;
}

bool preStartChecks()
{
    if (!isExeExist("rst2html.py", { "-h" })) {
        qDebug() << "rst2html.py not found";
        return false;
    }
    return true;
}

bool isExeExist(const QString &name, const QStringList &args)
{
    QProcess proc;
    proc.start(name, args);
    if (!proc.waitForFinished(1000)) {
        return false;
    }

    return proc.exitCode() == 0 && proc.exitStatus() == QProcess::NormalExit;
}

CliData parseArgs()
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Documetation generator.");

    QCommandLineOption mainDocGenPath(QStringList("main-docgen"),
        "Sets path to svgcleaner's docgen <path>.", "path");
    parser.addOption(mainDocGenPath);

    QCommandLineOption docDir(QStringList("doc-dir"),
        "Sets path to svgcleaner documentation <dir>.", "dir");
    parser.addOption(docDir);

    QCommandLineOption outDocPath(QStringList("out-doc"),
        "Sets path to generated documentation file <path>.", "path");
    parser.addOption(outDocPath);

    QCommandLineOption styleSheet(QStringList("stylesheet"),
        "Sets path to CSS style <path>.", "path");
    parser.addOption(styleSheet);

    if (!parser.parse(QCoreApplication::arguments())) {
        qDebug() << parser.errorText();
        parser.showHelp(1);
    }

    CliData data;

    const auto getValue = [&parser](const QCommandLineOption &opt){
        QString value;
        if (parser.isSet(opt)) {
            value = parser.value(opt);
        }

        if (value.isEmpty()) {
            qDebug() << QString("Error: '%1' is not set.").arg(opt.names().first());
            parser.showHelp(1);
        }

        return value;
    };

    data.docgen = getValue(mainDocGenPath);
    data.docdir = getValue(docDir);
    data.outdoc = getValue(outDocPath);
    data.stylesheet = getValue(styleSheet);

    return data;
}

QByteArray readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "failed to open file:" << path;
        exit(1);
    }

    return file.readAll();
}

bool runProcess(const QString &path, const QStringList &args)
{
    QProcess proc;
    proc.start(path, args);
    if (!proc.waitForFinished()) {
        qDebug() << "failed to start:" << path;
        return false;
    }

    if (proc.exitCode() != 0) {
        qDebug() << QString("Process '%1 %2' finished with exit code %3.")
                        .arg(path).arg(args.join(' ')).arg(proc.exitCode());
        qDebug().noquote() << proc.readAllStandardError();
        return false;
    }

    return true;
}
