/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "nmuiheaders.h"

/*!
    \class NmMessageListViewItem
    \brief list view item for message list view
*/

static const int NmFolderTypeRole = Qt::UserRole+1; 

/*!
    Constructor
*/
NmMessageListViewItem::NmMessageListViewItem(QGraphicsItem *parent):
    HbTreeViewItem(parent),
    mSender(NULL),
    mSubject(NULL),
    mTime(NULL),
    mDividerTitle(NULL),
    mNewMsgIcon(NULL),
    mIcon1(NULL),
    mIcon2(NULL),
    mPrimarySize(0),
    mSecondarySize(0),
    mTinySize(0)    
{
    NM_FUNCTION;
}

/*!
    Destruction.
*/
NmMessageListViewItem::~NmMessageListViewItem()
{
    NM_FUNCTION;
}

/*!
*/
HbTreeViewItem *NmMessageListViewItem::createItem()
{
    NM_FUNCTION;
    
    return new NmMessageListViewItem(*this);
}

/*!
    boolean value indicating model index availability. Always true since
    this item prototype is used for both divider and message
*/
bool NmMessageListViewItem::canSetModelIndex(const QModelIndex &index) const
{
    NM_FUNCTION;
    
    Q_UNUSED(index);
    // This item class can handle all items in message list
    return true;
}

/*!
    update child items. List calls this function whenever item is needed
    to be updated.
*/
void NmMessageListViewItem::updateChildItems()
{
    NM_FUNCTION;
    
    // Get model pointer
    NmMessageListModelItem *msgModelItem = modelIndex().data(
            Qt::DisplayRole).value<NmMessageListModelItem*>();
    // Check whether item is message item or title divider
    // and set the layout accordingly, dividers not currently used
    if (msgModelItem && msgModelItem->itemType() ==
        NmMessageListModelItem::NmMessageItemMessage){
        NmMessageListModel *model = static_cast<NmMessageListModel*>(msgModelItem->model());
        if (model){
            // First set item layout
            createMessageItemLayout();
            // Set content data to item after layout is created
            setContentsToMessageItem(msgModelItem->envelope(), model->dividersActive());
        }
    }
    HbTreeViewItem::updateChildItems();
}

/*!
    Set item layout.
    Function does not take ownership of model or model item
*/
void NmMessageListViewItem::createMessageItemLayout()
{ 
    NM_FUNCTION;
    
    getFontSizes();
    // Create sender label and set name from widgetml
    if (!mSender){
        mSender = new HbTextItem(this);
        HbStyle::setItemName(mSender, "sender");
        mSender->setObjectName("ListViewItemMessageSender");
        mSender->setTextWrapping(Hb::TextNoWrap);
    }
    // Create time label and set name from widgetml
    if (!mTime){
        mTime = new HbTextItem(this);
        HbStyle::setItemName(mTime, "time");
        mTime->setObjectName("ListViewItemMessageTime");
    }
    // Create subject label and set name from widgetml
    if (!mSubject){
        mSubject = new HbTextItem(this);
        HbStyle::setItemName(mSubject, "subject");
        mSubject->setObjectName("ListViewItemMessageSubject");
        mSubject->setTextWrapping(Hb::TextNoWrap);
    }
    // Set new message icon
    if (!mNewMsgIcon) {
        mNewMsgIcon = new HbFrameItem(this);
     }
    // Create priority icon and set name from widgetml
    if (!mIcon1){
        mIcon1 = new HbIconItem(this);
        HbStyle::setItemName(mIcon1, "icon1");
        // According to layout guide, icon alignment is always right
        mIcon1->setAlignment(Qt::AlignRight);
    }
    // Create attachment icon and set name from widgetml
    if (!mIcon2){
        mIcon2 = new HbIconItem(this);
        HbStyle::setItemName(mIcon2, "icon2");
        // According to layout guide, icon alignment is always right
        mIcon2->setAlignment(Qt::AlignRight);
    }
    setObjectName("ListViewItemMessage");
}

