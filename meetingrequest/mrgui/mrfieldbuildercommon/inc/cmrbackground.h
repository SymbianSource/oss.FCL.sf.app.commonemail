/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Background/focus component for fields.
*
*/

#ifndef CMRBACKGROUND_H
#define CMRBACKGROUND_H

#include <coecntrl.h>
#include "nmrbitmapmanager.h"
#include "cesmrfield.h"

class CESMRLayoutManager;

/**
 * CESMRBorderLayer draws border around the focused control.
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CMRBackground ): public CBase, public MCoeControlBackground
    {
    public:
        
        /**
         * Two-phased constructor.
         *
         * @param aExt Control which has border around it.
         * @param aFocusType
         * @return Pointer to created and initialized esmr borderlayer object.
         */
         IMPORT_C static CMRBackground* NewL( CESMRLayoutManager& aLayoutManager );
    
         /**
          * Destructor.
          */
        IMPORT_C virtual ~CMRBackground();
        
    protected: // From MCoeControlBackground
        IMPORT_C void Draw( CWindowGc &aGc, const CCoeControl &aControl, const TRect &aRect ) const;
        
    private:
        /// Type of the field's background
        enum TBgType
            {
            EEditorFieldNoFocus = 1,
            EEditorFieldWithFocus,
            EViewerFieldWithFocus
            };        
        
        CMRBackground( CESMRLayoutManager& aLayoutManager );
        void CMRBackground::DrawFocus( 
                CWindowGc& aGc, 
                TRect aRect, 
                const CCoeControl& aControl, 
                TBgType aType ) const;
        CArrayFixFlat<NMRBitmapManager::TMRBitmapId>* IconSkinIdL( TBgType aType ) const;
        
    private:
        /// Ref: Layout manager.
        CESMRLayoutManager& iLayoutManager;
    };

#endif // CESMRBORDERLAYER_H
