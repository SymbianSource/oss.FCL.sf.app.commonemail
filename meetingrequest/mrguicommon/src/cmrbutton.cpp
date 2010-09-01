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
* Description:
*
*/

#include "cmrbutton.h"
#include "esmrhelper.h"

#include <gulicon.h>
#include <esmrgui.mbg>
#include <data_caging_path_literals.hrh>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRButton::~CMRButton
// ---------------------------------------------------------------------------
//
EXPORT_C  CMRButton* CMRButton::NewL(
        NMRBitmapManager::TMRBitmapId aBitmapId,
        const CCoeControl* aParent,
        TScaleMode aScaleMode
        )
    {
    CMRButton* self = new (ELeave) CMRButton( aScaleMode );
    CleanupStack::PushL( self );
    self->ConstructL( aBitmapId, aParent );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMRButton::~CMRButton
// ---------------------------------------------------------------------------
//
EXPORT_C CMRButton::~CMRButton()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRButton::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRButton::SizeChanged()
    {
    TSize sz = Rect().Size();
    const CFbsBitmap* bitmap = GetCurrentIcon()->Bitmap();
    AknIconUtils::SetSize( const_cast< CFbsBitmap* >( bitmap ), sz, iScaleMode );
    }

// ---------------------------------------------------------------------------
// CMRButton::CMRButton
// ---------------------------------------------------------------------------
//
CMRButton::CMRButton( TScaleMode aScaleMode )
    : CAknButton( KAknButtonNoFrame | KAknButtonPressedDownFrame ),
      iScaleMode( aScaleMode )
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// CMRButton::ConstructL
// ---------------------------------------------------------------------------
//
void CMRButton::ConstructL(
        NMRBitmapManager::TMRBitmapId aBitmapId,
        const CCoeControl* aParent )
    {
    CFbsBitmap* bitMap( NULL );
    CFbsBitmap* bitMapMask( NULL );
    TSize sz( Rect().Size() );
    User::LeaveIfError( NMRBitmapManager::GetSkinBasedBitmap( 
                                aBitmapId, 
                                bitMap, bitMapMask, sz ) ); 
    
    CGulIcon* icon = CGulIcon::NewL( bitMap, bitMapMask );
    
    CAknButton::ConstructL( 
            icon, 
            NULL, 
            NULL, 
            NULL, 
            KNullDesC(), 
            KNullDesC(), 
            KAknButtonNoFrame | KAknButtonPressedDownFrame );
    
    if ( aParent )
        {
        SetContainerWindowL( *aParent );
        }
    }

// EOF
