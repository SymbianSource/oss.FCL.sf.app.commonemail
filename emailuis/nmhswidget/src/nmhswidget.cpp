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
 * Description: 
 *
 */
#include <QDebug>
#include <QtGui>
#include <QGraphicsLinearLayout>
#include <hbcolorscheme.h>
#include <QTranslator>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include "nmcommon.h"
#include "nmhswidget.h"
#include "nmhswidgetemailengine.h"
#include "nmmessageenvelope.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetemailrow.h"
#include "nmhswidgetconsts.h"
#include "nmhswidgetdatetimeobserver.h"

NmHsWidget::NmHsWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags), 
      mEngine(0),
      mRowLayout(0),
      mTitleRow(0),
      mAccountId(0),
      mAccountIconName(0),
      mTranslator(0),
      mBackgroundFrameDrawer(0),
      mIsExpanded(true),
      mStaticWidget(true),
      mDateObserver(0)
{
    qDebug() << "NmHsWidget::NmHsWidget IN -->>";
       
    qDebug() << "NmHsWidget::NmHsWidget OUT <<--";
}

/*!
    Destructor
*/
NmHsWidget::~NmHsWidget()
{
    qDebug() << "NmHsWidget::~NmHsWidget IN -->>";
    if(mTranslator){
        delete mTranslator;
        mTranslator = NULL;
    }
    
    if(mEngine){
        delete mEngine;
        mEngine = NULL;
        }
    
    if(mBackgroundFrameDrawer){
        delete mBackgroundFrameDrawer;
        mBackgroundFrameDrawer = NULL;
        }
		
	if(mDateObserver){
        delete mDateObserver;
        mDateObserver = NULL;
        }
    qDebug() << "NmHsWidget::~NmHsWidget OUT <<--";
}

/*!
    \fn void NmHsWidget::onShow()

    called by home screen fw when widget gets visible
*/
void NmHsWidget::onShow()
{
    qDebug() << "NmHsWidget::onShow IN -->>";
    if (mEngine)
        {
        mEngine->activate();
        }
    qDebug() << "NmHsWidget::onShow OUT <<--";
}

/*!
    \fn void NmHsWidget::onHide()

    called by home screen fw when widget gets hidden
*/
void NmHsWidget::onHide()
{
    qDebug() << "NmHsWidget::onHide IN -->>";
    if (mEngine)
        {
        mEngine->suspend();
        }
    qDebug() << "NmHsWidget::onHide OUT <<--";
}

/*!
    Initializes Localization.
    /post mTranslator constructed & localization file loaded
    returns false in failure, otherwise true
*/
bool NmHsWidget::setupLocalization()
{
    qDebug() << "NmHsWidget::setupLocalization IN -->>";
    
    //Use correct localisation
    bool ret(false); 
    mTranslator = new QTranslator();
    QString lang = QLocale::system().name();
    ret = mTranslator->load(KNmHsWidgetLocFileName + lang, KNmHsWidgetLocLocation);
    qDebug() << "NmHsWidget::setupLocalization mTranslator->load loadSucceed:"<<ret;
    QCoreApplication::installTranslator(mTranslator);

    qDebug() << "NmHsWidget::setupLocalization OUT <<--";
    return ret;
}

