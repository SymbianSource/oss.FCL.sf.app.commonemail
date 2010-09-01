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
* Description:  FreestyleEmailUi  list visualiser 
*				 abstract base classes definitions
*
*/

 
#ifndef __FREESTYLEEMAILUI_LISTVISUALISER_H__
#define __FREESTYLEEMAILUI_LISTVISUALISER_H__

class MFSEmailListVisualiser
  {
public:
  virtual void Select(TInt aIndex) = 0;
  virtual void Refresh() = 0;
  virtual void HandleKeyEventL( const TAlfEvent& aEvent ) = 0;
  virtual void HandleSelectionL( TInt aSelectedIndex ) = 0;
  virtual void ListTypeToggleRequestedL() = 0;
  virtual void SendKeyPressedL() = 0;
  };

#endif // __FREESTYLEEMAILUI_LISTVISUALISER_H__
