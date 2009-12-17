/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  List pane component for UI fields
*
*/
#include "cmrlistpane.h"

#include <eikscrlb.h>
#include <AknUtils.h>
#include <touchfeedback.h>

#include "mesmrfieldstorage.h"
#include "cesmrfield.h"

#include "emailtrace.h"

namespace // codescanner::namespace
    {
    /**
     * Vertical scroll margin
     */
    const TInt KVerticalScrollMargin = 3;

    // ---------------------------------------------------------------------------
    // IndexByFieldId
    // ---------------------------------------------------------------------------
    //
    TInt IndexByFieldId( const MESMRFieldStorage& aFactory,
                         TESMREntryFieldId aFieldId )
        {
        TInt index( KErrNotFound );
        TInt count( aFactory.Count() );
        
        for ( TInt i = 0; i < count; ++i )
            {
            if ( aFactory.Field( i )->FieldId() == aFieldId )
                {
                index = i;
                break;
                }
            }
        
        return index;
        }
    }

//----- MEMBER FUNCTIONS ----

// ---------------------------------------------------------------------------
// CMRListPane::NewL
// ---------------------------------------------------------------------------
//
CMRListPane* CMRListPane::NewL( const CCoeControl& aParent,
                                MESMRFieldStorage& aFactory,
                                TAknDoubleSpanScrollBarModel& aScrollModel )
    {
    CMRListPane* self = new (ELeave) CMRListPane( aFactory, aScrollModel );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRListPane::~CMRListPane
// ---------------------------------------------------------------------------
//
CMRListPane::~CMRListPane()
    {
    }

// ---------------------------------------------------------------------------
// CMRListPane::CMRListPane
// ---------------------------------------------------------------------------
//
CMRListPane::CMRListPane( MESMRFieldStorage& aFactory,
                          TAknDoubleSpanScrollBarModel& aScrollModel )
    : iFactory( aFactory ),
      iScrollModel( aScrollModel )
    {
    }

// ---------------------------------------------------------------------------
// CMRListPane::ConstructL
// ---------------------------------------------------------------------------
//
void CMRListPane::ConstructL( const CCoeControl& aParent )
    {
    SetContainerWindowL( aParent );
    TBool focusSet = EFalse;
    const TInt count( iFactory.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        iFactory.Field(i)->SetContainerWindowL( *this );
        }

    for ( TInt i = 0; i < count; i++ )
        {
        CESMRField* field = iFactory.Field( i );
        
        // Initialize field
        field->InitializeL();
        User::LeaveIfError( field->SetParent( this ) );

        if ( !focusSet
             && field->IsVisible()
             && !field->IsNonFocusing())
            {
            field->SetOutlineFocusL( ETrue );
            focusSet = ETrue;
            iFocus = i;
            }
        }    
    }

// ---------------------------------------------------------------------------
// CMRListPane::FocusedItem
// ---------------------------------------------------------------------------
//
CESMRField* CMRListPane::FocusedItem() const
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    if ( iFactory.Count() > 0 )
        {
        field = iFactory.Field( iFocus );
        }
    return field;
    }
        
// ---------------------------------------------------------------------------
// CMRListPane::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRListPane::CountComponentControls() const
    {
    return iFactory.Count();
    }

// ---------------------------------------------------------------------------
// CMRListPane::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRListPane::ComponentControl( TInt aIndex ) const
    {
    return iFactory.Field( aIndex );
    }

// ---------------------------------------------------------------------------
// CMRListPane::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRListPane::SizeChanged()
    {
    TPoint tl( Position() );
    TInt scrollSpan = 0;
    // Loop all the visible fields and set size and position in the list
    TBool topVisibleFound( EFalse );
    
    const TInt count( iFactory.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        CESMRField* field = iFactory.Field( i );

        if ( field->IsVisible() )
            {
            LayoutField( *field, tl );
            
            if ( !topVisibleFound )
                {
                iTopVisibleIndex = i;
                topVisibleFound = ETrue;
                }

            TInt height = field->Size().iHeight;
            tl.iY += height;
            scrollSpan += height;
            }
        }
    
    iScrollModel.SetScrollSpan( scrollSpan );
    iScrollModel.SetWindowSize( iSize.iHeight );
    UpdateFocusPosition();
    }

