/*
 * Copyright (C) 2015-2022 Département de l'Instruction Publique (DIP-SEM)
 *
 * Copyright (C) 2013 Open Education Foundation
 *
 * Copyright (C) 2010-2013 Groupement d'Intérêt Public pour
 * l'Education Numérique en Afrique (GIP ENA)
 *
 * This file is part of OpenBoard.
 *
 * OpenBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * OpenBoard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBoard. If not, see <http://www.gnu.org/licenses/>.
 */




#include "UBDocumentProxy.h"

#include "frameworks/UBStringUtils.h"

#include "core/UBApplication.h"
#include "core/UBPersistenceManager.h"
#include "core/UBSettings.h"
#include "core/UBDocumentManager.h"
#include "core/memcheck.h"

#include "adaptors/UBMetadataDcSubsetAdaptor.h"

UBDocumentProxy::UBDocumentProxy()
    : mPageCount(0)
    , mPageDpi(0)
    , mPersistencePath("")
    , mDocumentDateLittleEndian("")
    , mDocumentUpdatedAtLittleEndian("")
    , mNeedsCleanup(true)
    , mLastVisitedIndex(0)
    , mIsInFavoriteList(false)
{
    init();
}


UBDocumentProxy::UBDocumentProxy(const QString& pPersistancePath)
    : mPageCount(0)
    , mPageDpi(0)
    , mNeedsCleanup(true)
    , mLastVisitedIndex(0)
    , mIsInFavoriteList(false)
{
    init();
    setPersistencePath(pPersistancePath);

    mMetaDatas = UBMetadataDcSubsetAdaptor::load(pPersistancePath);
}


void UBDocumentProxy::init()
{
    setMetaData(UBSettings::documentGroupName, "");

    QDateTime now = QDateTime::currentDateTime();
    setMetaData(UBSettings::documentName, QLocale::system().toString(now, QLocale::ShortFormat));

    setUuid(QUuid::createUuid());

    setDefaultDocumentSize(UBSettings::settings()->pageSize->get().toSize());
}

bool UBDocumentProxy::theSameDocument(std::shared_ptr<UBDocumentProxy> proxy)
{
    return  proxy && mPersistencePath == proxy->mPersistencePath;
}

UBDocumentProxy::~UBDocumentProxy()
{
    // NOOP
}

std::shared_ptr<UBDocumentProxy> UBDocumentProxy::deepCopy() const
{
    std::shared_ptr<UBDocumentProxy> copy = std::make_shared<UBDocumentProxy>();

    copy->mPersistencePath = QString(mPersistencePath);
    copy->mMetaDatas = QMap<QString, QVariant>(mMetaDatas);
    copy->mPageCount = mPageCount;
    copy->mLastVisitedIndex = mLastVisitedIndex;
    copy->mIsInFavoriteList = mIsInFavoriteList;

    return copy;
}

int UBDocumentProxy::lastVisitedSceneIndex() const
{
    return mLastVisitedIndex;
}

void UBDocumentProxy::setLastVisitedSceneIndex(int lastVisitedSceneIndex)
{
    mLastVisitedIndex = lastVisitedSceneIndex;
}

bool UBDocumentProxy::isInFavoriteList() const
{
    return mIsInFavoriteList;
}

void UBDocumentProxy::setIsInFavoristeList(bool isInFavoriteList)
{
    mIsInFavoriteList = isInFavoriteList;
}

int UBDocumentProxy::pageCount()
{
    return mPageCount;
}


void UBDocumentProxy::setPageCount(int pPageCount)
{
    mPageCount = pPageCount;
}

int UBDocumentProxy::pageDpi()
{
    return mPageDpi;
}

void UBDocumentProxy::setPageDpi(int dpi)
{
    mPageDpi = dpi;
}

bool UBDocumentProxy::isWidgetCompatible(const QUuid &uuid) const
{
    return mWidgetCompatibility.value(uuid, true);
}

