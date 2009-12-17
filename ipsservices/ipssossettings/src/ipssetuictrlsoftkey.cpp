/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsSetUiCtrlSoftkey.
*
*/


#include "emailtrace.h"
#include <StringLoader.h>           // StringLoader
#include <eikbtgpc.h>           // CEikButtonGroupContainer

#include "ipssetutilsexception.h"

#include "ipssetuictrlsoftkey.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::CIpsSetUiCtrlSoftkey()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlSoftkey::CIpsSetUiCtrlSoftkey(
     CEikButtonGroupContainer& aSoftkeys )
     :
     iSoftkeys( aSoftkeys ),
     iFlags( 0 )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::~CIpsSetUiCtrlSoftkey()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlSoftkey::~CIpsSetUiCtrlSoftkey()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlSoftkey* CIpsSetUiCtrlSoftkey::NewL(
     CEikButtonGroupContainer& aSoftkeys )
    {
    FUNC_LOG;
    CIpsSetUiCtrlSoftkey* self = NewLC( aSoftkeys );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlSoftkey* CIpsSetUiCtrlSoftkey::NewLC(
     CEikButtonGroupContainer& aSoftkeys )
    {
    FUNC_LOG;
    CIpsSetUiCtrlSoftkey* self = new ( ELeave ) CIpsSetUiCtrlSoftkey( aSoftkeys );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
   

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::ChangeObserver()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiCtrlSoftkey::ChangeObserver( 
    const TBool aOn,
    const TIpsSetUiObservedKeys aSoftkey,
    MEikCommandObserver& aCommandObserver )
    {
    FUNC_LOG;
    TInt error = KErrNone;
    
    if ( aOn )
        {
        error = SetObserver( aSoftkey, aCommandObserver );
        }
    else
        {
        RemoveObserver( aSoftkey );
        }
        
    return error;        
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::SetObserver()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiCtrlSoftkey::SetObserver(
    const TIpsSetUiObservedKeys aSoftkey,
    MEikCommandObserver& aCommandObserver )
    {
    FUNC_LOG;
    TRAPD( error, 
        iSoftkeys.UpdateCommandObserverL( aSoftkey, aCommandObserver ) );

    iFlags |= aSoftkey;
    
    return error;
    }
        
// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::SetObserver()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiCtrlSoftkey::SetObserver( 
    const TBool aLeftSoftkey,
    const TBool aRightSoftkey,
    const TBool aMiddleSoftkey,
    MEikCommandObserver& aCommandObserver )
    {
    FUNC_LOG;
    // Set observer for the left softkey
    TInt error = ChangeObserver( 
        aLeftSoftkey, ELeftSoftkey, aCommandObserver );    
    IPS_ASSERT_DEBUG( error == KErrNone, ESoftkeyLeft, ESoftkeys ); 

    // Set observer for the right softkey        
    if ( error == KErrNone )    
        {        
        error = ChangeObserver( 
            aRightSoftkey, ERightSoftkey, aCommandObserver );
            
        IPS_ASSERT_DEBUG( error == KErrNone, ESoftkeyRight, ESoftkeys ); 
        }

    // Set observer for the middle softkey
    if ( error == KErrNone )    
        {                
        error = ChangeObserver( 
            aMiddleSoftkey, EMiddleSoftkey, aCommandObserver );        
        IPS_ASSERT_DEBUG( error == KErrNone, ESoftkeyMiddle, ESoftkeys ); 
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::RemoveObserver()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::RemoveObserver( 
    const TIpsSetUiObservedKeys aSoftkey )
    {
    FUNC_LOG;
    iSoftkeys.RemoveCommandObserver( aSoftkey );

    iFlags &= ~aSoftkey;
    }
    
// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::operator()()
// ----------------------------------------------------------------------------
//
CEikButtonGroupContainer& CIpsSetUiCtrlSoftkey::operator()()
    {
    FUNC_LOG;
    return iSoftkeys;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::DefineCustomSoftkeyL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::DefineCustomSoftkeyL(
    const TInt aLabelResource,
    const TIpsSetUiObservedKeys aSoftkeyPos,
    const TInt aSoftkeyCmd,
    MEikCommandObserver* aCommandObserver )
    {
    FUNC_LOG;
    HBufC* options = NULL; // <cmail>
    
    if( aLabelResource == KErrNotFound )
        {
        options = NULL;
        DefineCustomSoftkeyL(
            *options, aSoftkeyPos, aSoftkeyCmd, aCommandObserver );
        }
    else
        {
        // Define buttons 'Options' and 'Start'
        options = StringLoader::LoadL( aLabelResource );
        CleanupStack::PushL( options );
        DefineCustomSoftkeyL(
            *options, aSoftkeyPos, aSoftkeyCmd, aCommandObserver );
        CleanupStack::PopAndDestroy( options );
        options = NULL;
        }
    }


// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::DefineCustomSoftkeyL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::DefineCustomSoftkeyL(
    const TDesC& aSoftkeyLabel,
    const TIpsSetUiObservedKeys aSoftkeyPos,
    const TInt aSoftkeyCmd,
    MEikCommandObserver* aCommandObserver )
    {
    FUNC_LOG;
    // Make sure there is matching command for the position
    if ( iSoftkeys.PositionById( aSoftkeyCmd ) > KErrNotFound )
        {        
        // First, remove the observer for the key
        if ( iFlags & aSoftkeyPos )
            {
            RemoveObserver( aSoftkeyPos );
            }
                
        // Remove the key information and then add new command            
        iSoftkeys.RemoveCommandFromStack( aSoftkeyPos, aSoftkeyCmd );
        iSoftkeys.AddCommandToStackL( aSoftkeyPos, aSoftkeyCmd, aSoftkeyLabel );
            
        if ( aCommandObserver )
            {
            // Set as observer and set the flags
            SetObserver( aSoftkeyPos, *aCommandObserver );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::MSKLabelling()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiCtrlSoftkey::MSKLabelling()
    {
    FUNC_LOG;
    return ( ( iFlags & EAllowMiddleSoftkeyLabelChange ) > 0 );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::SetMSKLabelling()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::SetMSKLabelling( const TBool aNewState )
    {
    FUNC_LOG;
    if ( aNewState )
        {
        iFlags |= EAllowMiddleSoftkeyLabelChange;
        }
    else 
        {
        iFlags &= EAllowMiddleSoftkeyLabelChange;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::UpdateMiddleSoftkeyLabelL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::UpdateMiddleSoftkeyLabelL( 
    const TInt aLabelResource, 
    const TInt aCommandId,
    MEikCommandObserver* aCommandObserver )
    {
    FUNC_LOG;
    // If labelling allowed, change the label for the middle soft key
    if ( MSKLabelling() && iPreviousMskLabelResource != aLabelResource )
        {
        iPreviousMskLabelResource = aLabelResource;
        DefineCustomSoftkeyL(
            aLabelResource, 
            EMiddleSoftkey,
            aCommandId,
            aCommandObserver );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiCtrlSoftkey::SetAbsorb()
// ---------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::SetAbsorb( const TBool aAbsorb )
    {
    FUNC_LOG;
    // Absorbing keys, keep the order, so the flags are not messed up.
    if ( aAbsorb )
        {
        iFlags |= EAbsorbing;
        SetObserver( ETrue, ETrue, ETrue, *this );
        }
    else
        {
        SetObserver( EFalse, EFalse, EFalse, *this );
        iFlags &= ~EAbsorbing;
        }                    
    }

// ---------------------------------------------------------------------------
// From class MEikCommandObserver.
// CIpsSetUiCtrlSoftkey::ProcessCommandL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiCtrlSoftkey::ProcessCommandL( TInt /* aCommandId */ )
    {
    FUNC_LOG;
    
    }

// End of File

