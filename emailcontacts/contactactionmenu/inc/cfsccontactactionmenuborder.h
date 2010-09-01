/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class CFscContactActionMenuBorder.
*
*/


#ifndef C_FSCCONTACTACTIONMENUBORDER_H
#define C_FSCCONTACTACTIONMENUBORDER_H

#include <e32base.h>
#include <coecntrl.h>

// FORWARD DECLARATIONS

/**
 *  Contact Action Menu Border implementation.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuBorder : public CCoeControl
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @param aRect Rect
     * @param aCornerType Corner type
     *
     * @return New instance of the component  
     */
    static CFscContactActionMenuBorder* NewL( 
        const TRect& aRect,
        TCornerType aCornerType );

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuBorder();
    
public: // From CCoeControl
    
    /**
     * Draw
     *
     * @param aRect rect
     */
    virtual void Draw( const TRect& aRect ) const;

public: // Public methods

    /*
     * Set new rect
     *
     * @param aRect Rect
     */   
    void SetRect( const TRect& aRect );  
    
    /*
     * Set new corner type
     *
     * @param aCornerType Corner type
     */   
    void SetCornerType( TCornerType aCornerType );  
        
    /*
     * Set new color for the border
     *
     * @param aColor
     */   
    void SetColor( TRgb aColor );  

private: // Private constructors

    /**
     * Constructor.
     *
     * @param aRect Rect
     * @param aCornerType Corner type
     */
    CFscContactActionMenuBorder( 
        const TRect& aRect,
        TCornerType aCornerType );

    /**
     * Second phase constructor.
     */
    void ConstructL();
                                       
private: // data

    /*
     * Border rect
     */
    TRect iRect;

    /*
     * Border's corner type
     */
    TCornerType iCornerType;
    
    /**
     * Border's color
     */
    TRgb iColor;
    
    };

#endif // C_FSCCONTACTACTIONMENUBORDER_H