void UBDocumentProxy::setWidgetCompatible(const QUuid &uuid, bool compatible)
{
    mWidgetCompatibility[uuid] = compatible;
}

bool UBDocumentProxy::testAndResetCleanupNeeded()
{
    bool tmp = mNeedsCleanup;
    mNeedsCleanup = false;
    return tmp;
}

int UBDocumentProxy::incPageCount()
{
    if (mPageCount <= 0)
    {
        mPageCount = 1;
    }
    else
    {
        mPageCount++;
    }

    return mPageCount;

}


int UBDocumentProxy::decPageCount()
{
    mPageCount --;

    if (mPageCount < 0)
    {
        mPageCount = 0;
    }

    mNeedsCleanup = true;

    return mPageCount;
}

QString UBDocumentProxy::persistencePath() const
{
    return mPersistencePath;
}

QString UBDocumentProxy::documentFolderName() const
{
    return mPersistencePath.section('/', -1);
}

void UBDocumentProxy::setPersistencePath(const QString& pPersistencePath)
{
    if (pPersistencePath != mPersistencePath)
    {
        mPersistencePath = pPersistencePath;
    }
}

void UBDocumentProxy::setMetaData(const QString& pKey, const QVariant& pValue)
{
    if (mMetaDatas.contains(pKey) && mMetaDatas.value(pKey) == pValue)
        return;
    else
    {
        mMetaDatas.insert(pKey, pValue);
        if (pKey == UBSettings::documentUpdatedAt)
        {
            mDocumentUpdatedAtLittleEndian = "";
        }
    }
}

QVariant UBDocumentProxy::metaData(const QString& pKey) const
{
    if (mMetaDatas.contains(pKey))
    {
        return mMetaDatas.value(pKey);
    }
    else
    {
        qDebug() << "Unknown metadata key" << pKey;
        return QString(""); // failsafe
    }
}

QMap<QString, QVariant> UBDocumentProxy::metaDatas() const
{
    return mMetaDatas;
}

QString UBDocumentProxy::name() const
{
    return metaData(UBSettings::documentName).toString();
}

QString UBDocumentProxy::groupName() const
{
    return metaData(UBSettings::documentGroupName).toString();
}

QSize UBDocumentProxy::defaultDocumentSize() const
{
    if (mMetaDatas.contains(UBSettings::documentSize))
        return metaData(UBSettings::documentSize).toSize();
    else
        return UBSettings::settings()->pageSize->get().toSize();
}

void UBDocumentProxy::setDefaultDocumentSize(QSize pSize)
{
    if (defaultDocumentSize() != pSize)
    {
        setMetaData(UBSettings::documentSize, QVariant(pSize));
    }
}

void UBDocumentProxy::setDefaultDocumentSize(int pWidth, int pHeight)
{
    setDefaultDocumentSize(QSize(pWidth, pHeight));
}


QUuid UBDocumentProxy::uuid() const
{
    QString id = metaData(UBSettings::documentIdentifer).toString();
    QString sUuid = id.replace(UBSettings::uniboardDocumentNamespaceUri + "/", "");

    return QUuid(sUuid);
}

void UBDocumentProxy::setUuid(const QUuid& uuid)
{
    setMetaData(UBSettings::documentIdentifer,
            UBSettings::uniboardDocumentNamespaceUri + "/" + UBStringUtils::toCanonicalUuid(uuid));
}


QDateTime UBDocumentProxy::documentDate()
{
    if(mMetaDatas.contains(UBSettings::documentDate))
        return UBStringUtils::fromUtcIsoDate(metaData(UBSettings::documentDate).toString());
    return QDateTime::currentDateTime();
}

QDateTime UBDocumentProxy::lastUpdate()
{
    if(mMetaDatas.contains(UBSettings::documentUpdatedAt))
    {
        return UBStringUtils::fromUtcIsoDate(metaData(UBSettings::documentUpdatedAt).toString());
    }
    return QDateTime::currentDateTime();
}





