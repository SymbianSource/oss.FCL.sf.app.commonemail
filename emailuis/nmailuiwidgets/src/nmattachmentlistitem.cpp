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
static const int PROGRESSBAR_HIDE_COUNTDOWN = 500;
static const int LONGPRESS_TIMER = 2000;

// Hardcoded file size length. Maximum (999.9 Mb) fits into size field.
static const int FILE_SIZE_FIELD_LENGTH = 120;

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
    Setter for items text color override. This fucntion can be used
    if theme background is not used and text needs to be shown in diferent color.
 */
void NmAttachmentListItem::setTextColor(const QColor color)
{
    mTextColor=color;
}

/*!
    Destructor.
 */
NmAttachmentListItem::~NmAttachmentListItem( )
{
    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_CSS);
    
    delete mTimer;
    mTimer = NULL; 
}

/*!
    Set the text to be displayed in the file name item.
 */
void NmAttachmentListItem::setFileNameText(const QString &fileName)
{
    if (mFileNameText){
        if (mTextColor.isValid()){
            mFileNameText->setTextColor(mTextColor);
        }
        mFileNameText->setTextWrapping(Hb::TextNoWrap);
        mFileNameText->setText(fileName);    
    }
}

/*!
    Set the text to be displayed in the file size item
 */
void NmAttachmentListItem::setFileSizeText(const QString &fileSize)
{
    if (mFileSizeText){
        if (mTextColor.isValid()){
            mFileSizeText->setTextColor(mTextColor);
        }
        mFileSizeText->setTextWrapping(Hb::TextNoWrap);
        mFileSizeText->setText(fileSize);
    }
}

/*!
    Set the length of the filename field.
 */
void NmAttachmentListItem::resetFileNameLength(Qt::Orientation orientation)
{
	QSizeF reso = screenSize(orientation);
	
	if (orientation == Qt::Horizontal) {
        mFileNameText->setPreferredWidth(reso.width() / 2 - FILE_SIZE_FIELD_LENGTH);
	}
	else {		
        mFileNameText->setPreferredWidth(reso.width() - FILE_SIZE_FIELD_LENGTH);
	}
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
        mProgressBar = new HbProgressBar(this); 
        mProgressBar->setObjectName("attachmentlistitem_progress");
        mProgressBar->setRange(PROGRESSBAR_MIN,PROGRESSBAR_MAX);
        HbStyle::setItemName( mProgressBar, "progressbar" );
        repolish();
    }
    mProgressBar->setProgressValue(value);
    
    //start hiding count down
    if(PROGRESSBAR_MAX <= value){
        hideProgressBar();
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
    Hides progress bar, used if download is cancelled before 100 precent is reached
*/
void NmAttachmentListItem::hideProgressBar()
{
    QTimer::singleShot(PROGRESSBAR_HIDE_COUNTDOWN,this, SLOT(removeProgressBar()));
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
    mTimer->setSingleShot(true);
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
    mFileNameText->setElideMode(Qt::ElideRight);

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
    if(mTimer){
        mTimer->start(LONGPRESS_TIMER);        
    }
}

/*!
    \reimp
 */
void NmAttachmentListItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    NMLOG("NmAttachmentListItem::mouseReleasedEvent");
    if ( mTimer && mButtonPressed ){
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
    }
}

/*!
    This function returns screen size depending on the orientation.
    Function is copied from NmApplication.
 */
QSize NmAttachmentListItem::screenSize(Qt::Orientation orientation)
{
    QSize ret(0,0);
    HbDeviceProfile currentP = HbDeviceProfile::current();
    HbDeviceProfile altP(currentP.alternateProfileName());
    QSize curPSize = currentP.logicalSize();
    QSize altPSize = altP.logicalSize();
    if (orientation == Qt::Horizontal) {
        // Get wide profile size in landscape
        if (curPSize.width() > altPSize.width()) {
            ret = curPSize;
        }
        else{
            ret = altPSize;
        }
    }
    else {
        // Get narrow profile size in portrait
        if (curPSize.width() < altPSize.width()) {
            ret = curPSize;
        }
        else{
            ret = altPSize;
        }
    }
    return ret;
}

