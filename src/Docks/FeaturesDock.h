//
// C++ Interface: FeaturesDock
//
// Description:
//
//
// Author: cbro <cbro@semperpax.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FEATURESDOCK_H
#define FEATURESDOCK_H

#include "MDockAncestor.h"
#include "Coord.h"
#include "Feature.h"
#include "MapTypedef.h"

#include "ui_FeaturesDock.h"

class MainWindow;
class QAction;

class FeaturesDock : public MDockAncestor
{
Q_OBJECT
public:
    FeaturesDock(MainWindow* aParent);

    ~FeaturesDock();


    Feature* highlighted(int idx);
    QList<Feature*> highlighted();
    int highlightedSize() const;

public slots:
    void updateList();

    void on_FeaturesList_itemSelectionChanged();
    void on_FeaturesList_itemDoubleClicked(QListWidgetItem* item);
    void on_FeaturesList_customContextMenuRequested(const QPoint & pos);
    void on_FeaturesList_delete();

    void on_rbWithin_stateChanged ( int state );
    void on_rbSelectionFilter_stateChanged ( int state );

    void on_centerAction_triggered();
    void on_centerZoomAction_triggered();
    void on_downloadAction_triggered();
    void on_addSelectAction_triggered();

    void on_btFind_clicked(bool);
    void on_btReset_clicked(bool);

    void on_Viewport_changed(bool visible);

    void tabChanged(int idx);

    void invalidate();

private:
    QList<Feature*> Highlighted;
    QList<Feature*> Found;

    MainWindow* Main;
    Ui::FeaturesDockWidget ui;
    QAction* centerAction;
    QAction* centerZoomAction;
    QAction* downloadAction;
    QAction* addSelectAction;
    QAction* deleteAction;

    CoordBox theViewport;
    IFeature::FeatureType curFeatType;

    void clearItems();
    void addItem(MapFeaturePtr F);

    bool findMode;

public:
    void changeEvent(QEvent*);
    void retranslateUi();
    void retranslateTabBar();
};

#endif // FEATURESDOCK_H
