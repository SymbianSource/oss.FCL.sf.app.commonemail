/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Private implementation of Contact History Model API
*
*/

#include "emailmru.h"
#include "nmcontacthistorymodel_p.h"

// --------------------------------------------------------------------------
// Start of implementation (NmContactHistoryModelPrivate)
// --------------------------------------------------------------------------
/*!
    Constructor of NmContactHistoryModelPrivate

    \param type The valid values are described by
    NmContactHistoryModelPrivate::HistoryModelType enum.
*/
NmContactHistoryModelPrivate::NmContactHistoryModelPrivate(
    const NmContactHistoryModelType modelType) :
    mType(modelType),
    mContactManager(0),
    mModelReady(false)
{
    mContactManager = new QContactManager("symbian");
    mNameOrder = EmailMRU::nameOrder();
}

/*!
    Destructor of ContactHistoryModel
*/
NmContactHistoryModelPrivate::~NmContactHistoryModelPrivate()
{
    delete mContactManager;
    mPrivateItemList.clear();
    mModelItemList.clear();
}

/*!
    Searches contact & MRU  databases according to query parameters.
    Emits QueryCompleted() when ready.

    \param query Text to be searched
    \return none

    Note:: QContactManager supports max. ten characters long queries.
*/
void NmContactHistoryModelPrivate::queryDatabases(const QString& query)
{
    mModelReady = false;
    // Clear contacts in the list
    mPrivateItemList.clear();
    mModelItemList.clear();
    mMruList.clear();
    mMruMatches.clear();

    // Modify search to suit our needs
    // Space must be removed, because it is understood as logigal AND
    // with QContactManager API
    QString modifiedQuery = query;

    int spcPosition = query.indexOf(" ");

    if ( spcPosition != -1 )
        {
        modifiedQuery = query.left(spcPosition);
        }

    queryMruDatabase(modifiedQuery);
    populateListWithMruItems(modifiedQuery);

    if (mContactManager)
        {
        // Get matching IDs from Contacts DB
        QList<QContactLocalId> cnt_ids = queryContactDatabase(modifiedQuery);

        // Populate mPrivateItemList with contact items.
        populateListWithContactItems(cnt_ids, modifiedQuery);
        }

    // Currently we will always emit 0 as completion code.

    //TODO: Will be removed, Debug Code.
    for (int i = 0; i < mPrivateItemList.size(); i++)
        {
        QSharedPointer<NmContactHistoryModelItemData> itemData = mPrivateItemList[i];

        QString dbgString = itemData->mItems[0].mItemText;
        dbgString.append(" ");
        dbgString.append(itemData->mItems[1].mItemText);

        qDebug(dbgString.toLatin1());


        }

    emit queryCompleted(0);
}

/*!
    Internal helper function for queryDatabases()
    Not meant to be used alone.

    \param query Text to be searched
    \return list of Contact items.
    \sa queryDatabases( )
*/
QList<QContactLocalId> NmContactHistoryModelPrivate::queryContactDatabase(
    const QString &query)
{
    // Define filter
    QContactDetailFilter df;

    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName,
                               QContactEmailAddress::FieldEmailAddress );

    df.setMatchFlags( QContactFilter::MatchKeypadCollation );

    // Construct pattern
    QString pattern = query + QChar(30) + QString("vqwerty");
    df.setValue( pattern );

    // Get matching IDs
    QList<QContactLocalId> cnt_ids = mContactManager->contactIds( df );

    return cnt_ids;
}

/*!
    Internal helper function for queryDatabases()
    Not meant to be used alone.

    \param query Text to be searched from MRU
    \return true on success.
    \sa queryDatabases( )
*/
bool NmContactHistoryModelPrivate::queryMruDatabase(
    const QString &query)
{
    if (mType == EmailAddressModel)
    {
        bool mruListFilled = fillMruMatchList();

        if (!mruListFilled)
        {
            return false;
        }

        QHashIterator<QString, QString> i(mMruList);

        while (i.hasNext())
        {
            i.next();

            QString address = i.key();
            QString name = i.value();

            if ((name.length() == 0) && (address.contains('@')))
            {
                name = address.left(address.indexOf("@"));
            }

            if (address.contains(query, Qt::CaseInsensitive))
            {
                mMruMatches.insert(name, address);
            }
            else if (name.contains(query, Qt::CaseInsensitive))
            {
                mMruMatches.insert(name, address);
            }
        }
    }

    return true;
}

