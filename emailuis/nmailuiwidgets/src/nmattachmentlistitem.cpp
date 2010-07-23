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
      mProgressBar(NULL)
{
    NM_FUNCTION;
    
    init( );
	
    // Informs GestureFramework that NmAttachmentListItem widget is interested 
    // Tap gesture and TapAndHold gesture.
    grabGesture(Qt::TapGesture);
}

/*!
    Setter for items text color override. This fucntion can be used
    if theme background is not used and text needs to be shown in diferent color.
 */
void NmAttachmentListItem::setTextColor(const QColor color)
{
    NM_FUNCTION;
    
    mTextColor=color;
    if (mTextColor.isValid() && mFileNameText){
        mFileNameText->setTextColor(mTextColor);
    }    
}

/*!
    Destructor.
 */
NmAttachmentListItem::~NmAttachmentListItem( )
{
    NM_FUNCTION;
    
    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::unregisterFilePath(FILE_PATH_CSS);
}

/*!
    Set the text to be displayed in the file name item.
 */
void NmAttachmentListItem::setFileNameText(const QString &fileName)
{
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
    if (mFileSizeText){
        if (mTextColor.isValid()){
            mFileSizeText->setTextColor(mTextColor);
        }
        mFileSizeText->setTextWrapping(Hb::TextNoWrap);
        mFileSizeText->setText(fileSize);
    }
}

/*!
    Set the download progress bar value (0-100)%, if value is 0 progress bar is hidden
 */
void NmAttachmentListItem::setProgressBarValue(const int value)
{
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
    QTimer::singleShot(PROGRESSBAR_HIDE_COUNTDOWN,this, SLOT(removeProgressBar()));
}

/*!
    Initialize
*/
void NmAttachmentListItem::init( )
{
    NM_FUNCTION;
    
    constructUi();

    //set default values
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

/*!
    Constructs the UI, sets style itemnames etc.
*/
void NmAttachmentListItem::constructUi()
{
    NM_FUNCTION;
    
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
    Hides the download progress bar
 */
void NmAttachmentListItem::removeProgressBar()
{
    NM_FUNCTION;
    
	if ( mProgressBar ){
	    HbStyle::setItemName( mProgressBar, "" );
	    mProgressBar->deleteLater();
	    mProgressBar = 0;
	    repolish();
	}
}


/*!
    This function handles gestures
 */
void NmAttachmentListItem::gestureEvent(QGestureEvent *event)
{
    NM_FUNCTION;
    
    if (HbTapGesture *tap = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) {
        switch(tap->tapStyleHint()) {
        case HbTapGesture::Tap:
            {
                if (tap->state() == Qt::GestureFinished) {
                    emit itemActivated();
                }
             }
             break;
            
         case HbTapGesture::TapAndHold:
             {
                 if (tap->state() == Qt::GestureFinished) {
                 emit itemLongPressed(event->mapToGraphicsScene(tap->position()));
                 }
             }    
             break;
        }
    }
    else {
           HbWidget::gestureEvent(event);
    }
}


