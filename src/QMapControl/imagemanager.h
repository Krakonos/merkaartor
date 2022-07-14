/***************************************************************************
 *   Copyright (C) 2007 by Kai Winter   *
 *   kaiwinter@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QBuffer>
#include <QDebug>
#include <QMutex>
#include <QFileInfo>
#include <QCache>
#include "mapnetwork.h"

#include "IImageManager.h"

class MapNetwork;
class IMapAdapter;

/**
    @author Kai Winter <kaiwinter@gmx.de>
*/
class ImageManager : public QObject, public IImageManager
{
    Q_OBJECT;
    public:
        ImageManager(QObject* parent = 0);
        ImageManager(const ImageManager&) = default;
        ImageManager& operator=(const ImageManager&) = default;
        ~ImageManager();

        //! returns a QPixmap of the asked image
        /*!
         * If this component doesn´t have the image a network query gets started to load it.
         * @param host the host of the image
         * @param path the path to the image
         * @return the pixmap of the asked image
         */
        QImage getImage(IMapAdapter* anAdapter, const QString &url);
        QByteArray getData(IMapAdapter* anAdapter, const QString &url);

        //QPixmap prefetchImage(const QString& host, const QString& path);
        QImage prefetchImage(IMapAdapter* anAdapter, int x, int y, int z);

        void receivedData(const QByteArray& ba, const QHash<QString, QString>& headers, const QString& url);

        /*!
         * This method is called by MapNetwork, after all images in its queue were loaded.
         * The ImageManager emits a signal, which is used in MapControl to remove the zoom image.
         * The zoom image should be removed on Tile Images with transparency.
         * Else the zoom image stay visible behind the newly loaded tiles.
         */
        void loadingQueueEmpty();

        /*!
         * Aborts all current loading threads.
         * This is useful when changing the zoom-factor, though newly needed images loads faster
         */
        void abortLoading();

        void setCacheDir(const QDir& path);
        QDir getCacheDir();
        void setCacheMaxSize(int max);

    private:
        QPixmap emptyPixmap;
        MapNetwork* net;
        QStringList prefetch;

        static ImageManager* m_ImageManagerInstance;

        QCache<QString, QBuffer> m_dataCache;

    signals:
        void dataRequested();
        void dataReceived();
        void loadingFinished();
};

#endif
