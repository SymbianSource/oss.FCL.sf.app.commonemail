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
#include <msvapi.h>
#include <msvids.h>

#include <QTimer>
#include <QServiceManager>
#include <QServiceFilter>
#include <QServiceInterfaceDescriptor>

#include <HbMessageBox>
#include <HbProgressDialog>
#include <HbAction>

#include "nmipssettingsdeletemailboxop.h"
#include "nmipssettingsmanagerbase.h"

/// CONSTANTS

/// How many times the delete is attempted before giving up.
const int KDeleteRetryCount = 3;
/// How long will the operation wait (ms) for the disconnect event to arrive before timing out
const int KDisconnectTimeout = 2500;
/// How long will the operation wait (ms) before attempting the delete again if error happened.
const int KRetryDelay = 250;


/*!
    \class CMailboxDisconnectWait
    \brief Symbian helper class for mailbox delete operation. Used for listening disconnect events
           from messaging server.

*/
class CMailboxDisconnectWait : public CBase, public MMsvSessionObserver
{
public:
    static CMailboxDisconnectWait* NewL(
        NmIpsSettingsDeleteMailboxOp& aDeleteOp, const TMsvId& aMailboxId );
    ~CMailboxDisconnectWait();

    TBool IsConnected();
    
public: // from MMsvSessionObserver
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

private:
    CMailboxDisconnectWait( NmIpsSettingsDeleteMailboxOp& aDeleteOp, const TMsvId& aMailboxId );

private: // data
    /// Own. Messaging server session
    CMsvSession* iMsvSession;
    /// Mailbox ID that is observed
    TMsvId iMailboxId;
    /// Delete operation
    NmIpsSettingsDeleteMailboxOp& iDeleteOp;
};

/*!
    Constructor of CMailboxDisconnectWait.
*/
CMailboxDisconnectWait* CMailboxDisconnectWait::NewL( 
    NmIpsSettingsDeleteMailboxOp& aDeleteOp,
    const TMsvId& aMailboxId )
{
    CMailboxDisconnectWait* self = new(ELeave) CMailboxDisconnectWait( aDeleteOp, aMailboxId );
    CleanupStack::PushL( self );
    self->iMsvSession = CMsvSession::OpenSyncL( *self );
    CleanupStack::Pop( self );
    return self;
}

/*!
    Constructor of CMailboxDisconnectWait.
*/
CMailboxDisconnectWait::CMailboxDisconnectWait( 
    NmIpsSettingsDeleteMailboxOp& aDeleteOp, const TMsvId& aMailboxId ) : 
    iMailboxId( aMailboxId ),
    iDeleteOp( aDeleteOp )
{
}

/*!
    Destructor of CMailboxDisconnectWait.
*/
CMailboxDisconnectWait::~CMailboxDisconnectWait()
{
    delete iMsvSession;
}

/*!
    Check if observed mailbox is connected or not
*/
TBool CMailboxDisconnectWait::IsConnected()
{
    TBool connected = EFalse;
    TMsvEntry entry;
    TMsvId id;
    if( iMsvSession->GetEntry( iMailboxId, id, entry ) == KErrNone )
        {
        connected = entry.Connected();
        }
    return connected;
}

/*!
    Event handler of Messaging server events
*/
void CMailboxDisconnectWait::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
{
    // Entry changed event is received when online/offline state has changed
    if( EMsvEntriesChanged == aEvent )
        {
        // mailboxes are only on root level, so check the parent
        TMsvId parent = *static_cast<TMsvId*>(aArg2);
        if( parent == KMsvRootIndexEntryId )
            {
            // find the mailbox we're observing
            CMsvEntrySelection* selection = static_cast<CMsvEntrySelection*>(aArg1);
            TInt count = selection->Count();
            for( TInt i = 0; i < count; ++i )
                {
                if( (*selection)[i] == iMailboxId )
                    {
                    // if mailbox is not connected -> report to observer
                    TMsvEntry entry;
                    TMsvId id;
                    TInt err = iMsvSession->GetEntry( iMailboxId, id, entry );
                    if( !err && !entry.Connected() )
                        {
                        TInt err; // error code ignored
                        QT_TRYCATCH_ERROR( err, iDeleteOp.mailboxDisconnected() );
                        Q_UNUSED(err); // remove compile warning
                        }
                    }
                }
            }
        }
}


/*!
    \class NmIpsSettingsDeleteMailboxOp
    \brief Operation that handles the deletion of mailbox

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsDeleteMailboxOp.
*/
NmIpsSettingsDeleteMailboxOp::NmIpsSettingsDeleteMailboxOp(NmIpsSettingsManagerBase &settingsManager) :
    mDisconnectWait(NULL),
    mSettingsManager(settingsManager),
    mDeleteConfirmationDlg(NULL),
    mDeleteProgressDlg(NULL),
    mRetryCount(KDeleteRetryCount),
    mError(0)
{
    connect( this, SIGNAL(stateChanged()), this, SLOT(processState()), Qt::QueuedConnection );
}

/*!
    Destructor of NmIpsSettingsDeleteMailboxOp.
*/
NmIpsSettingsDeleteMailboxOp::~NmIpsSettingsDeleteMailboxOp()
{
    delete mDeleteConfirmationDlg;
    delete mDeleteProgressDlg;
    delete mDisconnectWait;
}

