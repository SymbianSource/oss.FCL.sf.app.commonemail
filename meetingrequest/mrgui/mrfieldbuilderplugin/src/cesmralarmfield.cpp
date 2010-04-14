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
* Description:  ESMR alarm field implementation
 *
*/

#include "cesmralarmfield.h"
#include "cesmralarm.h"
#include "esmrcommands.h"
#include "cesmrglobalnote.h"
#include "mesmrfieldvalidator.h"
#include "cesmrlistquery.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"

#include <avkon.hrh>
#include <e32keys.h>
#include <barsc.h>
#include <barsread.h>
#include <calentry.h>
#include <calalarm.h>
#include <esmrgui.rsg>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <aknutils.h>

// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace{ // codescanner::namespace

const TInt KMinuteInMicroSeconds(60000000);
const TInt KComponentCount( 2 );

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAlarmField::NewL
// ---------------------------------------------------------------------------
//
CESMRAlarmField* CESMRAlarmField::NewL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRAlarmField* self = new (ELeave)CESMRAlarmField( aValidator );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::~CESMRAlarmField
// ---------------------------------------------------------------------------
//
CESMRAlarmField::~CESMRAlarmField( )
    {
    FUNC_LOG;
    iArray.ResetAndDestroy ( );
    iArray.Close ( );
    delete iIcon;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCoeEnv* env = CCoeEnv::Static ( );
    // esmrgui.rss
    TResourceReader reader;
    env->CreateResourceReaderLC ( reader, R_ESMREDITOR_ALARMS );

    // Read alarm items to array
    iArray.ResetAndDestroy( );
    TInt count = reader.ReadInt16 ( );
    for (TInt i(0); i < count; i++ )
        {
        CESMRAlarm* alarm = new (ELeave) CESMRAlarm;
        CleanupStack::PushL ( alarm );
        alarm->ConstructFromResourceL ( reader );
        alarm->LoadTextL ( env );
        CleanupStack::Pop ( alarm );
        iArray.Append ( alarm );
        }

    // reader
    CleanupStack::PopAndDestroy(); // codescanner::cleanup 

    // if alarm is relative then set the text on right index.
    MESMRCalEntry::TESMRAlarmType alarmType;
    TTime alarmTime;
    aEntry.GetAlarmL (alarmType, alarmTime );

    switch (alarmType )
        {
        case MESMRCalEntry::EESMRAlarmRelative:
            {// Show relative alarm field and set its time.
             // time units can be minutes, hours or days
             TInt timeInMins = 0;
             CCalEntry& entry = aEntry.Entry ( );
             TTime entryTime= entry.StartTimeL().TimeLocalL ( );
             timeInMins = (entryTime.Int64() - alarmTime.Int64())/ KMinuteInMicroSeconds;
             // Loop through iArray and get the relevent item
             for (TInt i(0); i < iArray.Count ( ); i++ )
                 {
                 if ( iArray[i]->Relative ( )== timeInMins )
                     {
                     iAlarm->SetTextL ( *(iArray[i]->Text()) );
                     iOptIndex = i; // update current alarm index.

                     // Let's also update the current alarm offset to iValidator
                    TBool handleAlarmChange( EFalse );
                    iValidator->RelativeAlarmChangedL(
                            iArray[ iOptIndex ]->RelativeTimeInMinutes(),
                            handleAlarmChange,
                            iRelativeAlarmValid );
                     break;
                     }
                 }
            }
            break;
            //flow through
        case MESMRCalEntry::EESMRAlarmAbsolute:
        case MESMRCalEntry::EESMRAlarmNotFound:
        default:
            { // Search the default value (EAlarmFifteenMinutes) and set it
            for (TInt i(0); i < iArray.Count ( ); i++ )
                {
                if ( iArray[i]->Id ( )== EAlarmOff )
                    {
                    iAlarm->SetTextL ( *(iArray[i]->Text()) );
                    iOptIndex = i; // update current alarm index.

                    // Let's also update the current alarm offset to iValidator
                    TBool handleAlarmChange( EFalse );
                    iValidator->RelativeAlarmChangedL(
                            iArray[ iOptIndex ]->RelativeTimeInMinutes(),
                            handleAlarmChange,
                            iRelativeAlarmValid );
                    break;
                    }
                }// End For loop
            }
            break;
        }

    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( iRelativeAlarmValid )
        {
        CESMRAlarm* alarm = iArray[ iOptIndex ];
        // if alarm is set
        if ( alarm->Id()!= EAlarmOff )
            {// All Day event has absolute alarm handled in validator class
            // if not visible, this entry has absolute alarm.
            if ( IsVisible() )
                {
                CCalAlarm* calAlarm = CCalAlarm::NewL();
                CleanupStack::PushL( calAlarm );
                calAlarm->SetTimeOffset( alarm->RelativeTimeInMinutes() );

                CCalEntry& entry = aEntry.Entry();
                entry.SetAlarmL( calAlarm );
                CleanupStack::PopAndDestroy( calAlarm );
                }
            }
        else
            {
            CCalEntry& entry = aEntry.Entry( );
            entry.SetAlarmL( NULL );
            }
        }
    else
        {
        CCalEntry& entry = aEntry.Entry();
        entry.SetAlarmL( NULL );
        }

    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRAlarmField::OkToLoseFocusL(
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    return TriggerValidatorL();
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRAlarmField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);

    if ( aType == EEventKey )
        {
        switch ( aEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                {
                // next alert item
                if ( iOptIndex > 0 )
                    {
                    UpdateAlarmLabelL ( --iOptIndex );
                    response = EKeyWasConsumed;
                    }
                }
                break;

            case EStdKeyRightArrow:
                {
                // next alert item
                if ( iOptIndex < (iArray.Count() - 1) )
                    {
                    UpdateAlarmLabelL ( ++iOptIndex );
                    response = EKeyWasConsumed;
                    }
                }
                break;

            default:
                break;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::CESMRAlarmField
// ---------------------------------------------------------------------------
//
CESMRAlarmField::CESMRAlarmField( MESMRFieldValidator* aValidator )
:   iOptIndex(0),
    iRelativeAlarmValid( ETrue )
    {
    FUNC_LOG;
    
    iValidator = aValidator;
    SetFieldId ( EESMRFieldAlarm );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::ConstructL( )
    {
    FUNC_LOG;
    iAlarm = CMRLabel::NewL(); // base class takes ownership
    CESMRField::ConstructL( iAlarm );
    iAlarm->SetTextL ( KNullDesC() );
        
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarm );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRAlarmField::CountComponentControls() const
    {
    return KComponentCount;
    }
    
// ---------------------------------------------------------------------------
// CESMRAlarmField::ComponentControl
// ---------------------------------------------------------------------------
//    
CCoeControl* CESMRAlarmField::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = NULL;
    switch( aIndex )
        {
        case 0:
            {
            control = iAlarm;            
            break;
            }
        case 1:
            {
            control = iIcon;
            break;
            }
        default:
            ASSERT( EFalse );
        }
    
    return control;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::SizeChanged()
    {
    TRect rect( Rect() );
    TAknLayoutRect iconLayout = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorIcon );
    TRect iconRect( iconLayout.Rect() );
    iIcon->SetRect( iconRect );
    
    TAknLayoutRect bgLayoutRect = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    TAknTextComponentLayout editorLayout =
        NMRLayoutManager::GetTextComponentLayout( 
                NMRLayoutManager::EMRTextLayoutTextEditor );    
    
    AknLayoutUtils::LayoutLabel( iAlarm, rect, editorLayout );         
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::UpdateAlarmLabelL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::UpdateAlarmLabelL( TInt aIndex )
    {
    FUNC_LOG;
    if (aIndex < iArray.Count() )
        {
        HBufC* txt = iArray[ aIndex ]->Text ( );
        iAlarm->SetTextL ( *txt );

        // Let's only update the current alarm offset to iValidator
        TBool handleAlarmChange( EFalse );
        if (iValidator)
            {
            iValidator->RelativeAlarmChangedL(
                iArray[ aIndex ]->RelativeTimeInMinutes(),
                handleAlarmChange,
                iRelativeAlarmValid );
            }
        }
    iAlarm->DrawDeferred ( );
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::UpdateAlarmToFirstValidValueL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::UpdateAlarmToFirstValidValueL()
    {
    FUNC_LOG;
    // Changing the alarm to first valid value
    for (TInt i(0); i <= iOptIndex; i++ )
        {
        TTimeIntervalMinutes alarmOffset =
            iArray[ iOptIndex - i ]->RelativeTimeInMinutes(); //codescanner::accessArrayElementWithoutCheck2

        if ( iValidator->IsRelativeAlarmValid( alarmOffset ) )
            {
            iOptIndex = iOptIndex - i;
            break;
            }
        }

    UpdateAlarmLabelL( iOptIndex );
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus is gained on the field
    if (aFocus)
        {
        ChangeMiddleSoftKeyL(EESMRCmdOpenAlarmQuery,R_QTN_MSK_OPEN);
        }
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CESMRAlarmField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if(aCommand == EESMRCmdOpenAlarmQuery ||
       aCommand == EAknCmdOpen )
        {
		HandleTactileFeedbackL();
		
        ExecuteMSKCommandL();
        isUsed = ETrue;
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::ExecuteMskCommandL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::ExecuteMSKCommandL()
    {
    FUNC_LOG;
    DoAlarmTimeQueryL();
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::DoAlarmTimeQueryL
// ---------------------------------------------------------------------------
//
void CESMRAlarmField::DoAlarmTimeQueryL()
    {
    FUNC_LOG;
    TInt ret( CESMRListQuery::ExecuteL(
            CESMRListQuery::EESMRRelativeAlarmTimeQuery ) );
    if ( ret != KErrCancel )
        {
        iOptIndex = ret;
        UpdateAlarmLabelL( iOptIndex );
        }
    }

// ---------------------------------------------------------------------------
// CESMRAlarmField::TriggerValidatorL
// ---------------------------------------------------------------------------
//
TBool CESMRAlarmField::TriggerValidatorL()
    {
    FUNC_LOG;
    TBool entryOk( ETrue );
    
    // Let's handle the relative alarm change in iValidator
    TRAPD(err, iValidator->RelativeAlarmChangedL(
            iArray[ iOptIndex ]->RelativeTimeInMinutes(), //codescanner::accessArrayElementWithoutCheck2
            ETrue, // handleAlarmChange
            iRelativeAlarmValid ) );

    if ( err != KErrNone )
        {
        entryOk = EFalse;
        CESMRGlobalNote::ExecuteL ( CESMRGlobalNote::EESMRAlarmAlreadyPassed );
        UpdateAlarmToFirstValidValueL();
        }

    return entryOk;
    }

// EOF

