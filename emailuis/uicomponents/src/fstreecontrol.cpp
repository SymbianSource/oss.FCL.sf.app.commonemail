/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CFsTreeControl is a HUI control that handles UI events for
*                tree list.
*
*/


//////TOOLKIT INCLUDES
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfevent.h>
// </cmail>


// <cmail> Needed for pointer events. "Base class modifications for using touch"
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
// </cmail>


//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "fstreecontrol.h"
#include "fstreevisualizer.h"
 
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeControl* CFsTreeControl::NewL( CAlfEnv& aEnv, 
                                               CFsTree& aTree, 
                                               MFsTreeVisualizer& aVisualizer )
    {
    FUNC_LOG;
    CFsTreeControl* self = new( ELeave ) CFsTreeControl( //aEnv, 
                                                         aTree, 
                                                         aVisualizer );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreeControl::~CFsTreeControl() 
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// From class CHuiControl.
// Called when an input event is being offered to the control.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsTreeControl::OfferEventL(const TAlfEvent& aEvent)
  {
  FUNC_LOG;
  TBool eventHandled = EFalse;
  
  //Handle pointer events
  if ( aEvent.IsPointerEvent() )
      {
      if (aEvent.PointerEvent().iType == TPointerEvent::EButton1Down)
          {
          SetDragEvents(ETrue);
          }
      if (aEvent.PointerEvent().iType == TPointerEvent::EButton1Up)
          {
          SetDragEvents(EFalse);
          }
      eventHandled = iTreeVisualizer.HandlePointerEventL( aEvent );
      }
  else if( iFocused && aEvent.IsKeyEvent() && aEvent.Code() == EEventKey)
      {
      eventHandled = iTreeVisualizer.HandleKeyEventL( aEvent );
      }    
  return eventHandled;
  }

//---------------------------------------------------------------------------
// Sets focus state of list component
//---------------------------------------------------------------------------
void CFsTreeControl::SetFocused( const TBool aFocused )
    {
    FUNC_LOG;
    iFocused = aFocused;
    }
//---------------------------------------------------------------------------
// Returns state of the focus
//---------------------------------------------------------------------------
TBool CFsTreeControl::IsFocused() const
    {
    FUNC_LOG;
    return iFocused;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CFsTreeControl::VisualPrepareDrawFailed(CAlfVisual& /*aVisual*/,
                                             TInt /*aErrorCode*/)
    {
    FUNC_LOG;

    }

// <cmail> "Base class modifications for using touch"
// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeControl::CFsTreeControl( /*CAlfEnv& aEnv,*/ CFsTree& aTree, MFsTreeVisualizer& aVisualizer )
    : //CHuiControl( aEnv ),
      iTree(aTree),
      iTreeVisualizer( aVisualizer ),
	  iFocused( ETrue ),
	  iGetExtraPointerEvents(EFalse)
    {
    FUNC_LOG;

    }
// </cmail>


// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeControl::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );
    }

// <cmail>
void CFsTreeControl::NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& /*aDisplay*/ )
    {
    iTreeVisualizer.NotifyControlVisibilityChange(aIsVisible);
    if (!aIsVisible)
        {
        SetDragEvents(EFalse);
        }
    }


void CFsTreeControl::SetDragEvents(TBool aEnable)
    {
    //Add & remove extra touch events. 
    if (Display())
        {
        if(aEnable && !iGetExtraPointerEvents)
            {
            Display()->Roster().AddPointerEventObserver(EAlfPointerEventReportDrag, *this);
            Display()->Roster().AddPointerEventObserver(EAlfPointerEventReportLongTap, *this);
            Display()->Roster().AddPointerEventObserver(EAlfPointerEventReportUnhandled, *this);
            //aDisplay.Roster().DisableLongTapEventsWhenDragging(*this);
            iGetExtraPointerEvents = ETrue;
            }
        else if(!aEnable && iGetExtraPointerEvents )
            {
            Display()->Roster().RemovePointerEventObserver(EAlfPointerEventReportDrag, *this);
            Display()->Roster().RemovePointerEventObserver(EAlfPointerEventReportLongTap, *this);
            Display()->Roster().RemovePointerEventObserver(EAlfPointerEventReportUnhandled, *this);
            iGetExtraPointerEvents = EFalse;
            }
        }
    }
// </cmail>

