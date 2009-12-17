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
* Description:  Container class for compose view
*
*/



#ifndef CNCSATTACHMENTFIELD_H
#define CNCSATTACHMENTFIELD_H


#include "ncscontrol.h"


class MNcsFieldSizeObserver;
class CNcsHeaderContainer;


/**
 *  CNcsAttachmentField
 */
class CNcsAttachmentField: 
	public CCoeControl,
	public MNcsControl
    {
public: // construct & destruct

    static CNcsAttachmentField* NewL( 
    	TInt aLabelTextId,
        MNcsFieldSizeObserver* aSizeObserver,
        CNcsHeaderContainer* aParentControl );

    virtual ~CNcsAttachmentField();

public: // methods

    TInt MinimumHeight() const;

    void SetMaxLabelLength( TInt aMaxLength );

    TInt GetMinLabelLength() const;
    
    void SetTextL( const TDesC& aText );
    void SetTextL( const TDesC& aAttachmentName, const TDesC& aAttachmentSizeDesc );

public: // from MNcsControl
    
    TInt LineCount() const;

    TInt ScrollableLines() const;

    TInt GetNumChars() const;

    TInt CursorLineNumber() const;

    TInt CursorPosition() const;

    void Reposition( TPoint& aPt, TInt aWidth );

    const TDesC& GetLabelText() const;
    
    // <cmail> Platform layout change       
    TInt LayoutLineCount() const;
    // </cmail> Platform layout change
    
public: // from CCoeControl

    void Draw( const TRect& aRect ) const;
        
    void SizeChanged();

    void PositionChanged();

    void SetContainerWindowL( const CCoeControl& aContainer );

    void FocusChanged( TDrawNow aDrawNow );                  

    void HandleResourceChange( TInt aType );

private: // methods
    
    CNcsAttachmentField( MNcsFieldSizeObserver* aSizeObserver, 
                         CNcsHeaderContainer* aParentControl );
        
    void ConstructL( TInt aLabelTextId );
    
    void UpdateColors();
    
	void UpdateFontSize();

	// <cmail> Platform layout change 
    void LayoutControls();
	
	void UpdateIconPositions( const TRect& aRect );
	// </cmail> Platform layout change 

    void UpdateAttachmentTextL();
    
    // <cmail> Platform layout change
    /**
     * Resizes icons 
     */
    void ResizeIcons();
    
    TInt CreateIcons();
    void CreateIconsL();
    // </cmail> Platform layout change
    
private: // data
    
    CNcsHeaderContainer* iParentControl; // not owned
    
    CNcsLabel* iAttachmentLabel;
    
// <cmail> Platform layout change
    //CFont* iLabelFont; // not owned, needs to be released in the end
    //CFont* iEditorFont; // not owned, needs to be released in the end
    const CFont* iLabelFont; // not owned
    const CFont* iEditorFont; // not owned
// </cmail>
    
    HBufC* iAttachmentName;
    HBufC* iAttachmentSizeDesc;
        
    TInt iMaximumLabelLength;         

    TRgb iBgColor;
    
    TRgb iBorderColor;
    
    // Attachment icon
    CFbsBitmap* iAttachmentBitmap;
    CFbsBitmap* iAttachmentMask;
    TPoint iAttachmentIconPos;

    // Action menu icon
    CFbsBitmap* iActionMenuBitmap;
    CFbsBitmap* iActionMenuMask;
    TPoint iActionMenuIconPos;
    };

#endif
