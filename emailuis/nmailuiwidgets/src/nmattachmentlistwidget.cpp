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
* Description: Attachment list widget
*
*/

#include "nmailuiwidgetsheaders.h"

static const QString FILE_PATH_DOCML = ":nmattachmentlistwidget.docml";
static const QString ATTACHMENT_WIDGET = "nmattachmentlistwidget";

/*!
 @nmailuiwidgets
 \class NmAttachmentListWidget

 \brief The NmAttachmentListWidget widget provides a common look and feel for attachment lists.

Basic usage of the widget:
\code
NmAttachmentListWidget* attachmentWidget = new NmAttachmentListWidget(this);
connect(attachmentWidget,SIGNAL(itemActivated(int)),this,SLOT(attachmentActivated(int)));
connect(attachmentWidget,SIGNAL(longPressed(int,QPointF)),this,SLOT(attachmentLongPressed(int,QPointF)));
... (loop)
attachmentWidget->insertAttachment(i,list[i].attachmentNameString,attachmentSizeString);
... (loop)
layout()->addItem(attachmentWidget);

attachmentWidget->setProgressValue(index, value);
\endcode

 The NmAttachmentListWidget contains the following UI graphics items by default:
 \li file name (HbTextItem)
 \li file size (HbTextItem)

 The NmAttachmentListWidget may contain the following optional UI graphics items:
 \li progressbar (HbProgressBar)

 The progressbar will exists when setProgressValue function is called with value over 0.
 */

/*!
    \fn void NmAttachmentListWidget::insertAttachment(int index, const QString &fileName, const QString &fileSize)
    Inserts attachment to given index with name and size.
    \a index Index where attachment will be added in QList
    \a fileName Full name of attachment
    \a fileSize Full size of attachment in MB with accuracy one decimal
*/

/*!
    \fn int NmAttachmentListWidget::removeAttachment(int index)
    Removes attachment from given index.
    \a index Attachment index in QList
*/

/*!
    \fn int NmAttachmentListWidget::count()
    Returns attachment(s) count.
*/

/*!
    \fn int NmAttachmentListWidget::progressValue(int index)
    Returns progress bar value (0-100) of given attachment.
    \a index Attachment index in QList
*/

/*!
    \fn void NmAttachmentListWidget::setProgressValue(int index, int value)
    Set progress bar value (0-100) for given attachment.
    \a index Attachment index in QList
    \a value Value for progress bar
*/

/*!
    \fn void NmAttachmentListWidget::itemActivated(int index)
    Emits signal when attachment item is clicked. Item is identified by parameter.
    \a index Attachment index in QList
*/

/*!
    \fn void NmAttachmentListWidget::longPressed(int index, QPointF point);
    Emits signal when attachment item is long pressed.
    Item identification and the scene position of click are given by parameters.
    \a index Attachment index in QList
    \a point Point of location
*/


/*!
    Constructs a new NmAttachmentListWidget with \a parent.
 */
NmAttachmentListWidget::NmAttachmentListWidget(QGraphicsItem *parent)
    : HbWidget( parent ),
    mLayout(NULL),
    mOrientation(Qt::Vertical)
{
    init( );
}


/*!
    Destructor.
 */
NmAttachmentListWidget::~NmAttachmentListWidget( )
{
    mItemList.clear();
}

/*!
    Inserts attachment to given index. If index already contains data,
    old one will be moved to next.
 */
void NmAttachmentListWidget::insertAttachment(
        int index, 
        const QString &fileName, 
        const QString &fileSize)
{   
    NmAttachmentListItem *item = new NmAttachmentListItem(this);
    item->setObjectName(QString("nmattachmentlistitem_%1").arg(index));

    //connect to signals
    connect(item, SIGNAL(itemActivated()), this, SLOT(handleItemActivated()));
    connect(item, SIGNAL(itemLongPressed(QPointF)), this, SLOT(handleLongPressed(QPointF)));

    //set texts
    item->setTextItems(fileName,fileSize);

    //finally add item to item's list
    mItemList.insert(index,item);

    //do layout again
    rearrangeLayout();
}

/*!
    Removes attachment from given index.
 */
void NmAttachmentListWidget::removeAttachment(int index)
{
    if(!mLayout) {
        NMLOG("NmAttachmentListWidget::removeAttachment: Layout loading failed!");
        return;
    }

    if(index >= 0 && index < mItemList.count())
    {
        //remove from layout
        mLayout->removeAt(index);
        //remove from the array
        NmAttachmentListItem *item = mItemList.takeAt(index);
        delete item;
        item = NULL;

        //do layout again
        rearrangeLayout();
    }
}

/*!
    Returns attachment(s) count. 
 */
int NmAttachmentListWidget::count() const
{
    return mItemList.count();
}

/*!
    Returns value of item, otherwise -1 if index not found from list.
 */
