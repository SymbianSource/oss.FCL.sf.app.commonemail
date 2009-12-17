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
* Description:  Base class for icon fields
*
*/


#include "emailtrace.h"
#include <AknIconUtils.h>
#include <fbs.h>
#include <eikimage.h>
#include <layoutmetadata.cdl.h>
#include <AknsItemID.h>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>

#include "cesmriconfield.h"
#include "cesmrborderlayer.h"
#include "esmrhelper.h"
#include "cesmrlayoutmgr.h"

using namespace ESMRLayout;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRIconField::CESMRIconField
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRIconField::CESMRIconField()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRIconField::~CESMRIconField
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRIconField::~CESMRIconField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iBitmap;
    delete iBitmapMask;
    }

// ---------------------------------------------------------------------------
// CESMRIconField::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRIconField::ConstructL( 
		TAknsItemID aIconID,
        CCoeControl* aControl,
        TESMRFieldFocusType aFocusType )
    {
    FUNC_LOG;
    IconL( aIconID );
    iIconID = aIconID;
    CESMRField::ConstructL( aControl, aFocusType );
    }

// ---------------------------------------------------------------------------
// CESMRIconField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRIconField::SetContainerWindowL(
		const CCoeControl& aContainer )
    {
    FUNC_LOG;
    CESMRField::SetContainerWindowL( aContainer );

    if ( iIcon )
        {
        iIcon->SetContainerWindowL( aContainer );
        }
    }

// ---------------------------------------------------------------------------
// CESMRIconField::IconL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRIconField::IconL( TAknsItemID aIconID )
    {
    FUNC_LOG;

    // update the icon id:
    iIconID = aIconID;

    delete iIcon;
    iIcon = NULL;
    delete iBitmap;
    iBitmap = NULL;
    delete iBitmapMask;
    iBitmapMask = NULL;

    TInt fileIndex(-1);
    TInt fileMaskIndex(-1);

    CESMRLayoutManager::SetBitmapFallback( aIconID, fileIndex, fileMaskIndex );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath);

    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            aIconID,
                            iBitmap,
                            iBitmapMask,
                            bitmapFilePath,
                            fileIndex,
                            fileMaskIndex
                            );


    // Even if creating mask failed, bitmap can be used 
    // (background is just not displayed correctly)
    if( iBitmap )
        {
        AknIconUtils::SetSize( iBitmap, KIconSize, EAspectRatioPreserved );
        iIcon = new (ELeave) CEikImage;
        iIcon->SetPictureOwnedExternally(ETrue);
        iIcon->SetBrushStyle( CWindowGc::ENullBrush );
        iIcon->SetPicture( iBitmap, iBitmapMask );
        }
    }

// ---------------------------------------------------------------------------
// CESMRIconField::CountComponentControls
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRIconField::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iBorder )
    	{
        ++count;
    	}

    if ( iIcon )
    	{
        ++count;
    	}

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRIconField::ComponentControl
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CESMRIconField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch( aInd )
    	{
    	case 0:
    		{
    		return iIcon;    		
    		}
    	case 1:
    		{
    		return iBorder;
    		}
    	default:
    		{
    		return NULL;
    		}
    	}
    }

// ---------------------------------------------------------------------------
// CESMRIconField::SizeChanged
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRIconField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect = Rect();
    TPoint iconPos( 
    		TPoint( rect.iTl.iX, rect.iTl.iY + iLayout->IconTopMargin() ));
    TPoint borderPos( 
    		iconPos.iX + KIconSize.iWidth + KIconBorderMargin, rect.iTl.iY );
    TSize borderSize( 
    	rect.Width() - (KIconSize.iWidth + KIconBorderMargin ), rect.Height() );

    TRAPD( error, IconL( iIconID ) );

    if ( CESMRLayoutManager::IsMirrored() )
        {
        CCoeControl* parent = Parent();
        TRect r = parent->Rect();

        iconPos.iX = r.iBr.iX - KIconSize.iWidth - KMirroredIconMargin;
        borderPos.iX = iconPos.iX - (borderSize.iWidth + KIconBorderMargin );
        }

    if ( iIcon && error == KErrNone )
        {
        iIcon->SetPosition( iconPos );
        iIcon->SetSize( KIconSize );
        }

    if ( iBorder && error == KErrNone )
        {
        // reserve icon rect.
        iBorder->SetExtent( borderPos, BorderSize() );
        }
        }

// ---------------------------------------------------------------------------
// CESMRIconField::BorderSize
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CESMRIconField::BorderSize() const
    {
    FUNC_LOG;
    TSize borderSize( Rect().Width() - ( KIconSize.iWidth + 
            KIconBorderMargin ), Rect().Height() );
    
    return borderSize;
    }

