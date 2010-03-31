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

#include "cmrimage.h"
#include "esmrhelper.h"
#include <esmrgui.mbg>
#include <data_caging_path_literals.hrh>
#include <aknsconstants.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRImage::~CMRImage
// ---------------------------------------------------------------------------
//
EXPORT_C  CMRImage* CMRImage::NewL(
             NMRBitmapManager::TMRBitmapId aBitmapId, 
             TBool aIsMaskIcon,
             TScaleMode aScaleMode )
    {
    CMRImage* self = new (ELeave) CMRImage( aScaleMode );
    CleanupStack::PushL( self );
    self->ConstructL( aBitmapId, aIsMaskIcon );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMRImage::~CMRImage
// ---------------------------------------------------------------------------
//
EXPORT_C CMRImage::~CMRImage()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRImage::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRImage::SizeChanged()
    {
    TSize sz = Rect().Size();
    const CFbsBitmap* bitmap = Bitmap();
    AknIconUtils::SetSize( const_cast< CFbsBitmap* >( bitmap ), sz, iScaleMode );
    }

// ---------------------------------------------------------------------------
// CMRImage::CMRImage
// ---------------------------------------------------------------------------
//
CMRImage::CMRImage( TScaleMode aScaleMode )
    : iScaleMode( aScaleMode )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRImage::ConstructL
// ---------------------------------------------------------------------------
//
void CMRImage::ConstructL( NMRBitmapManager::TMRBitmapId aBitmapId,
						   TBool aIsMaskIcon  )
    {
    if( aIsMaskIcon )
    	{
    	CreateMaskIconL( aBitmapId );
    	}
    else
    	{
    	CreateIconL( aBitmapId );
    	}
    }

// ---------------------------------------------------------------------------
// CMRImage::CreateIconL
// ---------------------------------------------------------------------------
//
void CMRImage::CreateIconL( NMRBitmapManager::TMRBitmapId aBitmapId )
    {
    CFbsBitmap* bitMap( NULL );
    CFbsBitmap* bitMapMask( NULL );    
    TSize sz( Rect().Size() );
    User::LeaveIfError( 
                    NMRBitmapManager::GetSkinBasedBitmap( 
                            aBitmapId, 
                            bitMap, bitMapMask, sz ) ); 
    
    if( bitMap && bitMapMask )
        {
        SetPictureOwnedExternally( EFalse );
        SetBrushStyle( CWindowGc::ENullBrush );
        SetPicture( bitMap, bitMapMask );
        }
    }

// ---------------------------------------------------------------------------
// CMRImage::CreateMaskIconL
// ---------------------------------------------------------------------------
//
void CMRImage::CreateMaskIconL( NMRBitmapManager::TMRBitmapId aBitmapId )
	{
    CFbsBitmap* bitMap( NULL );
    CFbsBitmap* bitMapMask( NULL );    
    TSize sz( Rect().Size() );
    User::LeaveIfError( 
                    NMRBitmapManager::GetMaskIconBitmap( 
                            aBitmapId, 
                            bitMap, bitMapMask,
                            EAknsCIQsnIconColorsCG13,
                            sz ) ); 
    
    if( bitMap && bitMapMask )
        {
        SetPictureOwnedExternally( EFalse );
        SetBrushStyle( CWindowGc::ENullBrush );
        SetPicture( bitMap, bitMapMask );
        }
	}

// EOF