int NmAttachmentListWidget::progressValue(int index) const
{
    int ret(NmNotFoundError);
    if(index >= 0 && index < mItemList.count()){
        ret = mItemList.at(index)->progressBarValue();
    }
    return ret;
}

/*!
    Public slot connected to set items value. Shows progress bar when called
    with match index and positive value (1-100). Zero value hides progress bar.
 */
void NmAttachmentListWidget::setProgressBarValue(int index, int value)
{
    if(index >= 0 && index < mItemList.count()){
        mItemList[index]->setProgressBarValue(value);
    }
}

/*!
    Helper function for initialize object.
 */
void NmAttachmentListWidget::init( )
{
    //Get mainwindow for orientation changes
    HbMainWindow *mw = hbInstance->allMainWindows().at(0);

    //connect to mainwindow signal
    if(mw){
        connect(mw, SIGNAL(orientationChanged(Qt::Orientation)),this, SLOT(orientationChanged(Qt::Orientation)));
        mOrientation = mw->orientation();
    } else {
        NMLOG("NmAttachmentListWidget::init: mainWindow missing!");
    }

    //construct UI after orientation has been figured out
    constructUi();

    //set default values, needed?
    setFlag(QGraphicsItem::ItemIsFocusable);      
}

/*!
    Helper function for constructing UI components.
 */
void NmAttachmentListWidget::constructUi()
{
    setObjectName(QString(ATTACHMENT_WIDGET));
    HbDocumentLoader loader;
    bool loadingOk = false;

    //Pass this widget to documentloader
    QObjectList objectList;
    objectList.append(this);
    loader.setObjectTree(objectList);
    QObjectList widgetlist = loader.load(FILE_PATH_DOCML, &loadingOk);

    int widgetCount = widgetlist.count();
    if(loadingOk && widgetCount){
        if(layout()){
            mLayout = dynamic_cast<QGraphicsGridLayout*>(layout());
        } else {
            NMLOG("NmAttachmentListWidget::constructUi: Widget doesn't have layout!");
        }
    } else {
        NMLOG("NmAttachmentListWidget::constructUi: DocML loading failed.");
    }

}

/*!
    Helper slot for handling longpressed signals.
*/
void NmAttachmentListWidget::handleLongPressed(QPointF point)
{
    QObject *sender = QObject::sender();
    int index = findItem(sender);
    if(NmNotFoundError != index){
        emit longPressed(index, point);
    }
    else {
        NMLOG("NmAttachmentListWidget::handleLongPressed: item cannot found!");
    }

}

/*!
    Helper slot for handling activated signals.
*/
void NmAttachmentListWidget::handleItemActivated()
{
    QObject *sender = QObject::sender();
    int index = findItem(sender);
    if(NmNotFoundError != index){
        emit itemActivated(index);
    }
    else {
        NMLOG("NmAttachmentListWidget::handleItemActivated: item cannot found!");
    }
}


/*!
    Helper slot for handling orientation change.
*/
void NmAttachmentListWidget::orientationChanged(Qt::Orientation orientation)
{
    NMLOG("NmAttachmentListWidget::orientationChanged");

    //be sure that orientation has been changed
    if(mOrientation != orientation){
        mOrientation = orientation;
        rearrangeLayout();
    }
}


/*!
    Helper function for finding attachment list items from the array.
    \param QObject searched item.
    \return int as index
*/
int NmAttachmentListWidget::findItem(const QObject *obj)
{
    int found(NmNotFoundError);
    int index(0);

    QListIterator<NmAttachmentListItem*> iter(mItemList);
    while(iter.hasNext() && found == NmNotFoundError){
        if(iter.next() == obj){
            found = index;
        }
        index++;
    }

    return found;
}

/*!
    Helper function for calculating items correct place on layout
    \param NmAttachmentListItem item to add to the layout
*/
void NmAttachmentListWidget::insertItemToLayout(NmAttachmentListItem* item)
{
    if(!mLayout) {
        NMLOG("NmAttachmentListWidget::insertItemToLayout: Layout loading failed!");
        return;
    }
    int layout_count = mLayout->count();

    //check which orientation in use
    //if layout is horizontal, items need to be placed in 2 columns.
    if(Qt::Vertical == mOrientation){
        mLayout->addItem(item,layout_count,0);
    } else {
        mLayout->addItem(item,layout_count / 2, layout_count % 2);
    }

}

/*!
    Helper function for rearrange layout after removing or layout change
*/
void NmAttachmentListWidget::rearrangeLayout()
{
    if(!mLayout) {
        NMLOG("NmAttachmentListWidget::rearrangeLayout: Layout loading failed!");
        return;
    }

    //remove all items from the layout
    int count(mLayout->count());
    for(int i = count - 1; i >= 0; --i){
        mLayout->removeAt(i);
    }

    //then add them back
    foreach(NmAttachmentListItem *item, mItemList){
        insertItemToLayout(item);
    }
}
