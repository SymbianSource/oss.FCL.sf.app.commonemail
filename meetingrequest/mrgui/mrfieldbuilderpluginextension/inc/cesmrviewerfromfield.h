/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Meeting request from (organizer) field
 *
*/


#ifndef CESMRVIEWERFROMFIELD_H_
#define CESMRVIEWERFROMFIELD_H_

// INCLUDES
#include <e32base.h>
#include <fbs.h>
#include <e32cmn.h>
#include <eikedwob.h>

#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CESMRRichTextViewer;
class CMRLabel;

/**
 * Field for showing the "from" information. In other words this field shows
 * that who sent/organized this meeting request.
 */
NONSHARABLE_CLASS( CESMRViewerFromField ) : public CESMRField, 
                                                   MEikEdwinSizeObserver
    {
    public:
        /**
         * Constructor for new From field used in viewer mode.
         *
         * @return New viewer from field object
         */
        static CESMRViewerFromField* NewL( );
    
        /**
         * C++ Destructor.
         */
        ~CESMRViewerFromField( );
    
    public: // From CESMRField
        void InternalizeL( MESMRCalEntry& aEntry );
        void InitializeL();
        TBool ExecuteGenericCommandL( TInt aCommand );
        void SetOutlineFocusL( TBool aFocus );
        void HandleLongtapEventL( const TPoint& aPosition );
    
    public: // From CCoeControl
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aEvent, 
                TEventCode aType );
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aInd ) const;
        void SizeChanged();
        TSize MinimumSize();
        void SetContainerWindowL(const CCoeControl& aContainer);
        
    public: // From MEikEdwinSizeObserver
        TBool HandleEdwinSizeEventL( 
                CEikEdwin* aEdwin, 
                TEdwinSizeEvent aType,
                TSize aSize );
    
    private: // Implementation
        CESMRViewerFromField( );
        void ConstructL( );
    
    private: // data
        // Not own: Actual text of this field, not owned
        CESMRRichTextViewer* iRichTextViewer;
        // Own: Topic text of this fiels
        CMRLabel* iFieldTopic;
        // Size of the field when after field has expanded.
        TSize iExpandedSize;
        // Size of the title
        TSize iTitleSize;
    };

#endif /*CESMRVIEWERFROMFIELD_H_*/

// End of file

