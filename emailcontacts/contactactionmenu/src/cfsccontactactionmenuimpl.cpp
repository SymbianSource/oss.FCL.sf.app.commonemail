/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CFscContactActionMenuImpl.
*
*/


// INCUDES
#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include "cfsccontactactionservice.h"
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <avkon.hrh>
#include <AknsUtils.h>
#include <aknnotewrappers.h> 
#include <textresolver.h> 

#include "cfsccontactactionmenuimpl.h"
#include "fsccontactactionmenuuids.hrh"
#include "cfsccontactactionmenumodelimpl.h"
#include "tfsccontactactionmenuconstructparameters.h"
#include "cfsccontactactionmenulist.h"
#include "cfsccontactactionmenuitemimpl.h"
#include "cfsccontactactionmenucasitemimpl.h"

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFscContactActionMenuImplImpUid, 
                                CFscContactActionMenuImpl::NewL )
    };
      
    
// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuImpl* CFscContactActionMenuImpl::NewL(
    TAny* aParams )
    {
    FUNC_LOG;

    TFscContactActionMenuConstructParameters* params = 
        reinterpret_cast< TFscContactActionMenuConstructParameters* >( aParams );

    CFscContactActionMenuImpl* self = 
        new (ELeave) CFscContactActionMenuImpl( *params );
    CleanupStack::PushL( self );
    self->ConstructL( *params );
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::~CFscContactActionMenuImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuImpl::~CFscContactActionMenuImpl()
    {
    FUNC_LOG;
    delete iModel;
    delete iList;
    delete iWait;
    delete iTextResolver;
    delete iDummyMenuItem;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ExecuteL
// ---------------------------------------------------------------------------
//
TFscActionMenuResult CFscContactActionMenuImpl::ExecuteL( 
    TFscContactActionMenuPosition aPosition, TInt aIndex, MFsActionMenuPositionGiver* aPositionGiver)
    {
    FUNC_LOG;
          
    // Initialise some member variables
    iExit = EFalse;
    iMenuResult = EFscMenuDismissed;
    iError = KErrNone;
    
    // Check if menu is empty
    if ( iModel->VisibleItemCount() > 0 )
        {         
        iList = CFscContactActionMenuList::NewL( 
            this, iModel, ( iMode == EFscContactActionMenuModeAI ), aPositionGiver, iOpenedFromMR );
        // Execute menu
        TRAP( iError, DoExecuteL( aPosition, aIndex ) );

        // Delete iList
        iSelectedItemIndex = iList->SelectedItemIndex();
        delete iList;
        iList = NULL;

        User::LeaveIfError( iError );
        }

    return iMenuResult;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::Dismiss
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::Dismiss( TBool aSlide )
    {
    FUNC_LOG;
    iExit = !aSlide;
    iMenuResult = EFscMenuDismissed;
    // cancel action execution if running
    // needs to be canceled to close select popup dialog
    if ( iIsActionExecuting )
        {
        iModel->Service()->CancelExecute();
        iIsActionExecuting = EFalse;
        }
     
    DoDismiss();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::Model
// ---------------------------------------------------------------------------
//
MFscContactActionMenuModel& CFscContactActionMenuImpl::Model()
    {
    FUNC_LOG;
    return *iModel; 
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FocusedItemIndex
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuImpl::FocusedItemIndex()
    {
    FUNC_LOG;
    TInt result = iSelectedItemIndex;
    if ( iList != NULL )
        {
        result = iList->SelectedItemIndex();
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FocusedItem
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItem& CFscContactActionMenuImpl::FocusedItem()
    {
    FUNC_LOG;
    TInt i = FocusedItemIndex();
    TInt err = KErrNone;
    
    CFscContactActionMenuItem* focusedItem = NULL;
    
    TRAP( err, focusedItem = &(iModel->ItemL( i )) );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        focusedItem = iDummyMenuItem;
        }
    
    return *focusedItem;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::HandleListEvent
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleListEvent( 
    TFscContactActionMenuListEvent aEvent,
    TInt aError )
    {
    FUNC_LOG;
    switch ( aEvent )
        {
        case EFscMenuEventItemClicked:
            {
            HandleItemClick();
            break;
            }
        case EFscMenuEventError:
            {
            iError = aError;
            //break not needed because menu will be dismissed
            }
        case EFscMenuEventExitKey:
            {
            iExit = ETrue;
            //break not needed because menu will be dismissed
            }
        case EFscMenuEventCloseKey:
            {
            iMenuResult = EFscMenuDismissed;
            DoDismiss();
            break;
            }
        case EFscMenuEventMenuHidden:
            {
            StopWait();
            
            // Hide Cba and fade
            HideMenuCba();
            iFader.FadeBehindPopup( this, iList, EFalse );
            break;
            }
        case EFscMenuLayoutChanged:
            {
            Dismiss( iExit );
            break;
            }
        default:
            {
            // nothing to do here
            break;
            }    
        }
    } 

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::CountFadedComponents
// ---------------------------------------------------------------------------
//
TBool CFscContactActionMenuImpl::IsOperationCompleted()
    {
    FUNC_LOG;
    return isExecuteCompleted;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::SetOpenedFromMR
// ---------------------------------------------------------------------------
// 
void CFscContactActionMenuImpl::SetOpenedFromMR( TBool aOpenedFromMR )
    {
    iOpenedFromMR = aOpenedFromMR;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::CountFadedComponents
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuImpl::CountFadedComponents()
    {
    FUNC_LOG;
    TInt count = 1;
    if ( iList )
        {
        count++;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FadedComponent
// ---------------------------------------------------------------------------
//
CCoeControl* CFscContactActionMenuImpl::FadedComponent(TInt aIndex)
    {
    FUNC_LOG;
    CCoeControl* cntrl = NULL;
    switch (aIndex)
        {
        case 0:
            {
            cntrl = iButtonGroupContainer;
            break;
            }        
        case 1:
            {
            cntrl = iList;
            break;
            }
        default:
            {
            cntrl = NULL;
            break;
            }
        }
    return cntrl;    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FadedComponent
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleGainingForeground()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::FadedComponent
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleLosingForeground()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ProcessCommandL(TInt aCommandId)
    {
    FUNC_LOG;
    if ( isExecuteCompleted )
        {
        switch ( aCommandId )
            {
            case EAknSoftkeyCancel:
                {
                iMenuResult = EFscMenuDismissed;
                DoDismiss();
                break;
                }
            case EAknSoftkeySelect:
                {
                HandleItemClick();
                break;
                }
            default:
                {
                // Nothing to do
                break;
                }    
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::CFscContactActionMenuImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuImpl::CFscContactActionMenuImpl( 
    const TFscContactActionMenuConstructParameters& aParams ) 
    : CFscContactActionMenu(), iMode(aParams.iMode),
      iSelectedItemIndex( 0 ), isExecuteCompleted( ETrue ),
      iOpenedFromMR( EFalse )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ConstructL(
    const TFscContactActionMenuConstructParameters& aParams )
    {
    FUNC_LOG;
    iModel = CFscContactActionMenuModelImpl::NewL( aParams.iService );
    iWait = new ( ELeave ) CActiveSchedulerWait();
    iTextResolver = CTextResolver::NewL( *CCoeEnv::Static() );
    iDummyMenuItem = CFscContactActionMenuItemImpl::NewL();
    iDummyMenuItem->SetImplementationUid( KNullUid );
    }   

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::DoExecuteL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::DoExecuteL( 
    TFscContactActionMenuPosition aPosition,
    TInt aIndex )
    {
    FUNC_LOG;
    
    // Show list
    iList->ShowL( aPosition, aIndex );
    ShowMenuCbaL();
    iFader.FadeBehindPopup( this, iList, ETrue );
    
    // Wait untill dialog is hidden again
    StartWait(); 
    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::DoDismiss
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::DoDismiss()
    {
    FUNC_LOG;
    if ( iList )
        {
        iList->Hide( !iExit ); // If no instant exit, slide
        }
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::StartWait
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::StartWait()
    {
    FUNC_LOG;
    iWait->Start();
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::StopWait
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::StopWait()
    {
    FUNC_LOG;
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::ShowMenuCbaL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ShowMenuCbaL()
    {
    FUNC_LOG;
    
    TInt cbaResource = 0;
    if ( iList->ItemCount() > 0)
        {
        cbaResource = R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT;
        }
    else
        {
        cbaResource = R_AVKON_SOFTKEYS_CANCEL;
        }    
    
    iButtonGroupContainer = CEikButtonGroupContainer::NewL( 
        CEikButtonGroupContainer::ECba,
        CEikButtonGroupContainer::EHorizontal,
        this, cbaResource, *iList, 
        CEikButtonGroupContainer::EDelayActivation );
        
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::HideMenuCba
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HideMenuCba()
    {
    FUNC_LOG;
    delete iButtonGroupContainer;
    iButtonGroupContainer = NULL;
    }  

// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::HandleItemClick
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::HandleItemClick()
    {
    FUNC_LOG;
    
    TInt selectedItemIndex = FocusedItemIndex();
    TInt itemCount = iModel->VisibleItemCount();
    if ( selectedItemIndex >= 0 && selectedItemIndex < itemCount )
        {
        // Check type of selected item
        CFscContactActionMenuItem* baseItem = NULL;
        TRAP( iError, baseItem = &iModel->VisibleItemL( selectedItemIndex ) );
        
		if ( iError == KErrNone )
		    {
            // Check type of selected item
            CFscContactActionMenuCasItemImpl* item =
                dynamic_cast<CFscContactActionMenuCasItemImpl*>( baseItem );
            if ( item != NULL )
                {
                iMenuResult = EFscCasItemSelectedAndExecuting;
                TUid actionUid = item->ImplementationUid();
                isExecuteCompleted = EFalse;
                iIsActionExecuting = ETrue;
                TRAP( iError,
                    iModel->Service()->ExecuteL( actionUid, this ) );
                }
            else
                {
                // Custom item is not executed by the menu
                iMenuResult = EFscCustomItemSelected;
                DoDismiss();
                }
		    }

		if ( iError != KErrNone )
            {
            // Show error note
            TPtrC buf;
            buf.Set( iTextResolver->ResolveErrorString( iError ) );
            if ( buf.Length() > 0 )
                {
                CAknWarningNote* note = new CAknWarningNote();
                
                if ( note )
                    {
                    // If error string was found and note execution is 
                    // successfull iError will be KErrNone. Otherwise 
                    // iError will be delivered to the client application
                    TRAP( iError, note->ExecuteLD( buf ) );
                    }
                else
                    {
                    iError = KErrNoMemory;
                    }
                }
            }
        }
    else
        {
        iMenuResult = EFscMenuDismissed;
        }
        
    }


// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when QueryActionsL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::QueryActionsComplete()
    {
    FUNC_LOG;
    // CAS observer method implementation - not used in this component
    }

// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when QueryActionsL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::QueryActionsFailed( TInt /*aError*/ )
    {
    FUNC_LOG;
    // CAS observer method implementation - not used in this component
    }
    
// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when ExecuteL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ExecuteComplete()
    {
    FUNC_LOG;
    isExecuteCompleted = ETrue;
    iIsActionExecuting = EFalse;
    iMenuResult = EFscCasItemSelectedAndExecuted;
    DoDismiss();
    }
     
// ---------------------------------------------------------------------------
// From MFscContactActionServiceObserver.
// Called when ExecuteL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::ExecuteFailed( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    isExecuteCompleted = ETrue;
    
    //Fix for: EJKA-7KAEVA
    if ( KErrCancel != iError )
        {
        // Show error note
        TPtrC buf;
        buf.Set( iTextResolver->ResolveErrorString( iError ) );
        if ( buf.Length() > 0 )
            {
            CAknWarningNote* note = new CAknWarningNote();
        
            if ( note )
                {
                // If error string was found and note execution is 
                // successfull iError will be KErrNone. Otherwise 
                // iError will be delivered to the client application
                TRAP( iError, note->ExecuteLD( buf ) );
                }
            else
                {
                iError = KErrNoMemory;
                }
            }
        }
    else
        {
        iError = KErrNone;
        iIsActionExecuting = EFalse;
        iMenuResult = EFscMenuDismissed;
        }
    DoDismiss();
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuImpl::UpdateFadeColors
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuImpl::UpdateFadeColors()
    {
    FUNC_LOG;
    
    }
    
// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
        sizeof( TImplementationProxy );
    return ImplementationTable;
    }

