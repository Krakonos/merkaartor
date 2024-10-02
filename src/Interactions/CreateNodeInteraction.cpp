#include "Global.h"
#include "CreateNodeInteraction.h"

#include "MainWindow.h"
#include "PropertiesDock.h"
#include "DocumentCommands.h"
#include "WayCommands.h"
#include "Projection.h"
#include "Node.h"
#include "LineF.h"
#include "MoveNodeInteraction.h"
#include "Global.h"

#include <QList>

CreateNodeInteraction::CreateNodeInteraction(MainWindow* aMain)
    : FeatureSnapInteraction(aMain)
    , theMoveInteraction(0)
{
}

CreateNodeInteraction::~CreateNodeInteraction(void)
{
    delete theMoveInteraction;
}

QString CreateNodeInteraction::toHtml()
{
    QString help;
    //help = (MainWindow::tr("LEFT-CLICK to select; LEFT-DRAG to move"));

    QString desc;
    desc = QString("<big><b>%1</b></big><br/>").arg(MainWindow::tr("Create Node interaction"));
    desc += QString("<b>%1</b><br/>").arg(help);

    QString S =
    "<html><head/><body>"
    "<small><i>" + QString(metaObject()->className()) + "</i></small><br/>"
    + desc;
    S += "</body></html>";

    return S;
}

void CreateNodeInteraction::snapMousePressEvent(QMouseEvent * ev, Feature* aFeat)
{
    if (CAST_NODE(aFeat)) {
        clearNoSnap();
        addToNoSnap(aFeat);
        return theMoveInteraction->snapMousePressEvent(ev, aFeat);
    } else {
        SAFE_DELETE(theMoveInteraction);
#ifndef _MOBILE
        theMain->view()->setCursor(cursor());
#endif
    }

}

void CreateNodeInteraction::snapMouseMoveEvent(QMouseEvent* ev, Feature* aFeat)
{
    if (CAST_NODE(aFeat)) {
        if (!theMoveInteraction) {
            theMoveInteraction = new MoveNodeInteraction(theMain);
        }
#ifndef _MOBILE
        theMain->view()->setCursor(theMoveInteraction->cursor());
    } else
        theMain->view()->setCursor(cursor());
#else
    }
#endif

    if (theMoveInteraction)
        return theMoveInteraction->snapMouseMoveEvent(ev, aFeat);
}

void CreateNodeInteraction::snapMouseReleaseEvent(QMouseEvent * ev, Feature* aFeat)
{
    if (theMoveInteraction) {
        theMoveInteraction->snapMouseReleaseEvent(ev, aFeat);
        clearNoSnap();
        return;
    }

    Way* aRoad = dynamic_cast<Way*>(aFeat);
    if (!aFeat || aRoad) {
        SAFE_DELETE(theMoveInteraction);
        Coord P(XY_TO_COORD(ev->pos()));

        createNode(P, aFeat);

        theMoveInteraction = new MoveNodeInteraction(theMain);
#ifndef _MOBILE
        theMain->view()->setCursor(theMoveInteraction->cursor());
#endif
    }
}

#ifndef _MOBILE
QCursor CreateNodeInteraction::cursor() const
{
    return QCursor(Qt::CrossCursor);
}
#endif

void CreateNodeInteraction::createNode(Coord P, Feature* aFeat)
{
    Node* N;
    CommandList* theList;
    Way* aRoad = dynamic_cast<Way*>(aFeat);
    if (aRoad)
    {
        g_Merk_MainWindow->properties()->setSelection(0);
        theList  = new CommandList(MainWindow::tr("Create node in way %1").arg(aRoad->id().numId), aRoad);
        int SnapIdx = findSnapPointIndex(aRoad, P);
        N = g_backend.allocNode(g_Merk_MainWindow->document()->getDirtyOrOriginLayer(aRoad->layer()), P);
        theList->add(new AddFeatureCommand(g_Merk_MainWindow->document()->getDirtyOrOriginLayer(aRoad->layer()),N,true));
        theList->add(new WayAddNodeCommand(aRoad,N,SnapIdx,g_Merk_MainWindow->document()->getDirtyOrOriginLayer(aRoad->layer())));
    }
    else
    {
        N = g_backend.allocNode(g_Merk_MainWindow->document()->getDirtyOrOriginLayer(), P);
        theList  = new CommandList(MainWindow::tr("Create node %1").arg(N->id().numId), N);
        theList->add(new AddFeatureCommand(g_Merk_MainWindow->document()->getDirtyOrOriginLayer(),N,true));
        if (M_PREFS->getAutoSourceTag()) {
            QStringList sl = g_Merk_MainWindow->document()->getCurrentSourceTags();
            if (sl.size())
                N->setTag("source", sl.join(";"));
        }
        N->updateMeta();
    }
    g_Merk_MainWindow->document()->addHistory(theList);
    g_Merk_MainWindow->properties()->setSelection(N);
    g_Merk_MainWindow->view()->invalidate(true, true, false);
}
