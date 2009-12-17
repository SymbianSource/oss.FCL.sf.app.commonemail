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
* Description:  ESMR policy implementation
*
*/

#include "cesmrfield.h"

#include "esmrcommands.h"
#include "cesmrborderlayer.h"
#include "cesmrlayoutmgr.h"
#include "nmrlayoutmanager.h"
#include "mesmrfieldeventqueue.h"
#include "cesmrfieldcommandevent.h"
#include "cmrbackground.h"

#include "emailtrace.h"

#include <txtrich.h>
#include <eikenv.h>
#include <baclipb.h>
#include <eikbtgpc.h>
#include <StringLoader.h>
#include <AknUtils.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRField::CESMRField()
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRField::CESMRField()
    : iCustomMsk( EFalse ),
      iDisableRedraw( EFalse ),
      iDefaultMskVisible( EFalse ),
      iMskVisible( EFalse )
    {
    FUNC_LOG;
    // do nothing
    }


// ---------------------------------------------------------------------------
// CESMRField::~CESMRField()
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRField::~CESMRField()
    {
    FUNC_LOG;
    delete iBorder;
    if ( iEventQueue )
        {
        iEventQueue->RemoveObserver( this );
        }
    delete iBackground;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetExpandable()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetExpandable()
    {
    FUNC_LOG;
    iExpandable = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRField::IsExpandable()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::IsExpandable() const
    {
    FUNC_LOG;
    return iExpandable;
    }

// ---------------------------------------------------------------------------
// CESMRField::ExpandedHeight()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRField::ExpandedHeight() const
    {
    FUNC_LOG;
    return 0;
    }

// ---------------------------------------------------------------------------
// CESMRField::InitializeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::InitializeL()
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::FontChangedL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::FontChangedL()
    {
    FUNC_LOG;
    // This method re-initializes field
    InitializeL();
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFieldId()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFieldId( TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    iFieldId = aFieldId;
    }

// ---------------------------------------------------------------------------
// CESMRField::FieldId()
// ---------------------------------------------------------------------------
//
EXPORT_C TESMREntryFieldId CESMRField::FieldId() const
    {
    FUNC_LOG;
    return iFieldId;
    }

// ---------------------------------------------------------------------------
// CESMRField::ConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ConstructL( 
		CCoeControl* aControl, TESMRFieldFocusType aFocusType )
    {
    FUNC_LOG;
    iBorder = CESMRBorderLayer::NewL( aControl, aFocusType );
    iBorder->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRField::SizeChanged
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    if ( iBorder )
        {
        iBorder->SetRect( rect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::CountComponentControls
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRField::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iBorder )
    	{
        ++count;
    	}

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRField::ComponentControl()
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CESMRField::ComponentControl( TInt /*aInd*/ ) const
    {
    FUNC_LOG;
    return iBorder;
    }

// ---------------------------------------------------------------------------
// CESMRField::MinimumSize()
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CESMRField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );
    TAknLayoutRect listareaLayoutRect =
        NMRLayoutManager::GetLayoutRect( parentRect, NMRLayoutManager::EMRLayoutListArea );
    TAknLayoutRect fieldLayoutRect =
        NMRLayoutManager::GetFieldLayoutRect( listareaLayoutRect.Rect(), 1 );
    return TSize( Parent()->Size().iWidth, fieldLayoutRect.Rect().Height() );
    }

// ---------------------------------------------------------------------------
// CESMRField::SetContainerWindowL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetContainerWindowL(const CCoeControl& aContainer)
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    if ( iBorder )
        {
        iBorder->SetContainerWindowL( aContainer );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::SetListObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetListObserver( MESMRListObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    ListObserverSet();
    }

// ---------------------------------------------------------------------------
// CESMRField::SetListObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ListObserverSet()
    {
    FUNC_LOG;
    // default implementation.
    }

// ---------------------------------------------------------------------------
// CESMRField::SetLayoutManager()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetLayoutManager( CESMRLayoutManager* aLayout )
    {
    FUNC_LOG;
    iLayout = aLayout;
    if ( iBorder )
        {
        iBorder->SetLayoutManager( iLayout );
        }
    TRAP_IGNORE(
            iBackground = CMRBackground::NewL( *aLayout );
            this->SetBackground( iBackground );
            );
    }

// ---------------------------------------------------------------------------
// CESMRField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CESMRField::OfferKeyEventL( 
		const TKeyEvent& aEvent, TEventCode aType )
    {
    FUNC_LOG;
    if ( iBorder )
        {
        return iBorder->OfferKeyEventL( aEvent, aType );
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    iOutlineFocus = aFocus;
    if ( iBorder )
        {
        iBorder->SetOutlineFocusL( aFocus );
        }
    if ( aFocus )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if ( cba )
            {
            iDefaultMskVisible = cba->IsCommandVisibleByPosition(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition );
            iMskVisible = iDefaultMskVisible;
            }
        }
    else
        {
        RestoreMiddleSoftKeyL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::OkToLoseFocusL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::OkToLoseFocusL( // codescanner::LFunctionCantLeave
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetMinimumVisibleVerticalArea
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::GetMinimumVisibleVerticalArea(
		TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aUpper = 0;
    aLower = Rect().iBr.iY - Rect().iTl.iY;
    }

// ---------------------------------------------------------------------------
// CESMRField::InternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::InternalizeL( // codescanner::LFunctionCantLeave
        MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::ExternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ExternalizeL( // codescanner::LFunctionCantLeave
        MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ExecuteGenericCommandL( // codescanner::LFunctionCantLeave
        TInt /*aCommand*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::SetTitlePaneObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetTitlePaneObserver( 
		MESMRTitlePaneObserver* /*aObserver*/ )
    {
    FUNC_LOG;
    /* Empty implementation, subclasses should overwrite */
    }

// ---------------------------------------------------------------------------
// CESMRField::ChangeMiddleSoftKeyL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ChangeMiddleSoftKeyL( TInt aCommandId,
                                                TInt aResourceId )
    {
    FUNC_LOG;
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if ( cba )
        {
        HBufC* middleSKText = StringLoader::LoadLC( aResourceId,
                                                    iCoeEnv );
        cba->SetCommandL( 
        		CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
        		aCommandId, *middleSKText );
        CleanupStack::PopAndDestroy( middleSKText );
        if ( !iMskVisible )
            {
            cba->MakeCommandVisibleByPosition(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    ETrue );
            iMskVisible = ETrue;
            }
        cba->DrawDeferred();
        iCustomMsk = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::ChangeMiddleSoftKeyL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::ChangeMiddleSoftKeyL( TInt aResourceId )
    {
    FUNC_LOG;
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if ( cba )
        {
        cba->SetCommandL(
                CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                aResourceId );
        if ( !iMskVisible )
            {
            cba->MakeCommandVisibleByPosition(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    ETrue );
            iMskVisible = ETrue;
            }
        cba->DrawDeferred();
        iCustomMsk = ETrue;
        }       
    }

// ---------------------------------------------------------------------------
// CESMRField::CalculateVisibleRect()
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CESMRField::CalculateVisibleRect( TRect aRect )
    {
    FUNC_LOG;
    TRect targetRect(aRect);
    // highlight bitmap target size:
    TSize targetSize( Rect().Size() );

    // fetch the size of main pane
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( 
    		AknLayoutUtils::EMainPane, mainPaneRect );

    // the list drawable height:
    TInt listAreaHeight = mainPaneRect.Height() - iLayout->TitlePaneHeight();

    // if the size of field is larger than drawable height
    // let's downsize it:
    if ( Rect().Size().iHeight > listAreaHeight )
        {
        TInt shownHeight = Rect().Size().iHeight + Rect().iTl.iY;
        // check should the highlight be smaller than
        // whole screen:
        if ( shownHeight < listAreaHeight )
            {
            targetSize.iHeight = shownHeight;
            }
        else
            {
            targetSize.iHeight = listAreaHeight;
            }
        }

    // If part of the rect is not visible:
    if ( targetRect.iTl.iY < 0 )
        {
        targetRect.iTl.iY = 0;
        }

    // visible height:
    targetRect.SetHeight(listAreaHeight);

    return targetRect;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetEventQueueL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetEventQueueL( MESMRFieldEventQueue* aEventQueue )
    {
    FUNC_LOG;
    if ( aEventQueue != iEventQueue )
        {
        if ( aEventQueue )
            {
            // Add self to new queue 
            aEventQueue->AddObserverL( this );
            }
        
        if ( iEventQueue )
            {
            // Remove self from old queue
            iEventQueue->RemoveObserver( this );
            }
        
        iEventQueue = aEventQueue;
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::HasOutlineFocus()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRField::HasOutlineFocus() const
    {
    return iOutlineFocus;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetFocusRect()
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CESMRField::GetFocusRect() const
    {
    return iFocusRect;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFocusRect()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFocusRect( const TRect& aFocusRect )
    {
    iFocusRect = aFocusRect;
    }

// ---------------------------------------------------------------------------
// CESMRField::GetFocusType()
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRFieldFocusType CESMRField::GetFocusType() const
    {
    return iFocusType;
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFocusType()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFocusType( TESMRFieldFocusType aFocusType )
    {
    iFocusType = aFocusType;
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        iEventQueue->NotifyEventL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventL( TInt aCommand )
    {
    FUNC_LOG;
    CESMRFieldCommandEvent* event = CESMRFieldCommandEvent::NewLC( this,
                                                                   aCommand );
    NotifyEventL( *event );
    CleanupStack::PopAndDestroy( event );
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventAsyncL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventAsyncL( MESMRFieldEvent* aEvent )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        iEventQueue->NotifyEventAsyncL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::NotifyEventAsyncL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::NotifyEventAsyncL( TInt aCommand )
    {
    FUNC_LOG;
    CESMRFieldCommandEvent* event = CESMRFieldCommandEvent::NewLC( this,
                                                                   aCommand );
    NotifyEventAsyncL( event );
    CleanupStack::Pop( event );
    }

// ---------------------------------------------------------------------------
// CESMRField::RestoreMiddleSoftKeyL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::RestoreMiddleSoftKeyL()
    {
    FUNC_LOG;
    if ( iMskVisible != iDefaultMskVisible )
        {
        SetMiddleSoftKeyVisible( iDefaultMskVisible );
        }
    
    if ( iCustomMsk )
        {
        NotifyEventL( EESMRCmdRestoreMiddleSoftKey );
        iCustomMsk = EFalse;
        }
    }

EXPORT_C void CESMRField::SetMiddleSoftKeyVisible( TBool aVisible )
    {
    FUNC_LOG;
    if ( iMskVisible != aVisible )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if ( cba )
            {
            cba->MakeCommandVisibleByPosition(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    aVisible );
            iMskVisible = aVisible; // Visibility has been changed
            }
        }
    
    iCustomMsk = ETrue; // Field has modified editor default MSK                
    }

// ---------------------------------------------------------------------------
// CESMRField::HandleFieldEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::HandleFieldEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    if ( aEvent.Type() == MESMRFieldEvent::EESMRFieldCommandEvent )
        {
        TInt* command = static_cast<TInt*>( aEvent.Param( 0 ) );
        if ( EESMRCmdSizeExceeded != *command )
        	{
            ExecuteGenericCommandL( *command );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRField::EventObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C MESMRFieldEventObserver* CESMRField::EventObserver() const
    {
    FUNC_LOG;
    return const_cast< CESMRField* >( this );
    }

// ---------------------------------------------------------------------------
// CESMRField::SetFieldMode()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRField::SetFieldMode( TESMRFieldMode aMode )
    {
    iFieldMode = aMode;
    }

// ---------------------------------------------------------------------------
// CESMRField::FieldMode()
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRFieldMode CESMRField::FieldMode() const
    {
    return iFieldMode;
    }
// EOF

