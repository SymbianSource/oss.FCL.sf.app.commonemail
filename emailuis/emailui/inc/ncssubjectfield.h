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



#ifndef CNCSSUBJECTFIELD_H
#define CNCSSUBJECTFIELD_H


#include <aknview.h>
#include <AknDef.h>
#include "ncseditor.h"
#include "ncscontrol.h"
#include"ncsconstants.h"


class CEikImage;
class MNcsFieldSizeObserver;
class CNcsHeaderContainer;
class CAknPhysics;

// CLASS DECLARATION
/**
*  CNcsSubjectField container control class.
*
*  @lib -
*  @since S60 v3.0
*/
class CNcsSubjectField : public CCoeControl, public MEikEdwinSizeObserver, public MNcsControl
    {
public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CNcsSubjectField object, which will draw itself to aRect
        * @since S60 v3.0
        * @param aParent parent window.
        * @param aText caption text
        * @return a pointer to the created instance of CNcsSubjectField.
        */
    static CNcsSubjectField* NewL( TInt aLabelTextId,
                                   MNcsFieldSizeObserver* aSizeObserver,
                                   CNcsHeaderContainer* aParentControl );

    virtual ~CNcsSubjectField();

public: // new functions
    
    TInt MinimumHeight();
        
    void SetSubjectL( const TDesC& aSubject );
        
    CNcsEditor* Editor() const;
        
    TInt LineCount() const;

    TInt CursorLineNumber() const;

    TInt CursorPosition() const;

    TInt ScrollableLines() const;

    TInt GetNumChars() const;

    const TDesC& GetLabelText() const;
    
    // <cmail> Platform layout change       
    TInt LayoutLineCount() const;
    // </cmail> Platform layout change    
    
    void GetLineRectL( TRect& aLineRect ) const;

    void SetMaxLabelLength( TInt aMaxLength );

    TInt GetMinLabelLength() const;

    void SelectAllTextL();

    void SetTextColorL( TLogicalRgb aColor );
		
    void EnableKineticScrollingL(CAknPhysics* aPhysics );
    
    void SetCursorVisible( TBool aCursorVisible );
    
public: // from CoeControl
        /**
        * SetContainerWinowL
        * Set the cotainer window this control should raw in
        * @since S60 v3.0
        * @param aContainer The window continer that owns this control
        */
    void SetContainerWindowL(const CCoeControl& aContainer);

    /**
    * Draw
    * Draw this CNcsSubjectField to the screen.
    * @since S60 v3.0
    * @param aRect the rectangle of this view that needs updating
    */
    void Draw( const TRect& aRect ) const;
        
    /**
    * From CCoeControl
    */
    void SizeChanged();

    void PositionChanged();
        
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
    void FocusChanged(TDrawNow aDrawNow);                  

    void HandleResourceChange( TInt aType );
        
    /**
    * From MEikEdwinSizeObserver
    */
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize );
        
    inline void Reposition( TPoint& aPt, TInt aWidth );

private:
    
    CNcsSubjectField( MNcsFieldSizeObserver* aSizeObserver, CNcsHeaderContainer* aParentControl );
        
    void ConstructL( TInt aLabelTextId );
    
    void UpdateColors();
    
    void UpdateFontSize();
    
    //<cmail>
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    //</cmail>
        
private: // data
    
    CNcsHeaderContainer* iParentControl; // not owned
    
    /*
    * text field
    * Own
    */
    CNcsLabel* iLabel;
        
// <cmail> Platform layout changes
    const CFont* iFont; // not owned, needs to be released in the end
// </cmail> Platform layout changes
    
    /*
    * text field
    * Own
    */
    CNcsEditor* iTextEditor;
        
    TInt iEditorMinimumHeight;
            
    TInt iMaximumLabelLength;         
      	
    TRgb iBorderColor;
    
    TRgb iBgColor;
    
    TBool iDrawAfterFocusChange;
   
// <cmail> Platform layout changes
    TInt iEditorLineCount;
// </cmail> Platform layout changes
    };

#endif // CNCSSUBJECTFIELD_H
