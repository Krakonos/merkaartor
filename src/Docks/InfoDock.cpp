//
// C++ Implementation: InfoDock
//
// Description:
//
//
// Author: cbro <cbro@semperpax.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "InfoDock.h"
#include "MainWindow.h"
#include "MerkaartorPreferences.h"
#include "DownloadOSM.h"

#include <QApplication>
#include <QDesktopServices>

InfoDock::InfoDock(MainWindow* aParent)
    : MDockAncestor(aParent), Main(aParent), theText(new QTextBrowser(this))
{
    setMinimumSize(220,100);
    setWindowTitle(tr("Info"));
    setObjectName("infoDock");

    theText->setReadOnly(true);
    theText->setOpenLinks(false);
    setWidget(theText);

    connect(theText, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(on_anchorClicked(const QUrl &)));
    retranslateUi();
}


InfoDock::~InfoDock()
{
}

void InfoDock::setHtml(QString html)
{
    currentHtml = html;
    theText->setHtml(html);
}

void InfoDock::setHoverHtml(QString html)
{
    theText->setHtml(html);
}

void InfoDock::unsetHoverHtml()
{
    theText->setHtml(currentHtml);
}

QString InfoDock::getHtml()
{
    return theText->toHtml();
}

void InfoDock::on_anchorClicked(const QUrl & link)
{
//    QString data;

//    QString osmWebsite = M_PREFS->getOsmApiUrl();
//    QString osmUser = M_PREFS->getOsmUser();
//    QString osmPwd = M_PREFS->getOsmPassword();

//    Downloader theDownloader(osmUser, osmPwd);
//    QUrl theUrl(osmWebsite+link.path());

//    if (theDownloader.request("GET", theUrl, data)) {
//        QTextBrowser* b = new QTextBrowser;
//        QString s = QString::fromUtf8(theDownloader.content().constData());
//        b->setPlainText(s);
//        b->setAttribute(Qt::WA_DeleteOnClose,true);
//        b->resize(640, 480);
//        b->show();
//        b->raise();
//    } else {
//        QMessageBox::warning(Main,QApplication::translate("Downloader","Download failed"),QApplication::translate("Downloader","Unexpected http status code (%1)").arg(theDownloader.resultCode()));
//    }
    QUrl theUrl(M_PREFS->getOsmServer()->getServerInfo().Url+link.path());
    QDesktopServices::openUrl(theUrl);
}

void InfoDock::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi();
    MDockAncestor::changeEvent(event);
}

void InfoDock::retranslateUi()
{
    setWindowTitle(tr("Info"));
    setHtml("");
}

