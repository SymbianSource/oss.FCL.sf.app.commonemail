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
* Description:  ESMR time (start-end) field implementation
 *
*/

#include "cesmrtimefield.h"
#include "cmrtimecontainer.h"
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "cmrimage.h"
#include "nmrlayoutmanager.h"

// DEBUG
#include "emailtrace.h"

namespace // codescanner::namespace
    {
    const TInt KFieldComponentCount( 2 );
    } // unnamed namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTimeField::CESMRTimeField
// ---------------------------------------------------------------------------
//
CESMRTimeField::CESMRTimeField( )
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldMeetingTime );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::~CESMRTimeField
// ---------------------------------------------------------------------------
//
CESMRTimeField::~CESMRTimeField( )
    {
    FUNC_LOG;
    // iContainer is deleted by framework
    delete iFieldIcon;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::NewL
// ---------------------------------------------------------------------------
//
CESMRTimeField* CESMRTimeField::NewL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRTimeField* self = new (ELeave) CESMRTimeField;
    CleanupStack::PushL ( self );
    self->ConstructL ( aValidator );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::ConstructL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CCoeControl::SetComponentsToInheritVisibility( ETrue );
    iContainer =
            CMRTimeContainer::NewL(
                    aValidator,
                    this,
                    EESMRFieldMeetingTime );
    // This is taking ownership
    CESMRField::ConstructL( iContainer );

    iFieldIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapClock );
    iFieldIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::SetContainerWindowL(
        const CCoeControl& aControl )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL ( aControl );
    if ( iContainer )
        {
        iContainer->SetContainerWindowL ( aControl );
        iContainer->SetParent( this );
        }
    if ( iFieldIcon )
        {
        iFieldIcon->SetContainerWindowL( aControl );
        iFieldIcon->SetParent( this );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRTimeField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( iContainer )
        {
        response = iContainer->OfferKeyEventL ( aEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRTimeField::OkToLoseFocusL(
        TESMREntryFieldId aNext )
    {
    FUNC_LOG;
    if ( iContainer )
        {
        return iContainer->OkToLoseFocusL ( FieldId ( ), aNext );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SetValidatorL
// ---------------------------------------------------------------------------
//
void CESMRTimeField::SetValidatorL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    
    // Update iValidator member
    CESMRField::SetValidatorL( aValidator );
    
    if ( iContainer )
        {
        iContainer->SetValidatorL( iValidator );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
TBool CESMRTimeField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    
    TBool retValue( EFalse );
    
    if ( iContainer && EMRCmdDoEnvironmentChange == aCommand )
        {
        iContainer->EnvironmentChangedL();
        SizeChanged();
        DrawDeferred();
    
        retValue = ETrue;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRTimeField::SizeChanged()
    {
    TRect rect( Rect() );
    TAknLayoutRect iconLayout = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTimeEditorIcon );
    TRect iconRect( iconLayout.Rect() );
    iFieldIcon->SetRect( iconRect );
    
    TAknLayoutRect bgLayoutRect = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    iContainer->SetRect( rect );    
    iContainer->SetBgRect( bgRect );
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRTimeField::CountComponentControls() const
    {
    TInt count( KFieldComponentCount );
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRTimeField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRTimeField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iContainer;
        default:
            return NULL;
        }
    }

// EOF