/*!
    Starts the operation
*/
void NmIpsSettingsDeleteMailboxOp::startOperation()
{
    // reset state
    mRetryCount = KDeleteRetryCount;
    mResult = DeleteMbResultSuccess;
    mError = 0;
    
    delete mDisconnectWait;
    mDisconnectWait = NULL;
    TRAPD( err, mDisconnectWait = CMailboxDisconnectWait::NewL( 
        *this, mSettingsManager.mailboxId().id32() ) );

    if(!err) {
        changeState(StateConfirmation);
    } else {
        // failed
        complete(DeleteMbResultFailure, err);
    }
}

/*!
    Observer method to get notifications when the handled mailbox has disconnected
*/
void NmIpsSettingsDeleteMailboxOp::mailboxDisconnected()
{
    if( mState == StateDisconnect )
        {
        changeState(StateDelete);
        }
}

/*!
    Notification method that is called when user has responded to confirmation query
*/
void NmIpsSettingsDeleteMailboxOp::handleConfimationDlg(HbAction *action)
{
    if (action == mDeleteConfirmationDlg->actions().at(0)) {
        changeState(StateDisconnect);
    } else {
        complete(DeleteMbResultCanceled, 0);
    }
}

/*!
    Notification method that is called when timeout has occured while waiting for disconnect event
*/
void NmIpsSettingsDeleteMailboxOp::handleDisconnectTimeout()
{
    if( mState == StateDisconnect )
        {
        changeState(StateDelete);
        }
}

/*!
    Operation's state machine. Handles states.
*/
void NmIpsSettingsDeleteMailboxOp::processState()
{
    switch( mState ) {
    case StateConfirmation: {
        // Ask user whether to delete or not
        if(!mDeleteConfirmationDlg) {
            mDeleteConfirmationDlg =
                new HbMessageBox(HbMessageBox::MessageTypeQuestion);
            mDeleteConfirmationDlg->setText(
                hbTrId("txt_mail_dialog_do_you_want_to_delete_the_mailbox"));
            mDeleteConfirmationDlg->setTimeout(HbMessageBox::NoTimeout);
            mDeleteConfirmationDlg->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
        }
        mDeleteConfirmationDlg->open(this, SLOT(handleConfimationDlg(HbAction *)));
        break;
    }
    
    case StateDisconnect: {
        // Disconnect mailbox
        emit goOffline(mSettingsManager.mailboxId());

        // Display the progress note during rest of the operation
        if( !mDeleteProgressDlg ) {
            mDeleteProgressDlg = new HbProgressDialog(HbProgressDialog::WaitDialog);
            mDeleteProgressDlg->removeAction( mDeleteProgressDlg->actions().at(0));
            mDeleteProgressDlg->setText(hbTrId("txt_common_info_deleting"));
        }
        mDeleteProgressDlg->delayedShow();

        if( mDisconnectWait->IsConnected() ) {
            // connected -> wait for disconnect or timeout
            QTimer::singleShot(KDisconnectTimeout, this, SLOT(handleDisconnectTimeout()));
        } else {
            // not connected -> delete mailbox immediately
            changeState( StateDelete );
        }
        break;
    }
    
    case StateDelete: {
        // delete mailbox
        int error = mSettingsManager.deleteMailbox();
        if( !error ) {
            changeState(StateUnregisterMailbox);

        } else if( mRetryCount-- ) {
            // attempt again after small delay
            QTimer::singleShot(KRetryDelay, this, SLOT(processState()));
        
        } else {
            // enough failures. give up
            complete(DeleteMbResultFailure, error);
        }
        break;
    }
    
    case StateUnregisterMailbox: {
        // Unregister mailbox from application library
        QtMobility::QServiceManager manager;
        QtMobility::QServiceFilter filter("com.nokia.symbian.IEmailRegisterAccount");
        QList<QtMobility::QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);
        QObject *registerInterface = NULL;
        if (!interfaces.isEmpty()) {
            registerInterface = manager.loadInterface(interfaces.first());
        }

        if (registerInterface) {
            quint64 mailboxId(mSettingsManager.mailboxId().id());
            // Try to remove the mailbox from the App Library.
            (void)QMetaObject::invokeMethod(registerInterface,
                "unregisterMailbox", Q_ARG(quint64, mailboxId));
            }
        
        // All done -> complete
        complete(DeleteMbResultSuccess, 0);
        break;
    }
    
    case StateDone: // fall through
    default: {
        // Hide the progress note
        if( mDeleteProgressDlg ) {
            mDeleteProgressDlg->close();
        }
        emit operationComplete(mResult, mError);
        break;
    }
    }
}

/*!
    Changes the operation state
*/
void NmIpsSettingsDeleteMailboxOp::changeState(DeleteOpStates nextState)
{
    mState = nextState;
    emit stateChanged();
}

/*!
    Completes the operation by setting the result values and changing the state to [done]
*/
void NmIpsSettingsDeleteMailboxOp::complete(int result, int error)
{
    mError = error;
    mResult = result;
    mState = StateDone;
    emit stateChanged();
}


