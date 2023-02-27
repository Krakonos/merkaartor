#include "CreatePolygonInteraction.h"

#include "MainWindow.h"
#include "DocumentCommands.h"
#include "WayCommands.h"
#include "NodeCommands.h"
#include "Painting.h"
#include "Way.h"
#include "Node.h"
#include "LineF.h"
#include "PropertiesDock.h"
#include "MerkaartorPreferences.h"
#include "Global.h"

#include <QtGui/QPainter>
#include <QInputDialog>

#include <math.h>

CreatePolygonInteraction::CreatePolygonInteraction(MainWindow* aMain, int sides, const QList< QPair <QString, QString> >& tags)
    : Interaction(aMain), Origin(0,0), Sides(sides), HaveOrigin(false), bAngle(0.0), bScale(QPointF(1., 1.)), theTags(tags)
{
#ifndef _MOBILE
    theMain->view()->setCursor(cursor());
#endif
}

CreatePolygonInteraction::~CreatePolygonInteraction()
{
    view()->update();
}

QString CreatePolygonInteraction::toHtml()
{
    QString help;
    help = (MainWindow::tr("LEFT-CLICK to start;DRAG to scale;SHIFT-DRAG to rotate;LEFT-CLICK to end"));

    QStringList helpList = help.split(";");

    QString desc;
    desc = QString("<big><b>%1</b></big>").arg(MainWindow::tr("Create Polygon interaction"));

    QString S =
    "<html><head/><body>"
    "<small><i>" + QString(metaObject()->className()) + "</i></small><br/>"
    + desc;
    S += "<hr/>";
    S += "<ul style=\"margin-left: 0px; padding-left: 0px;\">";
    for (int i=0; i<helpList.size(); ++i) {
        S+= "<li>" + helpList[i] + "</li>";
    }
    S += "</ul>";
    S += "</body></html>";

    return S;
}


void CreatePolygonInteraction::mousePressEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (!HaveOrigin)
        {
            HaveOrigin = true;
            view()->setInteracting(true);
            Origin = XY_TO_COORD(event->pos());
            OriginF = QPointF(event->pos());
            bAngle = 0.;
            bScale = QPointF(1., 1.);
        }
        else
        {
            QPointF CenterF(0.5, 0.5);
            qreal Radius = 0.5;
            if (Sides == 4)
                Radius = sqrt(2.)/2.;
            qreal Angle = 2*M_PI/Sides;
            QBrush SomeBrush(QColor(0xff,0x77,0x11,128));
            QPen TP(SomeBrush,view()->pixelPerM()*4);

            QTransform transform;
            transform.translate(OriginF.x(), OriginF.y());
            transform.rotate(bAngle);
            transform.scale(bScale.x(), bScale.y());

            QPointF Prev(CenterF.x()+cos(-Angle/2)*Radius,CenterF.y()+sin(-Angle/2)*Radius);
            Node* First = g_backend.allocNode(theMain->document()->getDirtyOrOriginLayer(), XY_TO_COORD(transform.map(Prev).toPoint()));
            Way* R = g_backend.allocWay(theMain->document()->getDirtyOrOriginLayer());
            CommandList* L  = new CommandList(MainWindow::tr("Create polygon %1").arg(R->id().numId), R);
            L->add(new AddFeatureCommand(theMain->document()->getDirtyOrOriginLayer(),R,true));
            R->add(First);
            L->add(new AddFeatureCommand(theMain->document()->getDirtyOrOriginLayer(),First,true));
            for (qreal a = 2*M_PI - Angle*3/2; a>0; a-=Angle)
            {
                QPointF Next(CenterF.x()+cos(a)*Radius,CenterF.y()+sin(a)*Radius);
                Node* New = g_backend.allocNode(theMain->document()->getDirtyOrOriginLayer(), XY_TO_COORD(transform.map(Next).toPoint()));
                L->add(new AddFeatureCommand(theMain->document()->getDirtyOrOriginLayer(),New,true));
                R->add(New);
            }
            R->add(First);
            if (M_PREFS->getAutoSourceTag()) {
                QStringList sl = theMain->document()->getCurrentSourceTags();
                if (sl.size())
                    R->setTag("source", sl.join(";"));
            }
            QPair <QString, QString> tag;
            foreach (tag, theTags) {
                R->setTag(tag.first, tag.second);
            }
            for (FeatureIterator it(document()); !it.isEnd(); ++it)
            {
                Way* W1 = dynamic_cast<Way*>(it.get());
                if (W1 && (W1 != R))
                    Way::createJunction(theMain->document(), L, R, W1, true);
            }
            theMain->properties()->setSelection(R);
            document()->addHistory(L);
            view()->setInteracting(false);
            view()->invalidate(true, true, false);
            theMain->launchInteraction(0);
        }
    }
    else
        Interaction::mousePressEvent(event);
}

