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
* Description: Attachment list item widget
*
*/

#include "nmailuiwidgetsheaders.h"

static const QString FILE_PATH_WIDGETML = ":nmattachmentlistitem.widgetml";
static const QString FILE_PATH_CSS = ":nmattachmentlistitem.css";

static const int PROGRESSBAR_MIN = 0; 
static const int PROGRESSBAR_MAX = 100;
static const int PROGRESSBAR_HIDE_COUNTDOWN = 5000;
static const int LONGPRESS_TIMER = 2000;


/*!
 @nmailuiwidgets
 \class NmAttachmentListItem

 \brief The NmAttachmentListItem widget provides for showing a single attachment with file size and
        download progress in the attachment list.



 */


/*!
    Constructs a new NmAttachmentListItem with \a parent.
 */
NmAttachmentListItem::NmAttachmentListItem(QGraphicsItem *parent)
    : HbWidget( parent ),
      mFileNameText(NULL),
      mFileSizeText(NULL),
      mProgressBar(NULL),
      mTimer(NULL),
      mButtonPressed(false),
      mLongPressedPoint(0,0)
{
    init( );
}


/*!
    Destructor.
 */
NmAttachmentListItem::~NmAttachmentListItem( )
{
    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_CSS);
}

/*!
    Set the texts to be displayed in the file name and file size items
    
 */
void NmAttachmentListItem::setTextItems(const QString &fileName, const QString &fileSize)
{
    mFileNameText->setText(fileName);
    mFileSizeText->setText(fileSize);    
}


/*!
    Set the download progress bar value (0-100)%, if value is 0 progress bar is hidden
 */
void NmAttachmentListItem::setProgressBarValue(const int value)
{
    //first check if value is 0 or below -> hide progressbar
    if ( 0 >= value ){
        removeProgressBar();
        return;
    }

    if ( !mProgressBar ){
        mProgressBar = new HbProgressBar(HbProgressBar::SimpleProgressBar, this); 
        mProgressBar->setObjectName("attachmentlistitem_progress");
        mProgressBar->setRange(PROGRESSBAR_MIN,PROGRESSBAR_MAX);
        HbStyle::setItemName( mProgressBar, "progressbar" );
        repolish();
    }
    mProgressBar->setProgressValue(value);
    
    //start hiding count down
    if(PROGRESSBAR_MAX <= value){
        QTimer::singleShot(PROGRESSBAR_HIDE_COUNTDOWN,this, SLOT(removeProgressBar()));
    }
}

/*!
    Get the download progress bar value
*/
int NmAttachmentListItem::progressBarValue() const
{
    int ret = 0;
    if ( mProgressBar ){
        ret = mProgressBar->progressValue();
    }

    return ret;
}

/*!
    Initialize
*/
void NmAttachmentListItem::init( )
{
    constructUi();

    //set default values
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    //set temporary longpress timer
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(longPressedActivated()));
}

/*!
    Constructs the UI, sets style itemnames etc.
*/
void NmAttachmentListItem::constructUi()
{
    //construct default ui.    
    HbStyleLoader::registerFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::registerFilePath(FILE_PATH_CSS);
    
    mFileNameText = new HbTextItem(this); 
    mFileNameText->setObjectName("nmattachmentlistitem_filenametext");
    HbStyle::setItemName( mFileNameText, "filename" );    

    mFileSizeText = new HbTextItem(this); 
    mFileSizeText->setObjectName("nmattachmentlistitem_filenamesize");
    HbStyle::setItemName( mFileSizeText, "filesize" );
    mFileSizeText->setElideMode(Qt::ElideNone);
        
}


/*!
    \reimp
 */
void NmAttachmentListItem::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    NMLOG("NmAttachmentListItem::mousePressEvent");

    mButtonPressed = true;
    mLongPressedPoint = event->scenePos();
    mTimer->start(LONGPRESS_TIMER);
}

/*!
    \reimp
 */
void NmAttachmentListItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    NMLOG("NmAttachmentListItem::mouseReleasedEvent");
    if ( mButtonPressed ){
        emit itemActivated();
        mButtonPressed = false;
        mTimer->stop();
    }
}

/*!
    Hides the download progress bar
 */
void NmAttachmentListItem::removeProgressBar()
{
	if ( mProgressBar ){
	    HbStyle::setItemName( mProgressBar, "" );
	    mProgressBar->deleteLater();
	    mProgressBar = 0;
	    repolish();
	}
}

/*!

 */
void NmAttachmentListItem::longPressedActivated()
{
    //check first if button is not released already
    if ( mButtonPressed ){
        NMLOG("NmAttachmentListItem::longPressedActivated");
        emit itemLongPressed(mLongPressedPoint);
        mButtonPressed = false;
        mTimer->stop();
    }
}

