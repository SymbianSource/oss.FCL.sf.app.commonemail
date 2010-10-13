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
 * Interface for the attachment field observer
 */
class MNcsAttachmentFieldObserver
    {
public:
    /**
     * AttachmentOpenL
     * Informs observer that focused attachment should be opened
     */
    virtual void AttachmentOpenL() = 0;

    /**
     * AttachmentRemoveL
     * Informs observer that focused attachment should be removed
     */
    virtual void AttachmentRemoveL() = 0;
    };


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
        MNcsAttachmentFieldObserver* aObserver,
        CNcsHeaderContainer* aParentControl );

    virtual ~CNcsAttachmentField();

public: // methods

    TInt MinimumHeight() const;

    void SetMaxLabelLength( TInt aMaxLength );

    TInt GetMinLabelLength() const;
    
    void SetTextsLD( CDesCArray* aAttachmentNames, 
                     CDesCArray* aAttachmentSizes );

    TInt FocusedAttachmentLabelIndex() const;

    void SetFocusedAttachmentLabelIndex( TInt aIndex );

public: // from MNcsControl
    
    TInt LineCount() const;

    TInt ScrollableLines() const;

    TInt GetNumChars() const;

    TInt CursorLineNumber() const;

    TInt CursorPosition() const;

    void Reposition( TPoint& aPt, TInt aWidth );

    const TDesC& GetLabelText() const;
    
    TInt LayoutLineCount() const;
    
public: // from CCoeControl

    void Draw( const TRect& aRect ) const;
        
    void SizeChanged();

    void PositionChanged();

    void SetContainerWindowL( const CCoeControl& aContainer );

    void FocusChanged( TDrawNow aDrawNow );

    void HandleResourceChange( TInt aType );

    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

private: // methods

    CNcsAttachmentField( TInt aLabelTextId,
                         MNcsFieldSizeObserver* aSizeObserver, 
                         MNcsAttachmentFieldObserver* aObserver,
                         CNcsHeaderContainer* aParentControl );

    void ConstructL();
    
    void UpdateColors();
    void UpdateColors( CNcsLabel* aLabel );
    
	void UpdateFontSize();
	void UpdateFontSize( CNcsLabel* aLabel );

    void LayoutControls();
	
	void UpdateIconPositions( const TRect& aRect );

    void UpdateAttachmentTextsL();
    
    /**
     * Resizes icons 
     */
    void ResizeIcons();
    
    TInt CreateIcons();
    void CreateIconsL();
    
    void UpdateComponentArrayL();
    
    void UpdateSingleAttachmentLabelTextL( CNcsLabel* aLabel, TInt aIndex );
    
private: // data

    CNcsHeaderContainer* iParentControl; // not owned

    // attachment field observer
    MNcsAttachmentFieldObserver* iObserver;

    // attachment labels (within the array) owned
    RPointerArray<CNcsLabel> iAttachmentLabels;
    
    const CFont* iLabelFont; // not owned
    const CFont* iEditorFont; // not owned
    
    CDesCArray* iAttachmentNames;
    CDesCArray* iAttachmentSizes;
    
    TInt iMaximumLabelLength;         

    TRgb iBgColor;
    
    TRgb iBorderColor;
    
    // Attachment icon
    CFbsBitmap* iAttachmentBitmap;
    CFbsBitmap* iAttachmentMask;
    TPoint iAttachmentIconPos;
    
    TInt iLabelTextId;
    TInt iAttachmentLabelCount;
    TInt iFocusedLabelIndex;
    };

#endif