/*!
    set item text label contents, icons, etc.
    Function does not take ownership of model or model item
*/
void NmMessageListViewItem::setContentsToMessageItem(const NmMessageEnvelope &envelope, 
                                                     bool dividersActive)
{
    NM_FUNCTION;
    
    // member variables are created in previous function
    // sender
    mSender->setText(senderFieldText(envelope));
    // time
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDateTime localTime = envelope.sentTime().addSecs(locale.universalTimeOffset());
    QDate sentLocalDate = localTime.date();
    QDate currentdate = QDate::currentDate();
    if (dividersActive || sentLocalDate == currentdate) {
        QString shortTimeSpec = r_qtn_time_usual;
        QTime time = localTime.time();
        mTime->setText(locale.format(time, shortTimeSpec));
    } else {
        QString shortDateSpec = r_qtn_date_without_year;
        mTime->setText(locale.format(sentLocalDate, shortDateSpec));
    }
    // subject 
    QString subjectText = envelope.subject();
    if (subjectText.length()) {
        mSubject->setText(subjectText);
    } else {
        mSubject->setText(hbTrId("txt_mail_dblist_val_no_subject"));
    }
    // priority
    bool priorityIs(false);
    mIcon1->hide();
    mIcon2->hide();
    switch (envelope.priority()) {
        case NmMessagePriorityLow:
            priorityIs = true;
            mIcon1->setObjectName("ListViewItemMessageIconPriorityLow");
            mIcon1->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityLow));
            mIcon1->show();
            break;
        case NmMessagePriorityHigh:
            priorityIs = true;
            mIcon1->setObjectName("ListViewItemMessageIconPriorityHigh");
            mIcon1->setIcon(NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
            mIcon1->show();
            break;
        case NmMessagePriorityNormal:
        default:
            break;
    }
    // attachments
    HbIconItem *attaIcon = (priorityIs ? mIcon2 : mIcon1);
    if (envelope.hasAttachments()) {
        HbIcon &icon = NmIcons::getIcon(NmIcons::NmIconAttachment);
        attaIcon->setIcon(icon);
        attaIcon->setObjectName("ListViewItemMessageIconAttachment");
        attaIcon->show();
    }
    // message read status
    bool msgReadStatus(envelope.isRead());
    HbFrameDrawer *drawer(NULL);
    HbStyle::setItemName(mNewMsgIcon, "msgicon");
    if (!msgReadStatus) {

        setFontsUnread();
        mNewMsgIcon->setObjectName("ListViewItemMessageIconUnread");
        drawer = new HbFrameDrawer("qtg_fr_list_new_item",
                HbFrameDrawer::ThreePiecesVertical);
        drawer->setFillWholeRect(false);
        // Set drawer. Takes ownership of the drawer and deletes previous drawer.
        mNewMsgIcon->setFrameDrawer(drawer);
        mNewMsgIcon->show();
    } else {

        setFontsRead();
        mNewMsgIcon->setObjectName("ListViewItemMessageIconRead");
        drawer = new HbFrameDrawer(); // Create empty drawer
        drawer->setFillWholeRect(false);
        // Set drawer. Takes ownership of the drawer and deletes previous drawer.
        mNewMsgIcon->setFrameDrawer(drawer);
        mNewMsgIcon->hide();        
    }
}

/*!
    paint. Paint function for line painting.
*/
void NmMessageListViewItem::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    NM_FUNCTION;
    
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
}

/*!
    setFontsUnread
*/
void  NmMessageListViewItem::setFontsUnread()
{    
    NM_FUNCTION;
    
    static QColor colorRole = HbColorScheme::color("qtc_list_item_title_normal");
    HbFontSpec spekki(HbFontSpec::Primary);
    setFonts(colorRole, spekki);
}

/*!
    setFontsRead
*/
void  NmMessageListViewItem::setFontsRead()
{
    NM_FUNCTION;
    
    static QColor colorRole = HbColorScheme::color("qtc_list_item_content_normal");
    HbFontSpec spekki(HbFontSpec::Secondary);
    setFonts(colorRole, spekki);
}

/*!
    getFontSizes.
*/
void  NmMessageListViewItem::getFontSizes()
{
    NM_FUNCTION;
    
    // Get font sizes from style  
    qreal currentSize;
    HbStyle mystyle;
    bool found = mystyle.parameter(QString("hb-param-text-height-primary"), currentSize );
    if (found) {
        mPrimarySize = currentSize;
    }      
    found = mystyle.parameter(QString("hb-param-text-height-secondary"), currentSize );
    if (found) {
        mSecondarySize = currentSize;
    }        
    found = mystyle.parameter(QString("hb-param-text-height-tiny"), currentSize );
    if (found) {
        mTinySize = currentSize;
    } 
}

/*!
    setFonts.
*/
void  NmMessageListViewItem::setFonts(const QColor &colorRole, 
        HbFontSpec &spekki)
{
    NM_FUNCTION;
    
    // Change sizes explicitly since css is overwritten in polish now.    
    if (mSender && mSubject && mTime) {
        
        if (mPrimarySize) {
            spekki.setTextHeight(mPrimarySize);
        }      
        mSender->setFontSpec(spekki);
        mSender->setTextColor(colorRole);
        
        if (mSecondarySize) {
            spekki.setTextHeight(mSecondarySize);
        }        
        mSubject->setFontSpec(spekki);
        mSubject->setTextColor(colorRole);

        if (mTinySize) {
            spekki.setTextHeight(mTinySize);
        } 
        mTime->setFontSpec(spekki);        
        mTime->setTextColor(colorRole);
    }
}

/*!
    senderFieldText. Function returns sender field text from
    envelope based on currently used function
*/
QString NmMessageListViewItem::senderFieldText(const NmMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    QString ret;  
    QVariant folderType = modelIndex().data(
            NmFolderTypeRole).value<QVariant>();
    switch (folderType.toInt()) {
        // Outbox, drafts and sent folder, sender name is 
        // replaced with first recipient from to/cc list
        case NmFolderOutbox:
        case NmFolderDrafts:
        case NmFolderSent:
        {
            QList<NmAddress>& toRecipients = envelope.toRecipients();
            QList<NmAddress>& ccRecipients = envelope.ccRecipients();
            NmAddress addressToUse;
            bool foundAddress(false);
            if (toRecipients.count()){
                addressToUse=toRecipients[0];
                foundAddress=true;
            }
            else if (ccRecipients.count()) {
                addressToUse=ccRecipients[0]; 
                foundAddress=true;         
            }
            if (foundAddress){
                QString displayName = addressToUse.displayName();
                if (displayName.length()) {
                    ret += NmUtilities::cleanupDisplayName(displayName);
                } 
                else {
                    ret += addressToUse.address();                    
                }               
            }
            
        }
        break;    
        // All other folders will show sender display name
        default: 
        {
            QString displayName = envelope.sender().displayName();
            if (displayName.length()) {
                ret += NmUtilities::cleanupDisplayName(displayName);
            } 
            else {
                ret += envelope.sender().address();                    
            }
        }
        break;
    }
    return ret;
}

