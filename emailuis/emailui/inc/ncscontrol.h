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
* Description:  virtual class for compose view
*
*/



#ifndef __NCS_CONTROL_H__
#define __NCS_CONTROL_H__

#include <e32base.h>
#include <eiklabel.h>

#include "ncsfieldsizeobserver.h"

class CNcsEditor;
class CCoeControl;


// CLASS DECLARATION

/**
*  CNcsControl container control class.
*
*  @lib -
*  @since S60 v3.0
*/
class MNcsControl
    {

public: // constructor

    MNcsControl( MNcsFieldSizeObserver* aObserver );

public: // new functions

	virtual TInt LineCount() const = 0;
	virtual TInt ScrollableLines() const = 0;
	virtual TInt GetNumChars() const = 0;
	virtual TInt CursorLineNumber() const = 0;
	virtual TInt CursorPosition() const = 0;
	virtual void Reposition(TPoint& aPt, TInt aWidth) = 0;
    virtual const TDesC& GetLabelText() const = 0;
    // <cmail> Platform layout change    
    virtual TInt LayoutLineCount() const = 0;
    // </cmail> Platform layout change    

protected:
    
    MNcsFieldSizeObserver* iSizeObserver;

    };

class CNcsLabel : public CEikLabel, public MNcsControl
    {
public:

    // <cmail> Plaform layout change
	CNcsLabel( const CCoeControl& aParent, MNcsFieldSizeObserver* aSizeObserver );
	// </cmail> Plaform layout change

public:

	inline virtual TInt LineCount() const;
	inline virtual TInt ScrollableLines() const;
	inline virtual TInt GetNumChars() const;
	inline virtual TInt CursorLineNumber() const;
	inline virtual TInt CursorPosition() const;
	virtual void Reposition( TPoint& aPt, TInt aWidth );    
    inline virtual const TDesC& GetLabelText() const;
    // <cmail> Platform layout change       
    TInt LayoutLineCount() const;
    // </cmail> Platform layout change    

    virtual void FocusChanged( TDrawNow aDrawNow );
    
protected: // methods from base class

    virtual void HandleResourceChange( TInt aType );    
private: // methods used internally

// <cmail> Plaform layout change
private: // data
    
    void UpdateTextColor();
       
    const CCoeControl& iParent;
    
// </cmail> Plaform layout change
    };

#include "ncscontrol.inl"

#endif // __NCS_CONTROL_H__


// End of File