// ---------------------------------------------------------------------------
// CMRListPane::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRListPane::OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                          TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );
    
    // If we have at least one item in the list
    if ( iFocus < iFactory.Count() )
        {
        CESMRField* field = iFactory.Field( iFocus );
        // First check if the focused item needs the key event
        response = field->OfferKeyEventL( aKeyEvent, aType );

        if ( aType == EEventKey
             && response == EKeyWasNotConsumed )
            {
            // Check if the focus should be changed
            switch ( aKeyEvent.iScanCode )
                {
                case EStdKeyUpArrow:
                    {
                    response = MoveFocusUpL();
                    break;
                    }
                case EStdKeyDownArrow:
                    {
                    response = MoveFocusDownL();
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CMRListPane::ControlSizeChanged
// ---------------------------------------------------------------------------
//
void CMRListPane::ControlSizeChanged( CESMRField* aCtrl )
    {
    if ( !aCtrl )
        {
        SizeChanged();
        }
    else
        {
        TPoint tl( aCtrl->Position() );
        TInt index = IndexByFieldId( iFactory, aCtrl->FieldId() );
        
        // Relayout aCtrl
        TSize old( aCtrl->Size() );
        TSize size( aCtrl->MinimumSize() );
        if ( aCtrl->IsExpandable() )
            {
            size.iHeight = aCtrl->ExpandedHeight();
            }
        aCtrl->SetSize( size );
        iScrollModel.SetScrollSpan( iScrollModel.ScrollSpan()
                                    + size.iHeight - old.iHeight );
        
        // Move other fields
        ++index;
        tl.iY += size.iHeight;
        MoveFields( index, tl );
        UpdateFocusPosition();
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::InsertControl
// ---------------------------------------------------------------------------
//
void CMRListPane::InsertControl( TESMREntryFieldId aField )
    {
    CESMRField* field = iFactory.FieldById( aField );
    
    if ( field && !field->IsVisible() )
        {
        // Make field visible 
        field->MakeVisible( ETrue );
        TInt index = IndexByFieldId( iFactory, aField );
        if ( index < iTopVisibleIndex )
            {
            iTopVisibleIndex = index;
            }
        
        TPoint tl( Rect().iTl );
        TInt prevIndex = index - 1;
        
        // Get previous visible field position
        if ( prevIndex >= 0 )
            {
            CESMRField* previous = NULL;
            do
                {
                previous = iFactory.Field( prevIndex-- );
                }
            while ( prevIndex >= 0 && !previous->IsVisible() );
            
            tl.iY = previous->Rect().iBr.iY;
            }
        
        // Layout field
        LayoutField( *field, tl );
        iScrollModel.SetScrollSpan( iScrollModel.ScrollSpan()
                                    + field->Size().iHeight );
        
        // Move following fields
        tl.iY += field->Size().iHeight;
        MoveFields( ++index, tl );
        UpdateFocusPosition();
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::InsertControl
// ---------------------------------------------------------------------------
//
void CMRListPane::RemoveControl( TESMREntryFieldId aField )
    {
    CESMRField* field = iFactory.FieldById( aField );
        
    if ( field && field->IsVisible() )
        {
        field->MakeVisible( EFalse );
        iScrollModel.SetScrollSpan( iScrollModel.ScrollSpan() - field->Size().iHeight );
        TInt index = IndexByFieldId( iFactory, aField );
        TBool focused = ( index == iFocus )? ETrue : EFalse;
        
        if ( index == iTopVisibleIndex )
            {
            // Find next
            TInt count( iFactory.Count() );
            while ( index < count )
                {
                if ( iFactory.Field( ++index )->IsVisible() )
                    {
                    iTopVisibleIndex = index;
                    break;
                    }
                }
            }
        else
            {
            ++index;
            }
        TPoint pos( field->Position() );
        MoveFields( index, pos );
        if ( focused && !field->IsNonFocusing() )
            {
            TRAP_IGNORE( DoSetFocusL( index ) )
            }
        UpdateFocusPosition();
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::IsControlVisible
// ---------------------------------------------------------------------------
//
TBool CMRListPane::IsControlVisible( TESMREntryFieldId aField )
    {
    TBool visible( EFalse );
    CESMRField* field = iFactory.FieldById( aField );
    
    if ( field )
        {
        visible = field->IsVisible();
        }
    
    return visible;
    }

// ---------------------------------------------------------------------------
// CMRListPane::SetControlFocusedL
// ---------------------------------------------------------------------------
//
void CMRListPane::SetControlFocusedL( TESMREntryFieldId aField )
    {
    TInt count = iFactory.Count();
    for ( TInt i(0); i < count; i++ )
        {
        CESMRField* field = iFactory.Field(i);
        
        if ( field->FieldId() == aField )
            {
            if( field->IsVisible() && !field->IsNonFocusing() )
                {
                // Remove current focus before setting new one
                CESMRField* focusedField = FocusedItem();
                ASSERT( focusedField );
                focusedField->SetOutlineFocusL( EFalse );

                // Set new focus
                field->SetOutlineFocusL( ETrue );
                iFocus = i;
                break;
                }
            else
                {
                // Not possible to set focus to non-visible control
                User::Leave( KErrGeneral );
                }
            }
        }
    }

// SCROLLING_MOD: These two methods are for CESMRRichTextViewer to 
// be able to control view area
// ---------------------------------------------------------------------------
// CMRListPane::MoveListAreaDownL
// ---------------------------------------------------------------------------
//
void CMRListPane::MoveListAreaDownL( TInt aAmount )
    {
    FUNC_LOG;
    // Method for CESMRRichTextViewer for moving view area down if the text in
    // text field does not fit on the screen.

    // Fetch the position info about the first visible field in field set:
    CESMRField* field;
    if( iTopVisibleIndex != KErrNotFound )
        {
        field = iFactory.Field( iTopVisibleIndex );
        }
    else
        {
        field = iFactory.Field( iFactory.Count() - 1 );
        }
    TRect rect( field->Rect() );

    // check whether the first field is visible, if not, let's
    // check should we scroll less than needed to get the first
    // field shown.
    if ( rect.iTl.iY + aAmount > Rect().iTl.iY )
        {
        aAmount = Rect().iTl.iY - rect.iTl.iY + KVerticalScrollMargin;
        }
    else if ( rect.iTl.iY == Rect().iTl.iY )
        {
        // field already visible, do not scroll.
        aAmount = 0;
        }

    // Do the view scrolling if needed:
    if ( aAmount )
        {
        ScrollItemsDown( aAmount );
        UpdateFocusPosition();
        //UpdateScrollBar();
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::MoveListAreaUpL
// ---------------------------------------------------------------------------
//
void CMRListPane::MoveListAreaUpL( TInt aAmount )
    {
    FUNC_LOG;
    // Fetch the position information about currently focused field:
    CESMRField* field = iFactory.Field( iFocus );
    TRect rect( field->Rect() );

    // check whether the scroll pixels should be less than normal
    // scroll sequence to let the field bottom to be placed at
    // the bottom of view rect.
    if ( rect.iBr.iY - aAmount < Rect().iBr.iY )
        {
        aAmount = rect.iBr.iY - Rect().iBr.iY + KVerticalScrollMargin;
        }
    else if ( rect.iBr.iY + aAmount == Rect().iBr.iY )
        {
        // field already visible, do not scroll.
        aAmount = 0;
        }

    // do the scrolling if needed:
    if ( aAmount > 0 )
        {
        // if the focus is on last
        ScrollItemsUp( aAmount );
        UpdateFocusPosition();
        //UpdateScrollBar();
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::ListHeight
// ---------------------------------------------------------------------------
//
TInt CMRListPane::ListHeight()
    {
    FUNC_LOG;
    return iSize.iHeight;
    }

// ---------------------------------------------------------------------------
// CMRListPane::IsFieldBottomVisible
// ---------------------------------------------------------------------------
//
TBool CMRListPane::IsFieldBottomVisible()
    {
    // Fetch the position information about currently focused field:
    CESMRField* field = iFactory.Field( iFocus );
    TBool ret( EFalse );
    if ( field->Rect().iBr.iY <= Rect().iBr.iY )
       {
       ret = ETrue;
       }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRListPane::MoveFocusUpL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRListPane::MoveFocusUpL()
    {
    TInt ind( iFocus );
    
    // search next visible focus item
    while ( ind > 0 )
        {
        CESMRField* field = iFactory.Field( --ind );

        if ( !field->IsNonFocusing() )
            {
            field = iFactory.Field( ind );

            CESMRField* focusedField = iFactory.Field( iFocus );
            if ( field->IsVisible() )
                {
                TBool canLoseFocus( 
                        focusedField->OkToLoseFocusL( field->FieldId() ) );

                if ( canLoseFocus )
                    {
                    iFocus = ind;
                                        
                    // update focus index and scroll the item visible
                    focusedField->SetOutlineFocusL( EFalse );
                    // This call changes the text color of previously 
                    // focused field
                    focusedField->SetFocus( EFalse );
                    field->SetOutlineFocusL( ETrue );
                    // This call changes the text color of focused field
                    field->SetFocus( ETrue );

                    ScrollItemVisible( iFocus );
                    }
                return EKeyWasConsumed;
                }
            }
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CMRListPane::MoveFocusDownL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRListPane::MoveFocusDownL()
    {
    TInt ind( iFocus );

    // start searching next possible focus item
    TInt maxItemIndex = iFactory.Count() - 1;
    
    while ( ind < maxItemIndex )
        {
        // only visible and focusable items can be focused
        CESMRField* field = iFactory.Field( ++ind );

        if ( field->IsVisible() && !field->IsNonFocusing() )
            {
            CESMRField* focusedField = iFactory.Field( iFocus );
                    
            TBool canLoseFocus( 
                    focusedField->OkToLoseFocusL( field->FieldId() ) );

            // check it its ok for the old focus item to lose focus
            if ( canLoseFocus )
                {
                iFocus = ind;
                                
                // update focus index and scroll the item visible
                focusedField->SetOutlineFocusL( EFalse );
                // This call changes the text color of previously 
                // focused field
                focusedField->SetFocus( EFalse );
                field->SetOutlineFocusL( ETrue );
                // This call changes the text color of focused field
                field->SetFocus( ETrue );

                ScrollItemVisible( iFocus );
                }
            return EKeyWasConsumed;
            }
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CMRListPane::MoveFocusL
// ---------------------------------------------------------------------------
//
TBool CMRListPane::MoveFocusL(TInt aNextFieldIndex)
    {
    // start searching next possible focus item
    TInt maxItemIndex = iFactory.Count() - 1;

    CESMRField* focusedField = iFactory.Field( iFocus );
    
    // only visible and focusable items can be focused
    CESMRField* field = iFactory.Field( aNextFieldIndex );

    if ( field->IsVisible() && !field->IsNonFocusing() )
        {
        TBool canLoseFocus( 
                focusedField->OkToLoseFocusL( field->FieldId() ) );

        // check it its ok for the old focus item to lose focus
        if ( canLoseFocus )
            {
            // tactile feedback if touch 
            if ( AknLayoutUtils::PenEnabled() )
                {
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( this, ETouchFeedbackBasic );
                    }
                }
            
            iFocus = aNextFieldIndex;
                            
            // update focus index and scroll the item visible
            focusedField->SetOutlineFocusL( EFalse );
            // This call changes the text color of previously 
            // focused field
            focusedField->SetFocus( EFalse );
            field->SetOutlineFocusL( ETrue );
            // This call changes the text color of focused field
            field->SetFocus( ETrue );
            DrawDeferred();
            }
        return ETrue; // operation succeed
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CMRListPane::LayoutField
// ---------------------------------------------------------------------------
//
void CMRListPane::LayoutField( CESMRField& aField,
                               const TPoint& aTl )
    {
    TSize size( aField.MinimumSize() );
    if ( aField.IsExpandable() )
        {
        size.iHeight = aField.ExpandedHeight();
        }
    aField.SetPosition( aTl );
    aField.SetSize( size );
    }

// ---------------------------------------------------------------------------
// CMRListPane::MoveFields
// ---------------------------------------------------------------------------
//
void CMRListPane::MoveFields( TInt aIndex,
                              TPoint& aTl )
    {
    const TInt count( iFactory.Count() );
    
    for ( TInt i = aIndex; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );

        if ( field->IsVisible() )
            {
            field->SetPosition( aTl );
            
            aTl.iY += field->Size().iHeight;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::DoSetFocusL
// ---------------------------------------------------------------------------
//
void CMRListPane::DoSetFocusL( TInt aFocus )
    {
    TInt count( iFactory.Count() );
    aFocus = Max( 0, Min( aFocus, count - 1 ) );
    
    if ( aFocus != iFocus )
        {
        // Get current focused field
        CESMRField* old = iFactory.Field( iFocus );
        
        // Get next focused field
        CESMRField* field = iFactory.Field( aFocus );
        while ( aFocus < count && !field->IsVisible() )
            {
            field = iFactory.Field( aFocus++ );
            }
        
        if ( !field->IsVisible() )
            {
            aFocus = iFocus - 1;
            while ( aFocus > 0 && !field->IsVisible() )
                {
                field = iFactory.Field( aFocus-- );
                }
            }
        
        ASSERT( field->IsVisible() );
        
        // Remove focus from old
        iFocus = aFocus;
        // update focus index and scroll the item visible
        old->SetOutlineFocusL( EFalse );
        // Change the text color of previously focused field
        old->SetFocus( EFalse );
        field->SetOutlineFocusL( ETrue );
        // This call changes the text color of focused field
        field->SetFocus( ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::ScrollItemVisible
// ---------------------------------------------------------------------------
//
void CMRListPane::ScrollItemVisible( TInt aInd )
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    if ( aInd == KErrNotFound )
        {
        field = FocusedItem();
        }
    else
        {
        field = iFactory.Field( aInd );
        }

    ASSERT( field );

    TRect rect( field->Rect() );

    // move all items upwards
    TInt bottomRightY = Rect().iBr.iY;


    TInt fieldUpper(0);
    TInt fieldLower(0);
    field->GetMinimumVisibleVerticalArea( fieldUpper, fieldLower );

    // desired position below view rect:
    if ( rect.iTl.iY + fieldLower > Rect().iBr.iY )
        {
        // field rect Y position related to view rect:
        TInt fieldRelYPos = rect.iTl.iY - Size().iHeight;
        TInt px = fieldRelYPos + fieldLower;

        // if focus on first or last field: add margin height to
        // scroll amount.
        if ( iFocus == 0 || iFocus == iFactory.Count()-1 )
            {
            px += 2 * KVerticalScrollMargin;
            }

        ScrollItemsUp( px );
        }

    // move all items downwards.
    TInt topLeftY = Rect().iTl.iY;

    if ( rect.iBr.iY - (rect.Height() - fieldUpper ) < topLeftY )
        {
        TInt fieldRelYPos = topLeftY - rect.iBr.iY;
        TInt px = fieldRelYPos + ( rect.Height() - fieldUpper);

        // if focus on first or last field: add margin height to
        // scroll amount.
        if ( iFocus == 0 ||  iFocus == iFactory.Count()-1 )
            {
            px += KVerticalScrollMargin;
            }

        ScrollItemsDown( px );
        }

    UpdateFocusPosition();//UpdateScrollBar();
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRListPane::UpdateScrollBar
// ---------------------------------------------------------------------------
//
void CMRListPane::UpdateScrollBar()
    {
    FUNC_LOG;
    // Scroll span is the size of the scrolled list,
    // including the items that doesn't fit in the screen
    TInt spanSize( 0 );
    TInt hidden( 0 );
    const TInt count(iFactory.Count());
    for ( TInt i(0); i < count; i++ )
        {
        CESMRField* field = iFactory.Field( i );
        if ( field->IsVisible() )
            {
            TRect rect( field->Rect() );
            spanSize += rect.Height();
            // Check if the field's top Y-position is hidden.
            if ( rect.iTl.iY  < 0 )
                {
                // whole field is hidden
                if ( rect.iBr.iY < 0 )
                    {
                    hidden += rect.Height();
                    }
                // partly hidden:
                else
                    {
                    hidden += Abs( rect.iTl.iY );
                    }
                }
            }
        }

    iScrollModel.SetScrollSpan( spanSize );
    iScrollModel.SetWindowSize( iSize.iHeight );
    iScrollModel.SetFocusPosition( hidden );

    }

// ---------------------------------------------------------------------------
// CMRListPane::ScrollItemsUp
// ---------------------------------------------------------------------------
//
void CMRListPane::ScrollItemsUp( TInt aPx )
    {
    FUNC_LOG;
    TInt count( iFactory.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field(i);
        if ( field->IsVisible() )
            {
            TPoint pos( field->Position() );
            pos.iY -= aPx;
            if ( i == iTopVisibleIndex && pos.iY < 0 )
                {
                iTopVisibleIndex = KErrNotFound;
                }
            else if ( iTopVisibleIndex == KErrNotFound && pos.iY >= 0 )
                {
                iTopVisibleIndex = i;
                }
            field->SetPosition( pos );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::ScrollItemsDown
// ---------------------------------------------------------------------------
//
void CMRListPane::ScrollItemsDown( TInt aPx )
    {
    FUNC_LOG;
    TInt count( iFactory.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );
        if ( field->IsVisible() )
            {
            TPoint pos( field->Position() );
            pos.iY += aPx;
            if ( pos.iY >= 0 && ( i < iTopVisibleIndex || iTopVisibleIndex == KErrNotFound ) )
                {
                iTopVisibleIndex = i;
                }
            field->SetPosition( pos );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRListPane::ScrollView
// ---------------------------------------------------------------------------
//
void CMRListPane::ScrollView( TInt aAmount )
    {
    FUNC_LOG;
    
    if ( aAmount > 0 )
        {
        // move list up
        ScrollItemsUp( aAmount );
        UpdateFocusPosition();
        DrawDeferred();
        }
    else if ( aAmount < 0 )
        {
        // move list down
        ScrollItemsDown( -aAmount );
        UpdateFocusPosition();
        DrawDeferred();
        }
    }


// ---------------------------------------------------------------------------
// CMRListPane::UpdateFocusPosition
// ---------------------------------------------------------------------------
//
void CMRListPane::UpdateFocusPosition()
    {
    TInt focusPos = 0;
    TInt count = iFactory.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );
        if ( field->IsVisible() )
            {
            TRect rect( field->Rect() );
            // Check if the field's top is above scroll area origo.
            if ( rect.iTl.iY  < 0 )
                {
                // Whole field is above origo
                if ( rect.iBr.iY < 0 )
                    {
                    focusPos += rect.Height();
                    }
                // Part of the field is above origo
                else
                    {
                    focusPos += Abs( rect.iTl.iY );
                    }
                }
            else
                {
                // Rest of the fields are below scroll area origo
                break;
                }
            }
        }
    
    iScrollModel.SetFocusPosition( focusPos );
    }


// -----------------------------------------------------------------------------
// CMRListPane::GetViewCenterPosition
// -----------------------------------------------------------------------------
//
TPoint CMRListPane::GetViewCenterPosition() const
    {
    const TInt count( iFactory.Count() );
    TInt topFieldYPos( 0 );
    for ( TInt i = 0; i < count; i++ )
        {
        CESMRField* field = iFactory.Field( i );

        if ( field->IsVisible() )
            {
            topFieldYPos = field->Position().iY;    
            break;
            }
        }
    TInt centerX = iSize.iWidth / 2;
    TInt visibleHeight = iSize.iHeight;

    TInt centerY = visibleHeight / 2 - topFieldYPos;
    
    
    return TPoint( centerX, centerY );
    }


// -----------------------------------------------------------------------------
// CMRListPane::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CMRListPane::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        TInt count = iFactory.Count();
        for ( TInt i = 0; i < count; ++i )
            {
            CESMRField* field = static_cast<CESMRField*>(iFactory.Field( i ) );
            TRect r = field->Rect();
            TBool tapped = field->Rect().Contains( aPointerEvent.iPosition );
            if (tapped && field->IsVisible() && !field->IsFocused() )
                {
                MoveFocusL( i );
                break;
                }
            }
        }
    // here some fields can further adjust their state (for example, CESMRResponseField has sub-fields) 
    CCoeControl::HandlePointerEventL(aPointerEvent);
    }
// End of file

