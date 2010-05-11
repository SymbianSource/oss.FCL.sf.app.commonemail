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



#ifndef CNCSADDRESSINPUTFIELD_H
#define CNCSADDRESSINPUTFIELD_H


// INCLUDES
#include <aknview.h>
#include <AknDef.h>
//<cmail>
#include <eikedwob.h>
//</cmail>
#include "ncscontrol.h"

class CAknPhysics;

//<cmail>  Temporary hardcoded layout values
//Horizontal values
const TInt KButtonXa = 5;
const TInt KButtonXb = 80;
const TInt KTxtFieldXa = KButtonXb + 7;
const TInt KTxtFieldXb = 445;

//To-field
const TInt KToAy = 5;
const TInt KToBy = KToAy + 45;
const TInt KToFieldAy = 5;
const TInt KToFieldBy = KToFieldAy + 45;

//Cc-field
const TInt KCcAy = KToBy + 7;
const TInt KCcBy = KCcAy + 45;
const TInt KCcFieldAy = KToFieldBy + 7;
const TInt KCcFieldBy = KCcFieldAy + 45;

//Bcc-field
const TInt KBccAy = KCcBy + 7;
const TInt KBccBy = KBccAy + 45;
const TInt KBccFieldAy = KCcFieldBy + 7;
const TInt KBccFieldBy = KBccFieldAy + 45;

//Subject field
const TInt KSubjectLblRectAy = KBccBy + 7;
const TInt KSubjectLblRectBy = KSubjectLblRectAy + 45;
const TInt KSubjectFieldRectAy = KBccFieldBy + 7;
const TInt KSubjectFieldRectBy = KSubjectFieldRectAy + 45;

//A konst for empty return values
_LIT( KEmptyReturnValue, "" );
//</cmail>

class CEikImage;
class CAknButton;
class CNcsAifEditor;
class CNcsEmailAddressObject;
class CNcsHeaderContainer;


/**
*  CNcsAddressInputField container control class.
*/
class MNcsAddressPopupList
    {
public:
    virtual void UpdatePopupContactListL( const TDesC& aMatchString, TBool iListAll ) = 0;
	virtual void ClosePopupContactListL() = 0;
    };


/**
*  CNcsAddressInputField container control class.
*/
class CNcsAddressInputField: public CCoeControl, public MEikEdwinSizeObserver,
                             public MNcsControl, public MCoeControlObserver
    {
public: // enum
    
    enum TNcsInputFieldType
        {
        EInputFieldTo,
        EInputFieldCc,
        EInputFieldBcc
        };
    
public: // Constructors and destructor

        /**
        * NewL
        * Create a CNcsAddressInputField object, which will draw itself to aRect
        */
    static CNcsAddressInputField* NewL( 
        TInt aLabelId,
        TNcsInputFieldType aFieldType,
        MNcsFieldSizeObserver* aSizeObserver,
        MNcsAddressPopupList* aAddressPopupList,
        CNcsHeaderContainer* aParentControl );
        
    /**
    * ~CNcsAddressInputField
    * Destructor
    */
    virtual ~CNcsAddressInputField();

public: // new functions
    
    TInt MinimumHeight() const;
        
    void SetAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddress );
        
    const RPointerArray<CNcsEmailAddressObject>& GetAddressesL( TBool aParseNow=ETrue );
        
    void GetLineRectL( TRect& aLineRect ) const;
        
    void SetMaxLabelLength( TInt aMaxLength );
        
    TInt GetMinLabelLength() const;
        
    void AddAddressL( const CNcsEmailAddressObject& aAddress ); 
        
    void AddAddressL( const TDesC& aDisplayName, const TDesC& aEmail );   
                    
    void AddAddressL( const TDesC& aEmail );
        
    void AppendAddressesL( const RPointerArray<CNcsEmailAddressObject>& aAddress );
        
    CEikEdwin* TextEditor() const;

    HBufC* GetLookupTextLC() const;

    const CNcsEmailAddressObject* EmailAddressObjectBySelection() const;
		
    void SelectAllTextL();
		
    void SetTextColorL( TLogicalRgb aColor );

    CNcsEditor* Editor() const;
        
    TBool IsEmpty() const;

    void SetLabelTextColorL (TRgb aColor);
    
    void FixSemicolonAtTheEndL();

    void SetPhysicsEmulationOngoing( TBool aPhysOngoing );
    
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
    * Draw this CNcsAddressInputField to the screen.
    * @since S60 v3.0
    * @param aRect the rectangle of this view that needs updating
    */
    void Draw( const TRect& aRect ) const;
        
    void SizeChanged();
    
	void PositionChanged();

    //<cmail>
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    //</cmail>
    
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
    void FocusChanged(TDrawNow aDrawNow);

    void HandleResourceChange( TInt aType );
        
public: // from MEikEdwinSizeObserver
        
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aEventType, TSize aDesirableEdwinSize );
        
public: // from MNcsControl

    virtual TInt LineCount() const;

    virtual TInt ScrollableLines() const;

    virtual TInt GetNumChars() const;

    virtual TInt CursorLineNumber() const;

    virtual TInt CursorPosition() const;

    virtual void Reposition(TPoint& aPt, TInt aWidth);

    virtual const TDesC& GetLabelText() const;

    // <cmail> Platform layout change       
    TInt LayoutLineCount() const;
    // </cmail> Platform layout change

    void EnableKineticScrollingL( CAknPhysics* aPhysics );
    
//<cmail>
private:
    
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
//</cmail>
private: // methods
    
    CNcsAddressInputField( 
        TNcsInputFieldType aFieldType,
        MNcsFieldSizeObserver* aSizeObserver,
        MNcsAddressPopupList* aAddressPopupList,
        CNcsHeaderContainer* aParentControl );
        
    void ConstructL( TInt aLabelTextId );
    
    void UpdateFontSize();
    
    //<cmail>
    void CreateControlsL( const TDesC& aControlText );
    
    void LayoutTouch();
    
    void LayoutNonTouch();  
    //</cmail>

private: // data
    	
    CNcsHeaderContainer* iParentControl; // not owned
    
    MNcsAddressPopupList* iAddressPopupList;
    
    /*
    * text field
    * Own
    */
    CNcsLabel* iLabel;
    
// <cmail> Platform layout changes
    const CFont* iFont; // not owned, needs to be released in the end
    
    CAknButton* iButton;
// </cmail> Platform layout changes   
    /*
    * text field
    * Own
    */
    CNcsAifEditor* iTextEditor;
        
    TNcsInputFieldType iFieldType;
        
    TBool iFocusChanged;
        
    TInt iEditorMinimumHeight;
    
// <cmail> Platform layout changes
    TInt iEditorLineCount;
// </cmail> Platform layout changes
       
    TInt iMaximumLabelLength;
        
    RTimer iPopupTimer;
        
    TRgb iBorderColor;
    
    TRgb iBgColor;
    
    // for keeping buttons in view while scrolling
    TPoint iOriginalFieldPos;
    TPoint iOriginalButtonPos;

    // panning related
    CAknPhysics* iPhysics;
    TBool iIsDraggingStarted;
    TPoint iStartPosition;
    };
	
#endif // CNCSADDRESSINPUTFIELD_H
