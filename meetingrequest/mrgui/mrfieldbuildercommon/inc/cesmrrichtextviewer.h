/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*  Description : CEikRichTextEditor based Rich Text viewer
*  Version     : %version: tr1sido#4.1.4 %
*
*/

#ifndef CESMRRICHTEXTVIEWER_H
#define CESMRRICHTEXTVIEWER_H

#include <eikrted.h>
#include <cntitem.h>
#include <eikcmobs.h>
#include <aknlongtapdetector.h>

#include "resmrstatic.h"

class CESMRRichTextLink;
class CESMRLayoutManager;
class MESMRFieldEventQueue;

// SCROLLING_MOD: List observer forward declaraion
class MESMRListObserver;

/*
 * Pure virtual link selection observer.
 */
class MESMRRichTextObserver
    {
public:
    /*
     * CESMRichTextViewer calls this function with selected link before
     * showing Contact Action Menu. If observer handles link selection it
     * should return ETrue, in which case CESMRRichTextViewer won't handle
     * it.
     */
    virtual TBool
            HandleRichTextLinkSelection(const CESMRRichTextLink* aLink ) = 0;
    };

/*
 * CEikRichTextEditor based Rich Text viewer.
 */
NONSHARABLE_CLASS( CESMRRichTextViewer ) : public CEikRichTextEditor,
                                           public MEikCommandObserver, 
                                           public MAknLongTapDetectorCallBack
    {
public:
    /*
     * Two-phase constructor.
     * @param aParent if aParent is NULL, 
     * 				  CEikRichTextEditor is constucted as 
     * 				  window owning control.
     * @return Created object
     */
    IMPORT_C static CESMRRichTextViewer* NewL(
            const CCoeControl* aParent = NULL);

    /*
     * Destructor.
     */
    IMPORT_C ~CESMRRichTextViewer( );

    // From CCoeControl
    IMPORT_C void PositionChanged( );
    IMPORT_C void FocusChanged( TDrawNow aDrawNow );
    IMPORT_C TKeyResponse OfferKeyEventL(
    		const TKeyEvent &aKeyEvent, 
    		TEventCode aType );

    IMPORT_C void SetMargins( TInt sMargin );

    IMPORT_C void SetFontL( 
    		const CFont* aFont, 
    		CESMRLayoutManager* aLayout = NULL );

    /*
     * Sets CEikRichTextEditor's text and searches text for emails,
     * phone numbers and urls if aSearchLinks is set to ETrue.
     *
     * @param aText text to be set for CEikRichTextEditor
     * @param aSearchLinks search emails, phone numbers and urls
     */
    IMPORT_C void SetTextL(const TDesC* aText, TBool aSearchLinks = EFalse );

    /*
     * Sets observer for link selections.
     * @param aLinkObserver pointer to observer object
     */
    IMPORT_C void SetLinkObserver(MESMRRichTextObserver* aLinkObserver);

    /*
     * Adds link to text.
     * @param aLink link to be added
     */
    IMPORT_C void AddLinkL(CESMRRichTextLink* aLink );
    
    /**
     * Inserts link to text.
     * @param aLink link to be added
     * @param aPosition position where link is added
     */
    IMPORT_C void InsertLinkL( CESMRRichTextLink* aLink, TInt aPosition );

    /*
     * Returns selected link, or NULL if none selected.
     */
    IMPORT_C const CESMRRichTextLink* GetSelectedLink( ) const;

    /*
     * Returns text for selected link from underlying CEikRichTextViewer.
     *
     * @param aLink link
     */
    IMPORT_C HBufC* GetLinkTextL( const CESMRRichTextLink& aLink ) const;

    /*
     * Returns pointer to new CContactItem constructed from aLink.
     * Function uses CESMRRichTextLink::Value to create contact
     * with CESMRRichTextLink::Type information.
     *
     * @param aLink link
     */
    IMPORT_C CContactItem* CreateContactItemL( const CESMRRichTextLink& aLink );

 
    /*
    * Method for cesmrviewerdescriptionfield to set the list observer
    *
    * @param aObserver observer that should be notified
    */
    IMPORT_C void SetListObserver( MESMRListObserver* aObserver );
    
    /**
     * Returns height of one row.
     *
     * @return row's height
     */
    IMPORT_C TInt RowHeight();
    /**
     * Returns text line count.
     *
     * @return line count
     */
    IMPORT_C TInt LineCount();
    /**
     * Returns line number of currenly 
     * focused row starting from one.
     * 
     * @return row's height
     */
    IMPORT_C TInt CurrentLineNumber();

    /**
     * Enables and disables action menu
     * @param aStatus Enables or disables action menu
     */
    IMPORT_C void SetActionMenuStatus( TBool aStatus );

    /**
     * Copies currently selected richtext link associated with 
     * action menu to the clipboard
     */
    IMPORT_C void CopyCurrentLinkToClipBoardL() const;

    /**
     * Copies currently link value to clipboard 
     * action menu to the clipboard
     */
    IMPORT_C void CopyCurrentLinkValueToClipBoardL() const;

    /**
     * convenience method to reset TLS static contactactionmenuhandler 
     * for classes that only have access to richtextviewer. 
     * Needed for richtextviewer fields when they lose focus.
     */
    IMPORT_C void ResetActionMenuL() const;

    /**
     * Sets event queue for field. Observer can be used
     * for triggering commands and notifying events.
     * @param aEventObserver the event observer
     */
    IMPORT_C void SetEventQueue( MESMRFieldEventQueue* aEventQueue );

    /**
     * Selects current link.
     * @return ETrue if link selected
     */
    IMPORT_C TBool LinkSelectedL();
    
protected: // From MEikCommandObserver
    
    /**
     * Process commands from contact menu handler.
     * Forwards commands to event observer. 
     */
    void ProcessCommandL( TInt aCommandId );

protected: // From MAknLongTapDetectorCallBack
    virtual void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                      const TPoint& aPenEventScreenLocation );
    