/*!
    Internal helper function for queryDatabases()
    Not meant to be used alone.

    \param cnt_ids List of contact IDs
    \param modifiedQuery Query
    \sa queryDatabases( )
*/
void NmContactHistoryModelPrivate::populateListWithContactItems(
    const QList<QContactLocalId> cnt_ids,
    const QString &modifiedQuery)
{
    int cntCount = cnt_ids.count();
    // Populate mPrivateItemList with contact items.
    for ( int cntIndex = 0; cntIndex < cntCount; cntIndex++)
    {
        QContactLocalId cid = cnt_ids.at( cntIndex );
        // Fetch extended contact information from DB by ID
        QContact contact = mContactManager->contact( cid );

        // Get number of associated emails for contact
        int numberOfEmails =  contact.details(
            QContactEmailAddress::DefinitionName).count();

        if (numberOfEmails == 0)
        {
            continue; // Scotty, I need warp speed.
        }

        // Get contact name details
        QContactName contactName = contact.detail(
            QContactName::DefinitionName);

        QString firstName = contactName.value(
            QContactName::FieldFirstName);
        QString lastName = contactName.value(
            QContactName::FieldLastName);

        for (int emailIndex = 0; emailIndex < numberOfEmails; emailIndex++)
        {
            // For Display name (ex. John Doe)
            NmContactHistoryModelSubItem itemSubItem1;
            // For Email address (ex. john.doe@company.com)
            NmContactHistoryModelSubItem itemSubItem2;

            int queryLength = modifiedQuery.length();

            // Obey the contacts setting (first name last name order)
            itemSubItem1.mItemText = obeyContactOrder(firstName, lastName);

            if( firstName.indexOf(modifiedQuery, 0, Qt::CaseInsensitive) == 0)
            {
                itemSubItem1.mMatchingRanges.append(0);
                itemSubItem1.mMatchingRanges.append(queryLength-1);
            }

            if( lastName.indexOf(modifiedQuery, 0, Qt::CaseInsensitive) == 0)
            {
                int indexLN = itemSubItem1.mItemText.indexOf(lastName,
                    firstName.length(),
                    Qt::CaseInsensitive );

                itemSubItem1.mMatchingRanges.append(indexLN);
                itemSubItem1.mMatchingRanges.append(indexLN+(queryLength-1));
            }

            itemSubItem2.mItemText = contact.details (
                 QContactEmailAddress::DefinitionName )[emailIndex].value (
                 QContactEmailAddress::FieldEmailAddress );

            if (itemSubItem2.mItemText.indexOf(modifiedQuery, 0,
                Qt::CaseInsensitive) == 0 )
            {
                itemSubItem2.mMatchingRanges.append(0);
                itemSubItem2.mMatchingRanges.append(queryLength-1);
            }

            if ((itemSubItem1.mMatchingRanges.count() != 0) ||
                (itemSubItem2.mMatchingRanges.count() != 0) )
            {
                QSharedPointer<NmContactHistoryModelItemData> itemData
                    (new NmContactHistoryModelItemData);

                itemData->mContactId = cid;
                itemData->mItems.append(itemSubItem1);
                itemData->mItems.append(itemSubItem2);

                mPrivateItemList.append(itemData);
            }
        }
    }
}

/*!
    Internal helper function for queryDatabases()
    Not meant to be used alone.

    \param modifiedQuery Query
    \sa queryDatabases( )
*/
void NmContactHistoryModelPrivate::populateListWithMruItems(
    const QString &query)
{
    QMapIterator<QString, QString> i(mMruMatches);

    while (i.hasNext())
    {
        i.next();
        // For Display name (ex. John Doe)
        NmContactHistoryModelSubItem itemSubItem1;
        // For Email address (ex. john.doe@company.com)
        NmContactHistoryModelSubItem itemSubItem2;

        itemSubItem1.mItemText = i.key();
        itemSubItem2.mItemText = i.value();

        // markup for first item.
        if( i.key().indexOf(query, 0, Qt::CaseInsensitive) == 0)
        {
            itemSubItem1.mMatchingRanges.append(0);
            itemSubItem1.mMatchingRanges.append(query.length()-1);
        }

        QRegExp rx("[,\\s]", Qt::CaseInsensitive);
        bool separatorExists = i.key().contains(rx);

        if (separatorExists)
        {
            int indexLN = i.key().indexOf(",", 0, Qt::CaseInsensitive);

            if (indexLN == -1)
            {
                indexLN = i.key().indexOf(" ", 0, Qt::CaseInsensitive);
            }

            if (indexLN > 0)
            {
                int matchPos = i.key().indexOf(query, indexLN, Qt::CaseInsensitive);

                if (matchPos > 0)
                {
                    itemSubItem1.mMatchingRanges.append(matchPos);
                    itemSubItem1.mMatchingRanges.append((matchPos+query.length())-1);
                }
            }
        }

        // markup for second item.
        if (itemSubItem2.mItemText.indexOf(query, 0, Qt::CaseInsensitive) == 0 )
         {
             itemSubItem2.mMatchingRanges.append(0);
             itemSubItem2.mMatchingRanges.append(query.length()-1);
         }

        // If match, add to data list.
        if ((itemSubItem1.mMatchingRanges.count() != 0) ||
            (itemSubItem2.mMatchingRanges.count() != 0) )
        {
            QSharedPointer<NmContactHistoryModelItemData> itemData
                (new NmContactHistoryModelItemData);

            itemData->mContactId = 0; // No ID available, use 0.
            itemData->mItems.append(itemSubItem1);
            itemData->mItems.append(itemSubItem2);

            mPrivateItemList.append(itemData);
        }
    }
}