/*!
    Initializes UI. Everything that is not done in docml files should be here.
    return true if ok, in error false.
*/
void NmHsWidget::setupUi()
{
    qDebug() << "NmHsWidget::setupUi IN -->>";
    setContentsMargins( KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    
    //Setup layout
    mRowLayout = new QGraphicsLinearLayout(Qt::Vertical);

    mRowLayout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    mRowLayout->setSpacing(KNmHsWidgetContentsMargin);
    setLayout(mRowLayout);
   
   //background
   mBackgroundFrameDrawer = new HbFrameDrawer( KNmHsWidgetBackgroundImage, HbFrameDrawer::NinePieces );
   HbFrameItem* backgroundLayoutItem = new HbFrameItem( mBackgroundFrameDrawer );
   //set to NULL to indicate that ownership transferred
   mBackgroundFrameDrawer = NULL;
   setBackgroundItem( backgroundLayoutItem );
   
   qDebug() << "NmHsWidget::setupUi OUT -->>";
}

/*!
    Initializes the widget.
    
    called by home screen fw when widget is added to home screen
*/
void NmHsWidget::onInitialize()
{
    QT_TRY{
        qDebug() << "NmHsWidget::onInitialize IN -->>";
        
        setupUi();
        //emit error if localization fails
        if(!setupLocalization()){
            emit error();
            return;
        }
        
        //Engine construction is 2 phased. 
        mEngine = new NmHsWidgetEmailEngine( mAccountId );
        //Client must connect to exception signals before calling the initialize function
        //because we don't want to miss any signals.
        connect(mEngine, SIGNAL( exceptionOccured(const int&) )
                ,this, SLOT( onEngineException(const int&) ) );
        if(!mEngine->initialize())
            {
            //engine construction failed. Give up.
            emit error();
            return;
            }

        //construct and load docml for title row
        mTitleRow = new NmHsWidgetTitleRow(); 
        if( !mTitleRow->loadDocML()){
            //if docml loading fails no point to proceed
            //but memoryleak must be prevented
            delete mTitleRow;
            mTitleRow = NULL;
            emit error();
            return;
        }
        mRowLayout->addItem(mTitleRow);
        mTitleRow->updateAccountName(mEngine->accountName());
    
	    //create observer for date/time change events
	    mDateObserver = new NmHsWidgetDateTimeObserver();
    
        updateMailData();
        mTitleRow->updateUnreadCount(mEngine->unreadCount());
        mTitleRow->setAccountIcon(mAccountIconName);
        
        //Get signals about changes in mail data
        connect(mEngine, SIGNAL( mailDataChanged() )
                ,this, SLOT( updateMailData() ) );
        
        //Get Signals about changes in unread count
        connect(mEngine, SIGNAL( unreadCountChanged(const int&) )
                ,mTitleRow, SLOT( updateUnreadCount(const int&) ) );
        
        //Get signals about account name changes
        connect(mEngine, SIGNAL( accountNameChanged(const QString&) )
                ,mTitleRow, SLOT( updateAccountName(const QString&) ) );

	    //Get signals about user actions
	    connect(mTitleRow, SIGNAL( mailboxLaunchTriggered() )
	            ,mEngine, SLOT( launchMailAppInboxView() ) );
	    connect(mTitleRow, SIGNAL( expandCollapseButtonPressed() )
	            ,this, SLOT( handleExpandCollapseEvent() ) );

	    //resize here so homescreen will place widget correctly on screen
	    setPreferredSize( mRowLayout->preferredSize() );
	    if (parentWidget()) {
	        //to place widget properly after adding to homescreen
	        parentWidget()->resize(preferredSize()); 
		}
        qDebug() << "NmHsWidget::onInitialize OUT <<--";  
    }
    QT_CATCH(...){
        emit error();
    }

}

/*!
    Uninitializes the widget.
    
    called by home screen fw when widget is removed from home screen
*/
void NmHsWidget::onUninitialize()
{
    qDebug() << "NmHsWidget::onUninitialize IN -->>";
    
    qDebug() << "NmHsWidget::onUninitialize OUT <<--";
}

/*!
    updateMailData slot
*/
void NmHsWidget::updateMailData()
{
    qDebug() << "NmHsWidget::updateData IN -->>";
    QList<NmMessageEnvelope> envelopes;
    int count = 0;
    if (mIsExpanded) {
        count = mEngine->getEnvelopes(envelopes, KMaxNumberOfMailsShown);
        }

    updateMailRowsList(count);
    
    for(int i=0; i<envelopes.count(); i++)
        {
        mMailRows[i]->updateMailData( envelopes[i] );
        }
    qDebug() << "NmHsWidget::updateData OUT <<--"; 
}

/*!
    Sets monitored account id from given string
    Needed for home screen framework which supports only QString type properties
*/
void NmHsWidget::setAccountId(const QString &text)
{
    qDebug() << "NmHsWidget::setAccountId IN -->>"; 
    bool ok;
    quint64 id = text.toULongLong(&ok);
    if (!ok)
        {
        qDebug() << "NmHsWidget::setAccountId: invalid account ID data, signal finished()!!!"; 
        //No valid account id so give up
        emit finished();
        }
    else
        {
        mAccountId.setId(id);
        }
    qDebug() << "NmHsWidget::setAccountId OUT <<--"; 
}

/*!
    Returns monitored account id as a string
    Needed for home screen framework which supports only QString type properties
*/
QString NmHsWidget::accountId() const
{
    qDebug() << "NmHsWidget::accountId()"; 
    return QString::number(mAccountId.id());
}

/*!
    Sets monitored account icon name from given string
*/
void NmHsWidget::setAccountIconName(const QString &text)
{
    qDebug() << "NmHsWidget::setAccountIconName IN -->>"; 
    mAccountIconName = text;
    qDebug() << "NmHsWidget::setAccountIconName OUT <<--"; 
}

/*!
    Returns monitored account icon name
*/
QString NmHsWidget::accountIconName() const
{
    qDebug() << "NmHsWidget::accountIconName()"; 
    return mAccountIconName;
}

/*!
    Slot to handle expand/collapse trigger event
*/
void NmHsWidget::handleExpandCollapseEvent()
{
    qDebug() << "NmHsWidget::handleExpandCollapseEvent IN -->>";
    toggleExpansionState();
    qDebug() << "NmHsWidget::handleExpandCollapseEvent OUT <<--"; 
}

/*!
    Sets widget expand/collapse state
    /post widget expansion state is changed
*/
void NmHsWidget::toggleExpansionState()
{
    qDebug() << "NmHsWidget::setExpanded IN -->>"; 

    mIsExpanded = !mIsExpanded;
    
    //save new state to home screen
    QStringList propertiesList;
    propertiesList.append("widgetState");
    emit setPreferences(propertiesList);
    
    //handle state change drawing
    updateMailData();
    
    qDebug() << "NmHsWidget::setExpanded OUT <<--"; 
}

/*!
    Sets expand/collapse state from given string (needed by homescreen)
*/
void NmHsWidget::setWidgetStateProperty(QString value)
{
    qDebug() << "NmHsWidget::setWidgetStateProperty IN -->>"; 
    if (value == KNmHsWidgetStateCollapsed)
        {
        mIsExpanded = false;
        }
    else
        {
        mIsExpanded = true;
        }
    qDebug() << "NmHsWidget::setWidgetStateProperty OUT <<--"; 
}

/*!
    Returns widget expand/collapse state as string (needed by homescreen) 
*/
QString NmHsWidget::widgetStateProperty()
{
    qDebug() << "NmHsWidget::widgetStateProperty()";
    if (mIsExpanded)
        {
        return KNmHsWidgetStateExpanded;
        }
    else
        {
        return KNmHsWidgetStateCollapsed;
        }
}

/*!
    Updates mMailRows list to include correct amount of mail row widgets
    /param mailCount defines how many mail rows is needed
    /post mMailRows list includes NmHsWidgetEmailRow for each mail item 
*/
void NmHsWidget::updateMailRowsList(const int mailCount)
{
    qDebug() << "NmHsWidget::updateMailRowsList IN -->>";
    qDebug() << "NmHsWidget - mMailRows.count() == " <<  mMailRows.count();
    qDebug() << "NmHsWidget - ordered count == " <<  mailCount;
    
    int neededRowsCount = mailCount;
    //force size when static and expanded
    if (mStaticWidget && mIsExpanded)
        {
        neededRowsCount = KMaxNumberOfMailsShown;
        }
    
    while (mMailRows.count() != neededRowsCount)
        {
        //more mails to show than rows
        if (mMailRows.count() < neededRowsCount)
            {
            qDebug() << "NmHsWidget - add new mail row";
            NmHsWidgetEmailRow *row = new NmHsWidgetEmailRow();
            if( !row->loadDocML()){
                qDebug() << "NmHsWidget::updateMailRowsList row->loadDocML() fails";
                //if docml loading fails no point to proceed
                //but memoryleak must be prevented
                delete row;
                row = NULL;
                emit error();
                return;
            }
            connect(row, SIGNAL(mailViewerLaunchTriggered(const NmId&))
                    ,mEngine, SLOT(launchMailAppMailViewer(const NmId&)));
            connect( mDateObserver, SIGNAL(dateTimeChanged())
                    ,row, SLOT(updateDateTime()) );
            mMailRows.append(row);
            mRowLayout->addItem(row);            
            }
        //too many rows
        else if (mMailRows.count() > neededRowsCount)
            {
            qDebug() << "NmHsWidget - remove mail row";
            mRowLayout->removeItem(mMailRows.last());
            delete mMailRows.takeLast();
            }
        }
    __ASSERT_ALWAYS( mMailRows.count() == neededRowsCount, User::Panic(_L("Invalid"), 500) );
    
    //resize the widget to new layout size
    setPreferredSize( mRowLayout->preferredSize() );
    
    if (mStaticWidget)
        {
        this->updateMailRowsVisibility(mailCount);
        }
    qDebug() << "NmHsWidget::updateMailRowsList OUT <<--";
}

/*!
    Updates mail row visibilities in static widget
    /param visibleCount defines how many items do have mail data
    /post all row items having mail data are visible, other rows are hidden
*/
void NmHsWidget::updateMailRowsVisibility(const int visibleCount)
{
    qDebug() << "NmHsWidget::updateMailRowsVisibility IN -->>";
  
    // set visible as many rows as requested by visibleCount param
    bool isVisible;
    for (int i=0; i < mMailRows.count(); i++) 
        {
        isVisible = false;
        if ((mIsExpanded) && (i < visibleCount)) 
            {
            isVisible = true;
            }
        mMailRows.at(i)->setVisible(isVisible);
        }
    
    qDebug() << "NmHsWidget::updateMailRowsVisibility OUT <<--";
}

/*!
    onEngineException (NmHsWidgetEmailEngineExceptionCode exc)
    signals widget to be finalized
    /param exc exception type
*/
void NmHsWidget::onEngineException(const int& exc)
    {
    qDebug() << "NmHsWidget:onEngineException IN -->>";
    switch (exc)
        {
        case (NmEngineExcAccountDeleted):
             emit finished(); //succesful ending
             break;
        case (NmEngineExcFailure):
             emit error(); //failure
             break;
        default: 
            break; 
        }
    }