protected: // From CEikEdwin
    /**
     * Sets the control as ready to be drawn.
     */
    void ActivateL();

protected: // From CCoeControl
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
private:
    /*
     * Private constuctor.
     */
    CESMRRichTextViewer( );

    /*
     * Two-phase constructor.
     * @param aParent if aParent is NULL, CEikRichTextEditor 
     * 				  is constucted as window owning control.
     */
    void ConstructL(const CCoeControl* aParent );

    /*
     * CCoeControl::Draw is overridden.
     */
    void Draw( const TRect& aRect ) const;

    /*
     * Draws right-click icon.
     * @param aLink link to which draw icon
     */
    void DrawRightClickIconL(const CESMRRichTextLink& aLink) const;

    /*
     * Highlights link in CEikRichTextEditor.
     *
     * @param aLink link to be highlighted
     */
    void HighlightLink(const CESMRRichTextLink& aLink );

    /*
     * Private internal function which searches for emails,
     * phone numbers and urls from given text using CFindItemEngine.
     *
     * @param aText text which is shown in CEikRichTextEditor
     */
    void SearchLinksL(const TDesC& aText );

    void ScrollViewL( 
    		TInt aNumberOfRows, 
    		TCursorPosition::TMovementType aDirection);
    TInt FindTextLinkBetweenNextScrollArea(
    		TInt aStartRow,                  
    		TInt aEndRow,                               
    		TCursorPosition::TMovementType aDirection);

    TBool SetHighLightToNextLinkL( TCursorPosition::TMovementType aDirection,
                                   TInt aStartRow,
                                   TInt aEndRow);
    TInt ValidLinkForFocusing(	TInt aIndex,
                                TCursorPosition::TMovementType aDirection,
                                TInt aStartRow,
                                TInt aEndRow);

    void SetFontColorL( TBool aFocused = EFalse );

    void SetValueL( const CESMRRichTextLink& aLink );

private: // Data
	/// Own:
    TInt iCurrentLinkIndex;
    /// Own:
    RPointerArray<CESMRRichTextLink> iLinkList;
    /// Ref:
    MESMRRichTextObserver* iLinkObserver;
    /// Own:
    RESMRStatic iESMRStatic;
    /// Own:
    CFbsBitmap* iActionMenuIcon;
    /// Own:
    CFbsBitmap* iActionMenuIconMask;
    /// Ref:
    MESMRListObserver* iObserver;
    /// Own:
    TInt iNumberOfLines;
    /// Own:
    TInt iApproximatelyRowHeight;
    /// Ref: pointer to font
    const CFont* iFont;
    /// Ref: Pointer to layout manager
    CESMRLayoutManager* iLayout;
    /// Ref: Contact Menu handler
    CESMRContactMenuHandler* iCntMenuHdlr;
    /// Ref: Pointer to event queue
    MESMRFieldEventQueue* iEventQueue;
    /// Own: Flag for action menu status
    TBool iActionMenuStatus;
    /// Own:
    TCharFormat iFormat;
    /// Own:
    TCharFormatMask iFormatMask;
    // Own:    
    CAknLongTapDetector* iLongTapDetector;
    // Indicates is action menu opened
    TBool iActionMenuOpen;
    };

#endif /*CESMRRICHTEXTVIEWER_H*/
