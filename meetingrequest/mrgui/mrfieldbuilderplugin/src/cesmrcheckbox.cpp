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
* Description:  ESMR All day event checkbox field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrcheckbox.h"

#include <eiklabel.h>
#include <AknIconUtils.h>
#include <fbs.h>
#include <eikimage.h>
#include <StringLoader.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>

#include <esmrgui.rsg>
#include "cesmrborderlayer.h"
#include "mesmrlistobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrfieldvalidator.h"
#include "esmrconfig.hrh"
#include "cesmriconfield.h"
#include "esmrhelper.h"
#include "cesmrlayoutmgr.h"

using namespace ESMRLayout;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCheckbox::NewL
// ---------------------------------------------------------------------------
//
CESMRCheckBox* CESMRCheckBox::NewL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRCheckBox* self = new (ELeave) CESMRCheckBox( aValidator );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::~CESMRCheckbox
// ---------------------------------------------------------------------------
//
CESMRCheckBox::~CESMRCheckBox( )
    {
    FUNC_LOG;
    delete iIcon;
    delete iBitmap;
    delete iBitmapMask;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        SwitchMSKLabelL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if(aCommand == EESMRCmdCheckEvent)
        {
        HandleCheckEventL();
        SwitchMSKLabelL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRCheckBox::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( aType == EEventKey )
        {
        if ( aEvent.iScanCode == EStdKeyDevice3 )
            {
            HandleCheckEventL ( );
            SwitchMSKLabelL();
            response = EKeyWasConsumed;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRCheckBox::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = CESMRField::CountComponentControls ( );
    if ( iIcon )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRCheckBox::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    if ( aInd == 0 )
        {
        return iIcon;
        }
    return CESMRField::ComponentControl ( aInd );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect = Rect ( );

    TInt iconTopMargin = ( rect.Height() - KIconSize.iHeight ) / 2;

    TPoint iconPos( rect.iTl.iX + KIconSize.iWidth + KIconBorderMargin,
                    rect.iTl.iY + iconTopMargin );


    TPoint borderPos( iconPos.iX + KIconSize.iWidth + KIconBorderMargin,
                      rect.iTl.iY);

    TSize borderSize( rect.Width() -
                      (2 * KIconSize.iWidth + 2 * KIconBorderMargin ),
                      rect.Height ( ));

    if ( CESMRLayoutManager::IsMirrored ( ) )
        {
        iconPos.iX = rect.iBr.iX - ( 2 * KIconSize.iWidth );
        borderPos.iX = iconPos.iX - borderSize.iWidth - KIconBorderMargin;
        }

    if ( iIcon )
        {
        iIcon->SetPosition ( iconPos );
        iIcon->SetSize ( KIconSize );
        }

    if ( iBorder )
        {
        iBorder->SetPosition ( borderPos );
        iBorder->SetSize ( borderSize );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::InitializeL()
    {
    FUNC_LOG;
    iLabel->SetFont( iLayout->Font( iCoeEnv, iFieldId ) );
    AknLayoutUtils::OverrideControlColorL(
            *iLabel,
            EColorLabelText,
            iLayout->GeneralListAreaTextColor() );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TBool alldayEvent( aEntry.IsAllDayEventL() );

    // Update the validator
    iValidator->SetAllDayEventL(  alldayEvent );

    // if this is all day event and not checked
    if ( alldayEvent && !iChecked )
        {
        HandleCheckEventL();
        }
    SetIconL( iChecked );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::ExternalizeL( MESMRCalEntry& /*aEntry*/)
    {
    FUNC_LOG;
    iValidator->SetAllDayEventL( iChecked );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetIconL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SetIconL( TBool aChecked )
    {
    FUNC_LOG;
    delete iIcon;
    iIcon = NULL;
    delete iBitmap;
    iBitmap = NULL;
    delete iBitmapMask;
    iBitmapMask = NULL;

    TAknsItemID iconID;
    TInt fileIndex(KErrNotFound);
    TInt fileMaskIndex(KErrNotFound);

    if( aChecked )
        {
        iconID = KAknsIIDQgnMeetReqIndiCheckboxMark;
        }
    else
        {
        iconID = KAknsIIDQgnMeetReqIndiCheckboxUnmark;
        }

    CESMRLayoutManager::SetBitmapFallback( iconID, fileIndex, fileMaskIndex );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath);

    TRAPD( err, AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            iconID,
                            iBitmap,
                            iBitmapMask,
                            bitmapFilePath,
                            fileIndex,
                            fileMaskIndex
                            ) );

    if (err == KErrNotFound)
        {
        User::Leave(KErrNotFound);
        }

    // Even if creating mask failed, bitmap can be used (background is just not displayed correctly)
    if( iBitmap )
        {
        AknIconUtils::SetSize( iBitmap, KIconSize, EAspectRatioPreserved );
        iIcon = new (ELeave) CEikImage;
        iIcon->SetPictureOwnedExternally(ETrue);
        iIcon->SetPicture( iBitmap, iBitmapMask );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::HandleCheckEventL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::HandleCheckEventL( )
    {
    FUNC_LOG;
    /*
     * Change check status
     */
    iChecked = !iChecked; // change status
    SetIconL ( iChecked ); // set icon for the status

    // update validator status:
    iValidator->SetAllDayEventL ( iChecked );

    /*
     * Add / remove new fields to list
     */
    if ( iChecked )
        {
        // start - end time should be removed
        // relateive alarm should be removed
        iObserver->RemoveControl ( EESMRFieldMeetingTime );
        iObserver->RemoveControl ( EESMRFieldAlarm );
        // absolute alarm should be inserted
        iObserver->InsertControl ( EESMRFieldAlarmOnOff );
        }
    else
        {
        iObserver->InsertControl ( EESMRFieldMeetingTime );
        iObserver->InsertControl ( EESMRFieldAlarm );
        iObserver->RemoveControl ( EESMRFieldAlarmOnOff );
        iObserver->RemoveControl ( EESMRFieldAlarmTime );
        iObserver->RemoveControl ( EESMRFieldAlarmDate );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SwitchMSKLabelL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SwitchMSKLabelL()
    {
    FUNC_LOG;
    if(iChecked)
        {
        ChangeMiddleSoftKeyL(EESMRCmdCheckEvent, R_QTN_MSK_UNMARK);
        }
    else
        {
        ChangeMiddleSoftKeyL(EESMRCmdCheckEvent, R_QTN_MSK_MARK);
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::CESMRCheckbox
// ---------------------------------------------------------------------------
//
CESMRCheckBox::CESMRCheckBox( MESMRFieldValidator* aValidator ) :
    iValidator( aValidator), iChecked( EFalse)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::ConstructL( )
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldAllDayEvent );
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetLabelAlignment( CESMRLayoutManager::IsMirrored()
                               ? ELayoutAlignRight : ELayoutAlignLeft );

    HBufC* txt = StringLoader::LoadLC ( R_QTN_MEET_REQ_ALL_DAY_EVENT );
    iLabel->SetTextL( *txt );
    CleanupStack::PopAndDestroy( txt );

    iIcon = new (ELeave) CEikImage;

    CESMRField::ConstructL( iLabel );
    }

// EOF

