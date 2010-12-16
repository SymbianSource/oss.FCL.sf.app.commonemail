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
* Description:  ESMR subject field implementation
 *
*/

#include "cesmrsubjectfield.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"
#include "cesmreditor.h"
#include "mesmrtitlepaneobserver.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include "nmrcolormanager.h"
#include "nmrbitmapmanager.h"
#include "mesmrlistobserver.h"

#include <calentry.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include <esmrgui.rsg>
#include <AknsBasicBackgroundControlContext.h>
#include <eikmfne.h>

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSubjectField::CESMRSubjectField
// ---------------------------------------------------------------------------
//
CESMRSubjectField::CESMRSubjectField()
    {
    FUNC_LOG;
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::~CESMRSubjectField
// ---------------------------------------------------------------------------
//
CESMRSubjectField::~CESMRSubjectField( )
    {
    FUNC_LOG;
    delete iFieldIcon;
    delete iBgControlContext;
    delete iPriorityIcon;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::ConstructL( TESMREntryFieldId aId, TInt aTextId,
        NMRBitmapManager::TMRBitmapId aIconID )
    {
    FUNC_LOG;
    SetFieldId( aId );

    iFieldIcon = CMRImage::NewL( aIconID );
    iFieldIcon->SetParent( this );
    
    iSubject = CESMREditor::NewL ( this, 1, KMaxTextLength,
            CEikEdwin::EResizable | CEikEdwin::EAvkonEditor | EAknEditorFlagNoLRNavigation);
    iSubject->SetEdwinSizeObserver ( this );
    iSubject->SetEdwinObserver( this );
    iSubject->SetParent( this );

    CESMRField::ConstructL( iSubject );  // iSubject ownership transfered

    HBufC* buf = StringLoader::LoadLC ( aTextId );
    iSubject->SetDefaultTextL( buf ); // ownership transferred
    CleanupStack::Pop( buf );
    
    // Setting background instead of theme skin  
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( 
            NMRBitmapManager::EMRBitmapInputCenter );

    TRect initialisationRect( 0, 0, 0, 0 );
    iBgControlContext = 
                CAknsBasicBackgroundControlContext::NewL( 
                        bitmapStruct.iItemId,
                        initialisationRect, 
                        EFalse );
                
    iSubject->SetSkinBackgroundControlContextL( iBgControlContext );
    
    iCurrentPriority = EFSCalenMRPriorityNormal;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::NewL
// ---------------------------------------------------------------------------
//
CESMRSubjectField* CESMRSubjectField::NewL( TFieldType aType )
    {
    FUNC_LOG;
    CESMRSubjectField* self = new( ELeave )CESMRSubjectField;
    CleanupStack::PushL ( self );
    if ( aType == ETypeSubject )
        {
        self->ConstructL( EESMRFieldSubject,
        R_QTN_MEET_REQ_SUBJECT_FIELD,
        NMRBitmapManager::EMRBitmapSubject );
        }
    else
        {
        self->ConstructL( EESMRFieldOccasion,
        R_QTN_CALENDAR_ANNIVERSARY_TYPE_OCCASION,
        NMRBitmapManager::EMRBitmapOccasion );
        }
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRSubjectField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );
    
    TRect fieldRect = 
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect editorRect( NMRLayoutManager::GetLayoutText( 
       fieldRect, 
       NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );
    
    if( iPriorityIcon && ( iCurrentPriority != EFSCalenMRPriorityNormal ) )
        {
		editorRect = NMRLayoutManager::GetLayoutText( 
			   fieldRect, 
			   NMRLayoutManager::EMRTextLayoutSingleRowEditorText ).TextRect();
        }
 
    // Adjust field size so that there's room for expandable editor.
    fieldRect.Resize( 0, iSize.iHeight - editorRect.Height() );
    
    return fieldRect.Size();
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::InitializeL()
    {
    FUNC_LOG;
    // Do nothing
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
    
    if( entry.PriorityL() != EFSCalenMRPriorityNormal &&
            entry.EntryTypeL() == CCalEntry::EAppt )
        {
        SetPriorityIconL( entry.PriorityL() );
        }

    // this is needed to be re-called here, otherwise the CEikEdwin
    // does not get correctly instantiated with default text
    iSubject->FocusChanged( EDrawNow );

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
    
    CCalEntry& entry = aEntry.Entry();
    
    if ( subject )
        {
        CleanupStack::PushL ( subject );

        // externalize the text only if it differs from the
        // default text. In other words, default text is not
        // externalized.
        if ( iSubject->DefaultText().Compare (*subject )!= 0 )
            {
            entry.SetSummaryL( *subject );
            }

        CleanupStack::PopAndDestroy( subject );
        }
    else
        {
        entry.SetSummaryL( KNullDesC );
        }

    entry.SetPriorityL( iCurrentPriority );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    if ( aFocus ) //Focus is gained on the field
        {
        ChangeMiddleSoftKeyL( EESMRCmdSaveMR,R_QTN_MSK_SAVE );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CESMRSubjectField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    TBool reDraw( EFalse );
    
    if( iSize != aSize )
        {
        // Let's save the required size for the iSubject
        iSize = aSize;
            
        if ( iObserver && aEdwin == iSubject )
           {
           iObserver->ControlSizeChanged( this );
           reDraw = ETrue;
           }
        
        if( iSubject->LineCount() != iLineCount )
            {
            // Line count has changed, the whole component needs
            // to be redrawn
            DrawDeferred();
            iLineCount = iSubject->LineCount();
            }
        }
    return reDraw;
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
// CESMRSubjectField::GetCursorLineVerticalPos
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower)
    {
    aLower = iSubject->CurrentLineNumber() * iSubject->RowHeight();
    aUpper = aLower - iSubject->RowHeight();
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
TBool CESMRSubjectField::ExecuteGenericCommandL(
        TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    switch ( aCommand )
        {
        case EESMRCmdPriorityHigh:
            {
            SetPriorityIconL( EFSCalenMRPriorityHigh );
            isUsed = ETrue;
            break;
            }
        case EESMRCmdPriorityNormal:
            {
            SetPriorityIconL( EFSCalenMRPriorityNormal );
            isUsed = ETrue;
            break;
            }
        case EESMRCmdPriorityLow:
            {
            SetPriorityIconL( EFSCalenMRPriorityLow );
            isUsed = ETrue;
            break;
            }
            
        case EESMRCmdSizeExceeded:
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
            isUsed = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRSubjectField::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iFieldIcon )
        {
        ++count;
        }

    if ( iSubject )
        {
        ++count;
        }
    
    if ( iPriorityIcon )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRSubjectField::ComponentControl( 
        TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFieldIcon;
        case 1:
        	if( iPriorityIcon )
        		{
        	    return iPriorityIcon;          
        		}
        	else
        		{
        	    return iSubject;                       
        		}
        case 2:
        	if( iPriorityIcon )
        		{
        	    return iSubject;       
        		}
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::SizeChanged( )
    {
    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect( 
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect rowRect( rowLayoutRect.Rect() );
    
    // Layouting field icon
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout( 
            NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutSingleRowAColumnGraphic ) );
        AknLayoutUtils::LayoutControl( iFieldIcon, rowRect, iconLayout );
        }
    
    // Layouting priority icon
    if( iPriorityIcon )
        {
		if( iCurrentPriority != EFSCalenMRPriorityNormal )
			{
			TAknWindowComponentLayout iconLayout(
					NMRLayoutManager::GetWindowComponentLayout(
						NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
			AknLayoutUtils::LayoutImage( iPriorityIcon, rowRect, iconLayout );
			}
        }
	else
		{
		delete iPriorityIcon;
		iPriorityIcon = NULL;			
		}
    
    TAknLayoutText editorLayoutText;

    if( iPriorityIcon && ( iCurrentPriority != EFSCalenMRPriorityNormal ) )
        {
        editorLayoutText = NMRLayoutManager::GetLayoutText( rowRect, 
                    NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
        }
    else
        {
        editorLayoutText = NMRLayoutManager::GetLayoutText( rowRect, 
                    NMRLayoutManager::EMRTextLayoutTextEditor );
        }
    
    // Layouting editor field
    TRect editorRect( editorLayoutText.TextRect() );

    // Resize height according to actual height required by edwin.
    editorRect.Resize( 0, iSize.iHeight - editorRect.Height() );
    iSubject->SetRect( editorRect );  
    
    // Try setting font. Failures are ignored.
    TRAP_IGNORE( iSubject->SetFontL( editorLayoutText.Font() ) );
    
    // Layouting focus
    TRect bgRect( TPoint( editorRect.iTl.iX, editorRect.iTl.iY ), 
            editorRect.Size() );
    
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );    

    if ( !iOutlineFocus )
        {
        iObserver->RedrawField( *this );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iSubject->SetContainerWindowL( aContainer );
    
    iSubject->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRSubjectField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    response = iSubject->OfferKeyEventL ( aEvent, aType );

    if ( aType == EEventKey && 
    		( aEvent.iScanCode != EStdKeyUpArrow &&
			  aEvent.iScanCode != EStdKeyDownArrow ))
    	{       
        iSubject->DrawDeferred();
    	}
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRSubjectField::SetPriorityIconL
// ---------------------------------------------------------------------------
//
void CESMRSubjectField::SetPriorityIconL( TUint aPriority )
    {
    switch ( aPriority )
        {
        case EFSCalenMRPriorityHigh:
            {
            if( iPriorityIcon )
            	{
				delete iPriorityIcon;
				iPriorityIcon = NULL;
            	}
            
            iPriorityIcon = CMRImage::NewL( 
                  NMRBitmapManager::EMRBitmapPriorityHigh,
                  this,
                  ETrue );
            iPriorityIcon->SetParent( this );
            if( iCurrentPriority == EFSCalenMRPriorityNormal )
            	{
				iCurrentPriority = EFSCalenMRPriorityHigh;
				SizeChanged();
            	}
            iCurrentPriority = EFSCalenMRPriorityHigh;
            break;
            }
          
        case EFSCalenMRPriorityNormal:
            {
            delete iPriorityIcon;
            iPriorityIcon = NULL;
            if( iCurrentPriority != EFSCalenMRPriorityNormal )
				{
				SizeChanged();
				}
            iCurrentPriority = EFSCalenMRPriorityNormal;
            break;
            }
          
        case EFSCalenMRPriorityLow:
            {
            if( iPriorityIcon )
            	{
				delete iPriorityIcon;
				iPriorityIcon = NULL;
            	}
            
            iPriorityIcon = CMRImage::NewL( 
                  NMRBitmapManager::EMRBitmapPriorityLow );
            iPriorityIcon->SetParent( this );  
            if( iCurrentPriority == EFSCalenMRPriorityNormal )
				{
				iCurrentPriority = EFSCalenMRPriorityLow;
				SizeChanged();
				}
            iCurrentPriority = EFSCalenMRPriorityLow;
            break;
            }
        default:
            {
            break;
            }
        }
    }

// EOF
