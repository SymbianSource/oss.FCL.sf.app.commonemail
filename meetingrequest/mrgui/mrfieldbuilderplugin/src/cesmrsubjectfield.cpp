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
* Description:  ESMR subject field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrsubjectfield.h"

#include <calentry.h>
#include <StringLoader.h>
#include <gulfont.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include "esmrfieldbuilderdef.h"

#include <esmrgui.rsg>

#include "cesmreditor.h"
#include "mesmrtitlepaneobserver.h"
#include "cesmrglobalnote.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSubjectField::CESMRSubjectField
// ---------------------------------------------------------------------------
//
CESMRSubjectField::CESMRSubjectField( )
:   CESMRIconField()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::~CESMRSubjectField
// ---------------------------------------------------------------------------
//
CESMRSubjectField::~CESMRSubjectField( )
    {
    FUNC_LOG;
    delete iFrameBgContext;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::ConstructL( TESMREntryFieldId aId, TInt aTextId,
        TAknsItemID aIconID )
    {
    FUNC_LOG;
    SetFieldId ( aId );
    SetExpandable ( );

    iSubject = CESMREditor::NewL ( this, 1, KMaxTextLength,
            CEikEdwin::EResizable | CEikEdwin::EAvkonEditor | EAknEditorFlagNoLRNavigation);
    iSubject->SetEdwinSizeObserver ( this );
    iSubject->SetEdwinObserver( this );

    HBufC* buf = StringLoader::LoadLC ( aTextId );
    iSubject->SetDefaultTextL( buf ); // ownership transferred
    CleanupStack::Pop( buf );

    iBackground = AknsDrawUtils::ControlContext( this );

    CESMRIconField::ConstructL ( aIconID, iSubject );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::NewL
// ---------------------------------------------------------------------------
//
CESMRSubjectField* CESMRSubjectField::NewL( TFieldType aType )
    {
    FUNC_LOG;
    CESMRSubjectField* self = new (ELeave) CESMRSubjectField;
    CleanupStack::PushL ( self );
    if ( aType == ETypeSubject )
        {
        self->ConstructL (EESMRFieldSubject,
        R_QTN_MEET_REQ_SUBJECT_FIELD,
        KAknsIIDQgnFscalIndiSubject );
        }
    else
        {
        self->ConstructL (EESMRFieldOccasion,
        R_QTN_CALENDAR_ANNIVERSARY_TYPE_OCCASION,
        KAknsIIDQgnFscalIndiOccasion );
        }
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::InitializeL()
    {
    FUNC_LOG;
    iSubject->SetFontL( iLayout->Font( iCoeEnv, iFieldId ), iLayout );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry ( );
    HBufC* subject = entry.SummaryL().AllocLC ( );

    // if there is no text available, don't overwrite the default text
    // with empty descriptor.
    if ( subject->Length ( )> 0 )
        {
        iSubject->ClearSelectionAndSetTextL ( *subject );
        if ( iTitlePaneObserver )
            {
            iTitlePaneObserver->UpdateTitlePaneTextL( *subject );
            }
        }

    // this is needed to be re-called here, otherwise the CEikEdwin
    // does not get correctly instantiated with default text
    iSubject->FocusChanged(EDrawNow);

    CleanupStack::PopAndDestroy ( subject );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* subject = iSubject->GetTextInHBufL ( );

    if ( subject )
        {
        CleanupStack::PushL ( subject );

        // externalize the text only if it differs from the
        // default text. In other words, default text is not
        // externalized.

       if ( iSubject->DefaultText().Compare (*subject )!= 0 )
            {
            CCalEntry& entry = aEntry.Entry ( );
            entry.SetSummaryL ( *subject );
            }

       CleanupStack::PopAndDestroy ( subject );
        }
    else
        {
        CCalEntry& entry = aEntry.Entry ( );
        entry.SetSummaryL( KNullDesC );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    if (aFocus) //Focus is gained on the field
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ExpandedHeight
// ---------------------------------------------------------------------------
//
TInt CESMRSubjectField::ExpandedHeight( ) const
    {
    FUNC_LOG;
    TInt height = iLayout->FieldSize( FieldId() ).iHeight;
    if( iSize.iHeight < height )
        {
        return height - KEditorDifference;
        }
    else
        {
        return iSize.iHeight;
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRSubjectField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    iSize = aSize;
    iSize.iHeight -= KEditorDifference;

    if (iLayout->CurrentFontZoom() == EAknUiZoomSmall ||
        iLayout->CurrentFontZoom() == EAknUiZoomVerySmall)
        {
        iSize.iHeight -= KEditorDifference;
        }

    if ( iObserver )
        {
        iObserver->ControlSizeChanged ( this );
        }

    if( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iSubject->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::HandleEdwinEventL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::HandleEdwinEventL( CEikEdwin *aEdwin, TEdwinEvent aEventType )
    {
    FUNC_LOG;
    if ( iTitlePaneObserver &&
         aEdwin == iSubject &&
         aEventType == EEventTextUpdate )
        {
        HBufC* text = iSubject->GetTextInHBufL();
        if ( text )
            {
            CleanupStack::PushL( text );
            
            iTitlePaneObserver->UpdateTitlePaneTextL( *text );
            
            TInt textLength( text->Length() );
            if ( iSubject->GetLimitLength() <= textLength )
            	{
            	NotifyEventAsyncL( EESMRCmdSizeExceeded );
            	}  
            
            CleanupStack::PopAndDestroy( text );
            }
        else
            {
            iTitlePaneObserver->UpdateTitlePaneTextL( KNullDesC );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::SetTitlePaneObserver
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver )
    {
    FUNC_LOG;
    iTitlePaneObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::PositionChanged()
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::PositionChanged()
    {
    FUNC_LOG;
    CCoeControl::PositionChanged();
    if( iFrameBgContext )
        {
        TRect visibleRect = CalculateVisibleRect( iSubject->Rect() );
        iFrameBgContext->SetFrameRects( visibleRect, visibleRect );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::GetVerticalFocusPosition
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower)
    {
    FUNC_LOG;
    aLower = iSubject->CurrentLineNumber() * iSubject->RowHeight();
    aUpper = aLower - iSubject->RowHeight();
    }
// ---------------------------------------------------------------------------
// CESMRSubjectField::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::ActivateL()
    {
    FUNC_LOG;
    CCoeControl::ActivateL();
    TRect rect(TPoint(iSubject->Position()), iSubject->Size());
    TRect inner(rect);
    TRect outer(rect);

    iSubject->SetSize( TSize( iSubject->Size().iWidth, iLayout->FieldSize( FieldId() ).iHeight ));

    delete iFrameBgContext;
    iFrameBgContext = NULL;
    iFrameBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrInput, outer, inner, EFalse ) ;

    iFrameBgContext->SetParentContext( iBackground );
    iSubject->SetSkinBackgroundControlContextL(iFrameBgContext);

    // update base class rects and redraw:
    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::ListObserverSet()
    {
    FUNC_LOG;
    iSubject->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::ExecuteGenericCommandL( 
		TInt aCommand )
	{
    FUNC_LOG;
	if ( EESMRCmdSizeExceeded == aCommand )
		{
		CESMRGlobalNote::ExecuteL ( 
				CESMRGlobalNote::EESMRCannotDisplayMuchMore );

		HBufC* text = iSubject->GetTextInHBufL();
		CleanupDeletePushL( text );
		if ( text )
			{    
	    	TInt curPos = iSubject->CursorPos();    	
	    	if( curPos > iSubject->GetLimitLength() - 1 )
	    		curPos = iSubject->GetLimitLength() - 1;
	    	HBufC* newText = 
	    		text->Des().Mid( 0, iSubject->GetLimitLength() - 1 ).AllocLC();    	
	    	
	    	iSubject->SetTextL ( newText );
	    	CleanupStack::PopAndDestroy( newText );
	    	newText = NULL;
	    	
	    	iSubject->SetCursorPosL (curPos, EFalse );
	    	iSubject->HandleTextChangedL();
	    	iSubject->UpdateScrollBarsL();
	    	SetFocus(ETrue);
			}
		CleanupStack::PopAndDestroy( text );	
		}
	}
// EOF
