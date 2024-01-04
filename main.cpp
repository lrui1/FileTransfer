#include "filetransfer.h"
#include "InitSock.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

CInitSock c;  /* 初始化winsock */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "FileTransfer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    FileTransfer w;
    w.show();
    return a.exec();
}
