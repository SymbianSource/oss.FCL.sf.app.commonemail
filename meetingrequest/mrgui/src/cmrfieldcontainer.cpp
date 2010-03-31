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
* Description:  Field container for UI fields
*
*/
#include "cmrfieldcontainer.h"
#include "mesmrfieldstorage.h"
#include "mmrfieldcontainerobserver.h"
#include "cesmrfield.h"

//DEBUG
#include "emailtrace.h"

namespace { // codescanner::namespace

/**
 * Vertical scroll margin
 */
const TInt KVerticalScrollMargin = 3;


// ----------------
// IndexByFieldId
// ----------------
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
// CMRFieldContainer::CMRFieldContainer
// ---------------------------------------------------------------------------
//
CMRFieldContainer::CMRFieldContainer( MESMRFieldStorage& aFactory )
    : iFactory( aFactory )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::~CMRFieldContainer
// ---------------------------------------------------------------------------
//
CMRFieldContainer::~CMRFieldContainer()
    {
    FUNC_LOG;
    // No implementation
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::NewL
// ---------------------------------------------------------------------------
//
CMRFieldContainer* CMRFieldContainer::NewL( 
        MESMRFieldStorage& aFactory, 
        const CCoeControl& aParent )
    {
    FUNC_LOG;
    CMRFieldContainer* self = new( ELeave )CMRFieldContainer( aFactory );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::ConstructL( const CCoeControl& aParent )
    {
    FUNC_LOG;
    CCoeControl::SetComponentsToInheritVisibility( ETrue );
    SetContainerWindowL( aParent );
    
    TBool focusSet( EFalse );
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
            iFocusedFieldIndex = i;
            }
        }

    for ( TInt i = 0; i < count; i++ )
        {
        CESMRField* field = iFactory.Field( i );

        field->SetListObserver( this );
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::FocusedItem
// ---------------------------------------------------------------------------
//
CESMRField* CMRFieldContainer::FocusedField() const
    {
    FUNC_LOG;
    /*
     * Returns currently focused field
     */
    CESMRField* field = NULL;
    if ( iFactory.Count() > 0 )
        {
        field = iFactory.Field( iFocusedFieldIndex );
        }
    return field;
    }


// ---------------------------------------------------------------------------
// CMRFieldContainer::MoveFocusUpL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRFieldContainer::MoveFocusUpL( TBool aHiddenFocus )
    {
    FUNC_LOG;
    /*
     * Moves focus up after key event. If aHiddenFocus, moves focus 
     * to the first visible field in the bottom of the viewable area.
     */
     
    if( aHiddenFocus )
        {
        return MoveFocusVisibleL();
        }
    else
        {
        TInt ind( iFocusedFieldIndex );

        // search next visible focus item
        while ( ind > 0 )
            {
            CESMRField* field = iFactory.Field( --ind );

            if ( !field->IsNonFocusing() )
                {
                field = iFactory.Field( ind );

                CESMRField* focusedField = iFactory.Field( iFocusedFieldIndex );
                if ( field->IsVisible() )
                    {
                    TBool canLoseFocus(
                            focusedField->OkToLoseFocusL( field->FieldId() ) );

                    if ( canLoseFocus )
                        {
                        field->SetPreItemIndex( iFocusedFieldIndex );
                        iFocusedFieldIndex = ind;
                        field->SetCurrentItemIndex( iFocusedFieldIndex );

                        // Remove focus from previous position 
                        focusedField->SetOutlineFocusL( EFalse );
                        focusedField->SetFocus( EFalse );
                        
                        // Set focus to new position
                        field->SetOutlineFocusL( ETrue );
                        field->SetFocus( ETrue );

                        // Scrollbar and physics update is done here
                        ScrollControlVisible( iFocusedFieldIndex );
                        
                        DrawDeferred();
                        }

                    return EKeyWasConsumed;
                    }
                }
            }
        return EKeyWasNotConsumed;
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::MoveFocusDownL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRFieldContainer::MoveFocusDownL( TBool aHiddenFocus )
    {
    FUNC_LOG;
    /*
     * Moves focus down after key event. If aHiddenFocus, moves focus 
     * to the first visible field in the top of the viewable area.
     */
    
    if( aHiddenFocus )
        {
        return MoveFocusVisibleL();
        }
    else
        {
        TInt ind( iFocusedFieldIndex );

        // start searching next possible focus item
        TInt maxItemIndex = iFactory.Count() - 1;

        while ( ind < maxItemIndex )
            {
            // only visible and focusable items can be focused
            CESMRField* field = iFactory.Field( ++ind );

            if ( field->IsVisible() && !field->IsNonFocusing() )
                {
                CESMRField* focusedField = iFactory.Field( iFocusedFieldIndex );

                TBool canLoseFocus(
                        focusedField->OkToLoseFocusL( field->FieldId() ) );

                // check it its ok for the old focus item to lose focus
                if ( canLoseFocus )
                    {
                    field->SetPreItemIndex( iFocusedFieldIndex );
                    iFocusedFieldIndex = ind;
                    field->SetCurrentItemIndex( iFocusedFieldIndex );

                    // Remove focus from previous position 
                    focusedField->SetOutlineFocusL( EFalse );
                    focusedField->SetFocus( EFalse );
                    
                    // Set focus to new position
                    field->SetOutlineFocusL( ETrue );
                    field->SetFocus( ETrue );

                    // Scrollbar and physics update is done here
                    ScrollControlVisible( iFocusedFieldIndex );
                    
                    DrawDeferred();
                    }

                return EKeyWasConsumed;
                }
            }
        return EKeyWasNotConsumed;
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::MoveFocusVisibleL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRFieldContainer::MoveFocusVisibleL()
    {
    FUNC_LOG;
    /*
     * Move focus to first completely visible field in the view, 
     * if focus was in a field that was not visible in the view.
     * Use case: After pointer scroll focus is hidden. User presses
     * arrow keys -> Focus appears to the first visible field on the upper
     * or bottom part of the viewable area.
     */
    
    TKeyResponse response( EKeyWasNotConsumed );

    CESMRField* focusedField = FocusedField();
    CESMRField* visibleField = NULL;
    TRect view( Parent()->Rect() );

    // Focus is above the visible view area
    if ( focusedField->Rect().iBr.iY <= view.iTl.iY )
        {
        // Move focus to next visible field
        TInt lastIndex = iFactory.Count() - 1;
        TInt ind( iFocusedFieldIndex );
        while ( ind < lastIndex && !visibleField )
            {
            visibleField = iFactory.Field( ++ind );
            TRect fieldRect = visibleField->Rect();
            if ( visibleField->IsVisible()
                 && !visibleField->IsNonFocusing()
                 && fieldRect.iTl.iY >= view.iTl.iY )
                {
                // Update previous and current field indexes
                visibleField->SetPreItemIndex( iFocusedFieldIndex );
                iFocusedFieldIndex = ind;
                visibleField->SetCurrentItemIndex( iFocusedFieldIndex );
                }
            else
                {
                visibleField = NULL;
                }
            }
        }
    // Focus is below the visible view area
    else
        {
        // Move focus to previous visible field
        TInt ind( iFocusedFieldIndex );
        while ( ind > 0 && !visibleField )
            {
            visibleField = iFactory.Field( --ind );
            TRect fieldRect = visibleField->Rect();
            if ( visibleField->IsVisible()
                 && !visibleField->IsNonFocusing()
                 && fieldRect.iBr.iY <= view.iBr.iY )
                {
                // Update previous and current field indexes
                visibleField->SetPreItemIndex( iFocusedFieldIndex );
                iFocusedFieldIndex = ind;
                visibleField->SetCurrentItemIndex( iFocusedFieldIndex );
                }
            else
                {
                visibleField = NULL;
                }
            }
        }

    if ( visibleField )
        {
        // Remove existing focus
        focusedField->SetOutlineFocusL( EFalse );
        focusedField->SetFocus( EFalse );
        
        // Set focus to new field
        visibleField->SetOutlineFocusL( ETrue );
        visibleField->SetFocus( ETrue );

        response = EKeyWasConsumed;
        
        DrawDeferred();
        }
    
    return response;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::SetFieldContainerObserver
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::SetFieldContainerObserver( 
        MMRFieldContainerObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRFieldContainer::CountComponentControls() const
    {
    FUNC_LOG;
    return iFactory.Count();
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRFieldContainer::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    return iFactory.Field( aIndex );
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CMRFieldContainer::MinimumSize()
    {
    FUNC_LOG;
    /*
     * Returns the minimum size required by the field container
     */
    
    TSize containerSize;
    
    const TInt count( iFactory.Count() );
    
    for ( TInt i(0); i < count; ++i )
        {
        CESMRField* field = iFactory.Field( i );
        if ( field->IsVisible() )
            {
            TRect rect( field->Rect() );
            containerSize.iHeight += rect.Height();
            }
        }
    
    containerSize.iWidth = Parent()->Rect().Width();
    
    return containerSize;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::SizeChanged()
    {
    FUNC_LOG;
    
    // TEST CODE:
    TRect fieldcontainerRect = Rect();
    TRect parentRect = Parent()->Rect();
    

    // For example when orientation changes, we might need to scroll 
    // the currently focused control visible again. This handles also
    // scrollbar and physics updates.
    ScrollControlVisible( KErrNotFound ); 
    
    TPoint tl( Position() );

    const TInt count( iFactory.Count() );

    // Loop all the visible fields and set size and position in the list
    for ( TInt i = 0; i < count; i++ )
        {
        CESMRField* field = iFactory.Field( i );

        if ( field->IsVisible() )
            {
            LayoutField( *field, tl );

            TInt height = field->Size().iHeight;
            tl.iY += height;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::ControlSizeChanged
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::ControlSizeChanged( CESMRField* aField )
    {
    FUNC_LOG;
    /*
     * Called whenever a fields size has changed. Requires always 
     * relayouting. 
     */

    if ( !aField )
        {
        SizeChanged();
        }
    else
        {
        // Relayout field if size has changed
        TSize old( aField->Size() );
        TSize size( aField->MinimumSize() );
        if( size != old )
            {
            aField->SetSize( size );
            
            TPoint tl( aField->Position() );
            TInt index = IndexByFieldId( iFactory, aField->FieldId() );

            // Move fields below this particular field
            tl.iY += size.iHeight;
            MoveFields( index + 1, tl );

            // Fields have been re-layouted / moved. This requires resetting
            // the size of this field container.
            SetSize( MinimumSize() );
            
            // Update also scrollbar and physics
            iObserver->UpdateScrollBarAndPhysics();
            
            if( index == iFocusedFieldIndex )
                {
                // Scroll this field completely visible, if required.
                // This updates also scrollbar and physics if scrolling 
                // is done.
                ScrollControlVisible( iFocusedFieldIndex );
                }
            
            DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::ShowControl
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::ShowControl( TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    /*
     * Inserts field visible and layouts it.
     */
    
    CESMRField* field = iFactory.FieldById( aFieldId );

    if ( field && !field->IsVisible() )
        {
        // Make field visible
        field->MakeVisible( ETrue );
        TInt index = IndexByFieldId( iFactory, aFieldId );

        TPoint tl( Rect().iTl );
        TInt prevIndex = index - 1;

        // Get previous visible field position
        // Index 0 must be included in attendee field case, 
        // to avoid field collision. But in response field, 
        // causes misplacing of area.
        if ( ( prevIndex >= 0 ) && ( aFieldId != EESMRFieldResponseArea ) )
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
        LayoutField( *field, tl);

        // Move following fields
        tl.iY += field->Size().iHeight;
        MoveFields( ++index, tl );

        // Set fieldcontainer size again, because 
        // the amount of fields has changed.
        SetSize( MinimumSize() );
        
        // Scrollbar and physics require updating.
        iObserver->UpdateScrollBarAndPhysics();
        }
    
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::HideControl()
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::HideControl( TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    /*
     * Sets field non-visible and moves other fields accordingly.
     * Does not delete the field. 
     */
    CESMRField* field = iFactory.FieldById( aFieldId );

    if ( field && field->IsVisible() )
        {
        field->MakeVisible( EFalse );
        TInt index = IndexByFieldId( iFactory, aFieldId );
        TBool focused = ( index == iFocusedFieldIndex )? ETrue : EFalse;

        TPoint pos( field->Position() );
        MoveFields( index, pos );

        // Set fieldcontainer size again, because 
        // the amount of fields has changed.
        SetSize( MinimumSize() );
        
        // Scrollbar and physics require updating.
        iObserver->UpdateScrollBarAndPhysics();
        
        if ( focused && !field->IsNonFocusing() )            
            {
            // Set focus to next field, or if removed field was the last 
            // field, then move focus to last visible field
            TInt lastVisibleFieldIndex( LastVisibleField( aFieldId ) ); 
            
            // If field was the last one...
            if( lastVisibleFieldIndex == index )
                {
                // ... Set focus to last visible field.
                TRAP_IGNORE( DoSetFocusL( lastVisibleFieldIndex ) )
                }
            else
                {
                // Othwerwise set focus to next field
                TRAP_IGNORE( DoSetFocusL( index + 1 ) )
                }
            }
        }
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::IsControlVisible
// ---------------------------------------------------------------------------
//
TBool CMRFieldContainer::IsControlVisible( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    /*
     * Returns ETrue/EFalse if the field with given field id is 
     * visible or not.
     */
    
    TBool ret( EFalse );

    CESMRField* field = iFactory.FieldById( aField );
    if ( field && field->IsVisible() )
        {
        ret = ETrue;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::SetControlFocused
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::SetControlFocusedL( TESMREntryFieldId aField )
    {
    FUNC_LOG;
    /*
     * Set the field with given field id focused.
     */

    // This handles scrollbar and physics updates also.
    DoSetFocusL( IndexByFieldId( iFactory, aField ) );
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::ListHeight
// ---------------------------------------------------------------------------
//
TInt CMRFieldContainer::ListHeight()
    {
    FUNC_LOG;
    /*
     * Returns the height of all fields, in other words the height
     * of the field container.
     */
    return MinimumSize().iHeight;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::IsFocusedControlsBottomVisible
// ---------------------------------------------------------------------------
//
TBool CMRFieldContainer::IsFocusedControlsBottomVisible()
    {
    FUNC_LOG;
    /*
     * Checks if focused field is completely visible in the viewable area. 
     */
    
    // Fetch the position information about currently focused field:
    CESMRField* field = iFactory.Field( iFocusedFieldIndex );
    TBool ret( EFalse );
    if ( field->Rect().iBr.iY <= Parent()->Rect().iBr.iY )
       {
       ret = ETrue;
       }

    return ret;
    }


// ---------------------------------------------------------------------------
// CMRFieldContainer::ScrollControlVisible
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::ScrollControlVisible( TInt aInd )
    {
    FUNC_LOG;
    /*
     * Scrolls the field with the given index visible
     */
    
    CESMRField* field = NULL;
    if ( aInd == KErrNotFound )
        {
        field = FocusedField();
        }
    else
        {
        field = iFactory.Field( aInd );
        }

    ASSERT( field );
    
    TRect fieldRect( field->Position(), field->Size() );
    TRect parentRect( Parent()->Rect() );
    
    /*
     * Case 1: Field's height is less than the viewable area height,
     * let's scroll the whole field visible.
     */
    if( fieldRect.Height() < parentRect.Height() )
        {
        // Scrolling down, let's move fields up
        if( fieldRect.iBr.iY > parentRect.iBr.iY )
            {
            iObserver->ScrollFieldsUp( 
                    fieldRect.iBr.iY - parentRect.iBr.iY );
            }
        // scrolling up, let's move field down
        if( fieldRect.iTl.iY < parentRect.iTl.iY )
            {
            iObserver->ScrollFieldsDown( 
                    parentRect.iTl.iY - fieldRect.iTl.iY );
            }
        }
    
    /*
     * Case 2: Field's height is more than the viewable area's height.
     */
    else
        {
        // Field is in view mode
        if( field->FieldMode() == EESMRFieldModeView )
            {
            // Focus to this field is coming from above
            if( field->PreItemIndex() < field->CurrentItemIndex() )
                {
                // Let's scroll the top of the field to the 
                // top of the viewable area
                iObserver->ScrollFieldsUp( 
                                   fieldRect.iTl.iY - parentRect.iTl.iY );
                
                }
            // Focus to this field is coming from below
            if( field->PreItemIndex() > field->CurrentItemIndex() )
                {
                // Let's scroll the bottom of the field to the
                // bottom of the viewable area
                iObserver->ScrollFieldsDown( 
                                   parentRect.iBr.iY - fieldRect.iBr.iY );
                }
            }
        
        // Field is in edit mode
        if( field->FieldMode() == EESMRFieldModeEdit )
            {
            // TODO: CORRECT if broken
            TInt viewBottom( parentRect.iBr.iY );

            TInt fieldUpper( 0 );
            TInt fieldLower( 0 );
            field->GetCursorLineVerticalPos( fieldUpper, fieldLower );

            TInt focusFieldVisibleUp( fieldRect.iTl.iY + fieldUpper );
            TInt focusFieldVisibleBottom( fieldRect.iTl.iY + fieldLower );

            // desired position below view rect:
            TInt viewHeight( parentRect.Height() );
            if ( focusFieldVisibleBottom > viewBottom )
                {
                // move field focus line bottom to view bottom
                TInt px = focusFieldVisibleBottom - viewHeight;

                
                // if focus on last field: add margin height to
                // scroll amount.
                if ( iFocusedFieldIndex == iFactory.Count()-1 )
                    {
                    px += KVerticalScrollMargin; // TODO: Fix me! K-value must be removed.
                    }
                
                // Scrollbar and physics update is done here
                iObserver->ScrollFieldsUp( px );
                }
            else
                {
                // If field top is invisible, move downwards to make top visible.
                TInt viewTop( parentRect.iTl.iY );

                // recalculate field rect
                focusFieldVisibleUp = fieldRect.iTl.iY + fieldUpper;
                focusFieldVisibleBottom = fieldRect.iTl.iY + fieldLower;

                if ( focusFieldVisibleUp < viewTop )
                    {
                    TInt px( viewTop - focusFieldVisibleUp );

                    // Scrollbar and physics update is done here
                    iObserver->ScrollFieldsDown( px );
                    }
                }
            }        
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::RePositionFields
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::RePositionFields( TInt aAmount )
    {
    FUNC_LOG;
    
    // Movement downwards
    if( aAmount >= 0 )
        {
        // Also scrollbar and physics update is done here
        iObserver->ScrollFieldsDown( aAmount );
        }
    // Movement upwards
    else
        {
        // Also scrollbar and physics update is done here
        iObserver->ScrollFieldsUp( -aAmount );
        }
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::ViewableAreaRect
// ---------------------------------------------------------------------------
//
TRect CMRFieldContainer::ViewableAreaRect()
    {
    FUNC_LOG;
    return Parent()->Rect();
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::MoveFields
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::MoveFields( TInt aIndex, TPoint& aTl )
    {
    FUNC_LOG;
    /*
     * Moves fields from the given index towards the last item. 
     * This function does not update scrollbar or physics.
     */
    
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
// CMRFieldContainer::LayoutField
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::LayoutField( CESMRField& aField, const TPoint& aTl )
    {
    FUNC_LOG;
    /*
     * Layouts given field according to the size required by the field and
     * given TPoint. This function does not update scrollbar or physics.
     */
    
    TSize size( aField.MinimumSize() );
    aField.SetPosition( aTl );
    aField.SetSize( size );
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::IsLastVisibleField
// ---------------------------------------------------------------------------
//
TInt CMRFieldContainer::LastVisibleField( 
        TESMREntryFieldId aFieldId )
    {
    /*
     * Helper function to find out the last visible field in the list.
     */
    
    TInt lastVisibleFieldIndex( 0 );
    TInt count( iFactory.Count() );
    
    // Go through fields from last field towards the first field
    for( TInt i( 1 ); i > count; ++i )
        {
        // Once last visible field is found...
        if( iFactory.Field( count - i )->IsVisible() )
            {
            // ... Compare it to the given field index ...
            if( iFactory.Field( count - i )->FieldId() == aFieldId )
                {
                // ... And if match is found, given fieldId is the 
                // the last visible field.
                lastVisibleFieldIndex = 
                    IndexByFieldId( iFactory, aFieldId );
                }
            else
                {
                // Otherwise return the found last visible field.
                lastVisibleFieldIndex = 
                    IndexByFieldId( iFactory, 
                                    iFactory.Field( count - i )->FieldId() );
                }
            break;
            }
        }
    
    return lastVisibleFieldIndex;
    }

// ---------------------------------------------------------------------------
// CMRFieldContainer::DoSetFocusL
// ---------------------------------------------------------------------------
//
void CMRFieldContainer::DoSetFocusL( TInt aNewFocusIndex )
    {
    FUNC_LOG;
    /*
     * Sets the focus according to the given index.
     */
    
    TInt count( iFactory.Count() );
    aNewFocusIndex = Max( 0, Min( aNewFocusIndex, count - 1 ) );

    if ( aNewFocusIndex != iFocusedFieldIndex )
        {
        // Get current focused field
        CESMRField* old = iFactory.Field( iFocusedFieldIndex );

        // Get next focused field
        CESMRField* field = iFactory.Field( aNewFocusIndex );
        
        // Do sanity checks
        while ( aNewFocusIndex < count && !field->IsVisible() )
            {
            field = iFactory.Field( aNewFocusIndex++ );
            }

        if ( !field->IsVisible() )
            {
            aNewFocusIndex = iFocusedFieldIndex - 1;
            while ( aNewFocusIndex > 0 && !field->IsVisible() )
                {
                field = iFactory.Field( aNewFocusIndex-- );
                }
            }

        ASSERT( field->IsVisible() );
        
        // Update current and previous item indexes
        field->SetPreItemIndex( iFocusedFieldIndex );
        iFocusedFieldIndex = aNewFocusIndex;
        field->SetCurrentItemIndex( iFocusedFieldIndex );
        
        // Remove focus from old
        old->SetOutlineFocusL( EFalse );
        old->SetFocus( EFalse );
        
        // update focus index to new index
        field->SetOutlineFocusL( ETrue );
        field->SetFocus( ETrue );
        
        // This handles also scrollbar and physics updating,
        // if view scrolling is done.
        ScrollControlVisible( iFocusedFieldIndex ); 
        
        DrawDeferred();
        }
    }


// End of file
