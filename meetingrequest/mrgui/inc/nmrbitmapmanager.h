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
* Description: Bitmap manager for MRGUI usage.
*
*/

#ifndef NMRBITMAPMANAGER_H
#define NMRBITMAPMANAGER_H

#include <e32base.h>
#include <AknsItemID.h>

// FORWARD DECLARATIONS
class CFbsBitmap;

namespace NMRBitmapManager
    {    
    enum TMRBitmapId 
        {
        EMRBitmapNotSet = 0,
        EMRBitmapRightClickArrow = 1,
        EMRBitmapTrackingAccept,
        EMRBitmapTrackingReject,
        EMRBitmapTrackingTentative,
        EMRBitmapTrackingNone,
        EMRBitmapPriorityHigh,
        EMRBitmapPriorityLow,
        EMRBitmapPriorityNormal,
        EMRBitmapSynchronization,
        EMRBitmapSubject,
        EMRBitmapOccasion,
        EMRBitmapRecurrence,
        EMRBitmapBrowser,
        EMRBitmapIntranet,
        EMRBitmapBookmark,
        EMRBitmapAlarm,
        EMRBitmapAlarmDate,
        EMRBitmapAlarmTime,
        EMRBitmapAlarmClock,
        EMRBitmapCheckBoxOn,
        EMRBitmapCheckBoxOff,
        EMRBitmapClock,
        EMRBitmapLocation,
        EMRBitmapDateStart,
        EMRBitmapDateEnd,
        EMRBitmapAttachment,
        EMRBitmapCtrlBar,
        
        // Attachment handling icons
        EMRBitmapMailAttachment,
        EMRBitmapOpenAttachmentView,
        EMRBitmapOpenAttachment,
        EMRBitmapDownloadAttachment,
        EMRBitmapDownloadAllAttachments,
        EMRBitmapSaveAttachment,
        EMRBitmapSaveAllAttachments,
                    
        EMRBitmapListCenter,
        EMRBitmapListTop,
        EMRBitmapListBottom,
        EMRBitmapListLeft,
        EMRBitmapListRight,
        EMRBitmapListTopLeft,
        EMRBitmapListTopRight,
        EMRBitmapListBottomLeft,
        EMRBitmapListBottomRight,
        
        EMRBitmapInputCenter,
        EMRBitmapInputTop,
        EMRBitmapInputBottom,
        EMRBitmapInputLeft,
        EMRBitmapInputRight,
        EMRBitmapInputTopLeft,
        EMRBitmapInputTopRight,
        EMRBitmapInputBottomLeft,
        EMRBitmapInputBottomRight,

        EMRBitmapSetOptCenter,
        EMRBitmapSetOptTop,
        EMRBitmapSetOptBottom,
        EMRBitmapSetOptLeft,
        EMRBitmapSetOptRight,
        EMRBitmapSetOptTopLeft,
        EMRBitmapSetOptTopRight,
        EMRBitmapSetOptBottomLeft,
        EMRBitmapSetOptBottomRight            
        };
    
    class TMRBitmapStruct 
        {
        public:
            TMRBitmapStruct() : 
                iFallbackIndex( 0 ), iFallbackMaskIndex( 0 ) 
                    { iItemId.iMajor = 0; iItemId.iMinor = 0; }
            
        public: // Data
            TAknsItemID iItemId;
            TInt iFallbackIndex;
            TInt iFallbackMaskIndex;
        };
    
    /**
     * Converts logical TMRBitmapId to internal TMRBitmapStruct
     * which contains skin id for bitmap and fallback ids.
     * 
     * @param aBitmapId Logical bitmap id.
     * @return TAknsItemID Skin graphics id.
     */
    IMPORT_C TMRBitmapStruct GetBitmapStruct( TMRBitmapId aBitmapId );
    
    /**
     * Creates bitmap and bitmap mask. Bitmap is skin/theme based
     * and it's retrived by the given ID.Bitmap is also resized to
     * the given size.
     * Ownership of these created bitmaps is transferred to the caller.
     *
     * @param aIconID - id of the wanted bitmap
     * @param aBitmap - reference to the pointer where new bitmap is stored
     * @param aMask - reference to the pointer where new bitmap mask is stored
     * @param aSize - Desired size for the bitmap
     * @return TInt - KErrNone if bitmap was constructed succesfully
     *                KErrNotFound if any problems occured
     */
    IMPORT_C TInt GetSkinBasedBitmap( TMRBitmapId aBitmapId,
                                      CFbsBitmap*& aBitmap,
                                      CFbsBitmap*& aMask,
                                      TSize aSize );
    
    }

#endif
