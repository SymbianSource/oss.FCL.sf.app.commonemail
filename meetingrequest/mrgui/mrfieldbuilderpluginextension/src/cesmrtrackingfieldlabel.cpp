/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR UI Container class
*
*/

// CLASS HEADER
#include "emailtrace.h"
#include "cesmrtrackingfieldlabel.h"

// USER INCLUDES
#include "mesmrlistobserver.h"
#include "cesmrborderlayer.h"

// SYSTEM INCLUDES
#include <eiklabel.h>
#include <avkon.hrh>
#include <caluser.h>
#include <calentry.h>
#include <StringLoader.h>
#include <esmrgui.rsg>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::NewL
// ---------------------------------------------------------------------------
//
CESMRTrackingFieldLabel* CESMRTrackingFieldLabel::NewL(
        CCalAttendee::TCalRole aRole )
    {
    FUNC_LOG;
    CESMRTrackingFieldLabel* self = new (ELeave) CESMRTrackingFieldLabel( aRole );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRTrackingFieldLabel::ConstructL( )
    {
    FUNC_LOG;
    SetFieldId ( (iRole == CCalAttendee::EReqParticipant) ? EESMRFieldReqTrack
            : EESMRFieldOptTrack );

    iTitle = new (ELeave) CEikLabel();
    iTitle->SetLabelAlignment ( CESMRLayoutManager::IsMirrored ( ) ? ELayoutAlignRight
            : ELayoutAlignLeft );


    HBufC* label = StringLoader::LoadLC (iRole == CCalAttendee::EReqParticipant ?
            R_QTN_MEET_REQ_LABEL_REQUIRED : R_QTN_MEET_REQ_LABEL_OPT );

    iTitle->SetTextL ( *label );
    CleanupStack::PopAndDestroy( label );

    //this label control cannot ever get keyboard focus
    SetFocusing(EFalse);
   }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRTrackingFieldLabel::InitializeL()
    {
    FUNC_LOG;
    iTitleSize = iLayout->FieldSize ( EESMRFieldAttendeeLabel );
    iTitle->SetFont( iLayout->Font (iCoeEnv, EESMRFieldAttendeeLabel ) );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRTrackingFieldLabel::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect = Rect ( );
    if ( CESMRLayoutManager::IsMirrored ( ) )
        {
        TPoint titlePos( rect.iBr.iX - iTitleSize.iWidth, rect.iTl.iY);
        iTitle->SetExtent ( titlePos, iTitleSize );
        }
    else
        {
        // title
        iTitle->SetExtent ( rect.iTl, iTitleSize );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRTrackingFieldLabel::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize /*aSize */)
    {
    FUNC_LOG;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRTrackingFieldLabel::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = CESMRField::CountComponentControls ( );
    if ( iTitle )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRTrackingFieldLabel::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    if ( aInd == 0 )
        {
        return iTitle;
        }
    return CESMRField::ComponentControl ( aInd );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::~CESMRTrackingFieldLabel
// ---------------------------------------------------------------------------
//
CESMRTrackingFieldLabel::~CESMRTrackingFieldLabel( )
    {
    FUNC_LOG;
    delete iTitle;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingFieldLabel::CESMRTrackingFieldLabel
// ---------------------------------------------------------------------------
//
CESMRTrackingFieldLabel::CESMRTrackingFieldLabel(
        CCalAttendee::TCalRole aRole ) :
    iRole( aRole)
    {
    FUNC_LOG;
    //do nothing
    }

// End of File