/*!
    After queryDatabases has completed this will be called from public class()
    Not meant to be used alone. (NmContactHistoryModel is using this when needed)

    \sa queryDatabases( )
*/
void NmContactHistoryModelPrivate::refreshDataModel()
{
    mModelItemList.clear();

    for (int cntIndex = 0;
             cntIndex < mPrivateItemList.count();
             cntIndex++)
    {
        // Construct modelItem to be added to list.
        NmContactHistoryModelItem modelItem;

        QSharedPointer<NmContactHistoryModelItemData> itemData(
            mPrivateItemList[cntIndex]);

        int id = itemData->mContactId;
        modelItem.setContactId(itemData->mContactId);

        for (int itemIndex = 0;
                 itemIndex < itemData->mItems.count();
                 itemIndex++)
        {
            QString key = itemData->mItems[itemIndex].mItemText;
            modelItem.appendSubItem(itemData->mItems[itemIndex]);
        }

        // Append modelItem to list.
        mModelItemList.append(modelItem);
    }

    // Notify views that we are done updating model data.
    mModelReady = true;

}

/*!
    This is called by public class NmContactHistoryModel when rowcount() is requested
    Not meant to be used alone.

*/
int NmContactHistoryModelPrivate::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModelItemList.count();
}

/*!
    This is called by public class NmContactHistoryModel when data() is requested
    Not meant to be used alone.

*/
QVariant NmContactHistoryModelPrivate::data(const QModelIndex &index, int role) const
{
    if ( mModelReady )
    {
        if (!index.isValid() ||
            index.row() >= mModelItemList.count() ||
            index.row() < 0)
        {
            return QVariant();
        }

        if (role == Qt::DisplayRole)
        {
            QVariant v;
            NmContactHistoryModelItem i = mModelItemList.at(index.row());
            v.setValue(i);
            return v;
        }
    }

    return QVariant();
}

/*!
    Fills MruMatchList, called from queryMruDatabase.
    Function uses Email MRU API to fetch information from CenRep

    \sa queryMruDatabase( )
*/
bool NmContactHistoryModelPrivate::fillMruMatchList()
{
    //TODO: Possible optimization, remove extra calls EmailMRU(), We need to get list only once.
    EmailMRU *mru = new EmailMRU();

    if (mru)
    {
        int addressCount = mru->entryCount();

        for (int i = addressCount; i > 0; i--)
        {
            QString key, value; // "Email or Number" & "Display Name"
            bool success;

            success = mru->getEntry(i, value, key);

            if (success)
            {
                mMruList.insertMulti(key, value);
            }
        }

        delete mru;
        return true;
    }
    else
    {
        return false;
    }
}

/*!
    This is called from populateListWithContactItems() when correct order of
    first name, last name setting is required

    \param firstName First name
    \param lastName Last name

    \sa populateListWithContactItems( )

    \return concennated string formatted as contact setting specifies.
*/
QString NmContactHistoryModelPrivate::obeyContactOrder(const QString &firstName,
                                                       const QString &lastName)
{
    QString result;

    switch (mNameOrder)
    {
        case LastNameFirstName:
            result = lastName + " " + firstName;
        break;

        case LastNameCommaFirstName:
            result = lastName + ", " + firstName;
        break;

        case FirstNameLastName:
            result = firstName + " " + lastName;
        break;

        default:
            result = firstName + " " + lastName;
        break;
    }

    return result;
}

// --------------------------------------------------------------------------
// End of implementation (NmContactHistoryModelPrivate)
// --------------------------------------------------------------------------

