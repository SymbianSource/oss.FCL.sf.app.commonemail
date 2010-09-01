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
* Description: This file defines class CNcsEditor.
*
*/



#ifndef CNCSEDITOR_H
#define CNCSEDITOR_H


#include <eikrted.h>
#include <AknUtils.h>
#include "ncscontrol.h"
#include "ncsutility.h"
#include "ncsconstants.h"

class CAknsBasicBackgroundControlContext;
class CNcsCustomDraw;
class CNcsEditorCustomDraw;

/**
*  CNcsEditor
*/
class CNcsEditor : public CEikRichTextEditor, public MNcsControl, public MCoeCaptionRetrieverForFep
    {
public: // construction and destruction
    
    CNcsEditor(
        MNcsFieldSizeObserver* aSizeObserver = NULL,
        TBool aHeaderField = ETrue,
        TNcsEditorUsage aEditorUsage = ENcsEditorDefault,
        const TDesC& aCaptionText = TPtrC() );
    
    void ConstructL( const CCoeControl* aParent,
                     TInt aNumberOfLines,
                     TInt aTextLimit );    

    virtual ~CNcsEditor();

public: // new functions
    
    void OpenVirtualKeyBoardL();
    
    void GetLineRectL( TRect& aLineRect ) const;
        
    virtual TInt ScrollableLines() const;
    
    virtual TInt CursorLineNumber() const;
    
    virtual TInt LineCount() const;
        
    virtual TInt CursorPosition() const;
        
    virtual const TDesC& GetLabelText() const;
		
    // <cmail> Platform layout change       
    TInt LayoutLineCount() const;
    // </cmail> Platform layout change    
    
    virtual void Reposition(TPoint& /*aPt*/, TInt /*aWidth*/) {}

    void SetMaximumLabelLength( TSize aSize );
		
    void SetupEditorL();
		
    void PositionChanged();
        
    TInt TextHeight() const;
        
    TInt GetLineHeightL() const;
        
    TInt GetNumChars() const;
        
    TInt DocumentLength() const;

    TInt PixelsAboveBand() const;
    
    TInt ChangeBandTopL(TInt aPixels);
    
	void UpdateFontSize();
	
	TBool HasSemicolonAtTheEndL() const;
	
	//<cmail>
	void SetRealRect( TRect aRect );
	
	void FormatAllTextNowL();
	//<cmail>

	void UpdateCustomDrawer();
	
	void SetPhysicsEmulationOngoing( TBool aPhysOngoing );
	
	TBool IsPhysicsEmulationOngoing() const;
	
#ifdef _DEBUG
    void DebugDump();
#endif
        
    void SetTextColorL( TLogicalRgb aColor );

    TInt TrimmedTextLength();

// <cmail> Platform layout changes   
    void SetNcsFontType( NcsUtility::TNcsLayoutFont aFontType );
// </cmail> Platform layout changes

public: // from CoeControl

    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void ActivateL();

public: // from CEikEdwin

    virtual void SetTextL( const TDesC* aDes );

    virtual void HandleResourceChange( TInt aType );

protected: // from CEikEdwin    
    
    //<cmail> Platform layout change
    // virtual CLafEdwinCustomDrawBase* CreateCustomDrawL();
    // </cmail>

protected: // from MEditObserver

    virtual void EditObserver( TInt aStart, TInt aExtent );
    
public: // CEikEdwin
    TCoeInputCapabilities InputCapabilities() const;  
public: // MCoeCaptionRetrieverForFep
    void GetCaptionForFep(TDes& aCaption) const;

private: // methods used internally    
	// <cmail> Platform layout change 
    void UpdateFontL();
    // </cmail> Platform layout change 
	
    void UpdateColors();

    TAknTextComponentLayout GetLayout();

    void Draw( const TRect& aRect ) const;

private: // data
    
    TInt iPreviousFontHeight;
    TInt32 iPreviousLineSpacingInTwips;

    TRgb iTextColor;
    CCharFormatLayer* iGlobalCharFormat;
    
    // Flag indicating whether this control is header field.
    // Header fields have different backgrounds than message field.
    TBool iHeaderField;

    // Custom drawer. Not owned.
    CNcsCustomDraw* iCustomDrawer;

    // Platform layout change
    NcsUtility::TNcsLayoutFont iNcsFontType;

    // Custom drawer for row lines. Own  
    CNcsEditorCustomDraw* iEditorCustomDrawer;
    
    // what purpose editor is used to
    TNcsEditorUsage iEditorUsage;
    
    // Real screen rectangle
    TRect iRealRect;

	// Caption text for virtual ITU-T
    HBufC*   iCaptionText; 
    
    // flag to indicate is there physics based dragging or flicking ongoing
    TBool iPhysOngoing;
    };


#endif