void CreatePolygonInteraction::paintEvent(QPaintEvent* , QPainter& thePainter)
{
    if (HaveOrigin)
    {
        QPointF CenterF(0.5, 0.5);
        qreal Radius = 0.5;
        if (Sides == 4)
            Radius = sqrt(2.)/2.;

        QTransform transform;
        transform.translate(OriginF.x(), OriginF.y());
        transform.rotate(bAngle);
        transform.scale(bScale.x(), bScale.y());
        QPolygonF thePoly = transform.map(QRectF(QPointF(0.0, 0.0), QPointF(1.0, 1.0)));

        thePainter.setPen(QPen(QColor(0,0,255),1,Qt::DotLine));
        thePainter.drawPolygon(thePoly);

        qreal Angle = 2*M_PI/Sides;
        QBrush SomeBrush(QColor(0xff,0x77,0x11,128));
        QPen TP(SomeBrush,view()->pixelPerM()*4+1);
        QPointF Prev(CenterF.x()+cos(-Angle/2)*Radius,CenterF.y()+sin(-Angle/2)*Radius);
        for (qreal a = 2*M_PI - Angle*3/2; a>0; a-=Angle)
        {
            QPointF Next(CenterF.x()+cos(a)*Radius,CenterF.y()+sin(a)*Radius);
            ::draw(thePainter,TP,Feature::UnknownDirection, transform.map(Prev),transform.map(Next),4,view()->projection());
            Prev = Next;
        }
        QPointF Next(CenterF.x()+cos(-Angle/2)*Radius,CenterF.y()+sin(-Angle/2)*Radius);
        ::draw(thePainter,TP,Feature::UnknownDirection, transform.map(Prev),transform.map(Next),4,view()->projection());
    }
}

void CreatePolygonInteraction::mouseMoveEvent(QMouseEvent* event)
{
    if (HaveOrigin) {
        OriginF = COORD_TO_XY(Origin);

        QTransform transform;
        transform.translate(OriginF.x(), OriginF.y());
        transform.rotate(bAngle);

        if (event->modifiers() & Qt::ShiftModifier) {
            bAngle += radToAng(angle(transform.inverted().map(LastCursor), transform.inverted().map(event->pos())));

            QTransform transform2;
            transform2.translate(OriginF.x(), OriginF.y());
            transform2.rotate(bAngle);
            bScale = transform2.inverted().map(event->pos());
        } else {
            bScale = transform.inverted().map(event->pos());
        }

        view()->update();
    }
    LastCursor = event->pos();
    Interaction::mouseMoveEvent(event);
}

void CreatePolygonInteraction::mouseReleaseEvent(QMouseEvent* event)
{
    if (M_PREFS->getMouseSingleButton() && event->button() == Qt::RightButton) {
        HaveOrigin = false;
        view()->setInteracting(false);
        view()->update();
    }
    Interaction::mouseReleaseEvent(event);
}


#ifndef _MOBILE
QCursor CreatePolygonInteraction::cursor() const
{
    return QCursor(Qt::CrossCursor);
}
#endif
