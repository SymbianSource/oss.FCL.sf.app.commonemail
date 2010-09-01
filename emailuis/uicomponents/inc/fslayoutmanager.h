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
* Description: This file defines class CFsLayoutManager.
*
*/



#ifndef C_FSLAYOUTMANAGER_H
#define C_FSLAYOUTMANAGER_H

#include <gdi.h>

// <cmail> S60 UID update
const TInt KFsMessageLayoutChange( 0x2001E278 );
// </cmail> S60 UID update

//<cmail>
//A lot of stuff removed, dependencies to FSLayout REMOVED!!
//</cmail>

/**
 *  This class knows the specification's coordinate data format and ensures
 *  that different types of controls are positioned and setup correctly.
 *  Methods in this class are designed to be called from your control's
 *  SizeChanged() method!
 *
 *  @code
 *
 *   TRect parentRect;
 *   TRect layoutRect;
 *   // Request screen size from layout manager.
 *   // When requesting screen size parentRect is only needed by the
 *   // interface.
 *   CFsLayoutManager::LayoutMetricsRect( parentRect,
 *       CFsLayoutManager::EFsLmScreen, layoutRect );
 *   // Set parentRect to be the requested screen rect.
 *   parentRect = layoutRect;
 *   // Request position and size of application window.
 *   CFsLayoutManager::LayoutMetricsRect( parentRect,
 *       CFsLayoutManager::EFsLmApplicationWindow, layoutRect );
 *   // Corresponding value need to be set in parentRect when requesting it's
 *   // child pane.
 *   parentRect = layoutRect;
 *   // Request area top pane layout data.
 *   // Assume that landscape mode is in use. Check the variety values from
 *   // layout documents.
 *   CFsLayoutManager::LayoutMetricsRect( parentRect,
 *       CFsLayoutManager::EFsLmAreaTopPane, layoutRect, 1 );
 *
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFsLayoutManager : public CBase
	{

public:

    /**  Layout Metrics. */
    enum TFsLayoutMetrics
        {
        EFsLmApplicationWindow,
        EFsLmAreaTopPane,
        EFsLmBgSpFsCtrlbarPane,
        EFsLmFolderListMediumLineG1, // Folder view layout
		EFsLmFolderListMediumLineT1, // Folder view layout
        EFsLmListMediumLineG1,
        EFsLmListMediumLineG2G1,
        EFsLmListMediumLineG2G2,
        EFsLmListMediumLineG2T1,
        EFsLmListMediumLineG3G1,
        EFsLmListMediumLineG3G2,
        EFsLmListMediumLineG3G3,
        EFsLmListMediumLineG3T1,
        EFsLmListMediumLinePlainT1,
        EFsLmListMediumLinePlainT2T1,
        EFsLmListMediumLinePlainT2T2,
        EFsLmListMediumLinePlainT3T1,
        EFsLmListMediumLinePlainT3T2,
        EFsLmListMediumLinePlainT3T3,
        EFsLmListMediumLineRightIconG1,
        EFsLmListMediumLineRightIconT1,
        EFsLmListMediumLineRightIconx2G1,
        EFsLmListMediumLineRightIconx2G2,
        EFsLmListMediumLineRightIconx2T1,
        EFsLmListMediumLineT1,
        EFsLmListMediumLineT2G1,
        EFsLmListMediumLineT2G2G1,
        EFsLmListMediumLineT2G2G2,
        EFsLmListMediumLineT2G2T1,
        EFsLmListMediumLineT2G2T2,
        EFsLmListMediumLineT2G3G1,
        EFsLmListMediumLineT2G3G2,
        EFsLmListMediumLineT2G3G3,
        EFsLmListMediumLineT2G3T1,
        EFsLmListMediumLineT2G3T2,
        EFsLmListMediumLineT2RightIconG1,
        EFsLmListMediumLineT2RightIconT1,
        EFsLmListMediumLineT2RightIconT2,
        EFsLmListMediumLineT2RightIconx2G1,
        EFsLmListMediumLineT2RightIconx2G2,
        EFsLmListMediumLineT2RightIconx2T1,
        EFsLmListMediumLineT2RightIconx2T2,
        EFsLmListMediumLineT2T1,
        EFsLmListMediumLineT2T2,
        EFsLmListMediumLineT32RightIconT2,
        EFsLmListMediumLineT3G1,
        EFsLmListMediumLineT3G2G1,
        EFsLmListMediumLineT3G2G2,
        EFsLmListMediumLineT3G2T1,
        EFsLmListMediumLineT3G2T2,
        EFsLmListMediumLineT3G2T3,
        EFsLmListMediumLineT3G3G1,
        EFsLmListMediumLineT3G3G2,
        EFsLmListMediumLineT3G3G3,
        EFsLmListMediumLineT3G3T1,
        EFsLmListMediumLineT3G3T2,
        EFsLmListMediumLineT3G3T3,
        EFsLmListMediumLineT3RightIconG1,
        EFsLmListMediumLineT3RightIconT1,
        EFsLmListMediumLineT3RightIconT2,
        EFsLmListMediumLineT3RightIconx2G1,
        EFsLmListMediumLineT3RightIconx2G2,
        EFsLmListMediumLineT3RightIconx2T1,
        EFsLmListMediumLineT3RightIconx2T2,
        EFsLmListMediumLineT3RightIconx2T3,
        EFsLmListMediumLineT3T1,
        EFsLmListMediumLineT3T2,
        EFsLmListMediumLineT3T3,
        EFsLmListMediumLineT4G4T3,
        EFsLmListMediumLineX2,
        EFsLmListMediumLineX2G1,
        EFsLmListMediumLineX2T2G2T1,
        EFsLmListMediumLineX2T2G3T1,
        EFsLmListMediumLineX2T2G4T1,
        EFsLmListMediumLineX2T3G2T1,
        EFsLmListMediumLineX2T3G2T2,
        EFsLmListMediumLineX2T3G3G1,
        EFsLmListMediumLineX2T3G3G2,
        EFsLmListMediumLineX2T3G3G3,
        EFsLmListMediumLineX2T3G3T1,
        EFsLmListMediumLineX2T3G3T2,
        EFsLmListMediumLineX2T3G4G1,
        EFsLmListMediumLineX2T3G4G2,
        EFsLmListMediumLineX2T3G4G3,
        EFsLmListMediumLineX2T3G4T1,
        EFsLmListMediumLineX2T3G4T2,
        EFsLmListMediumLineX2T3G4T3,
        EFsLmListMediumLineX2T3T3,
        EFsLmListMediumLineX2T4G4G4,
        EFsLmListMediumLineX3,
        EFsLmListMediumLineX3T4G4G4,
        EFsLmListMediumLineX3T4G4T2,
        EFsLmListMediumLineX3T4G4T3,
        EFsLmListMediumLineX3T4G4T4,
        EFsLmListSingleDycRowPane,
        EFsLmListSingleDycRowPaneG1,
        EFsLmListSingleDycRowPaneG2,
        EFsLmListSingleDycRowPaneG3,
        EFsLmListSingleDycRowPaneG4,
        EFsLmListSingleDycRowTextPane,
        EFsLmListSingleDycRowTextPaneT1,
        EFsLmListSingleDycRowTextPaneT2,
        EFsLmListSingleDycRowTextPaneT3,
        EFsLmListSingleFsDycPane,
        EFsLmMainPane,
        EFsLmMainSpFsCtrlbarButtonPaneCp01,
        EFsLmMainSpFsCtrlbarDdmenuPane,
        EFsLmMainSpFsCtrlbarDdmenuPaneG1,
        EFsLmMainSpFsCtrlbarDdmenuPaneG2,
        EFsLmMainSpFsCtrlbarDdmenuPaneT1,
        EFsLmMainSpFsCtrlbarPane,
        EFsLmMainSpFsCtrlbarPaneG1,
        EFsLmMainSpFsCtrlbarPaneG2,
        EFsLmMainSpFsListPane,
        EFsLmMainSpFsListscrollPaneTe,
        EFsLmPopupSpFsActionMenuPane,
        EFsLmScreen,
        EFsLmSpFsScrollPane,
        EFsLmSpFsActionMenuListGenePaneG1,
        EFsLmSpFsActionMenuListGenePaneT1,
        
        // command button area
        
        ECmailDdmenuBtn01Pane,
        ECmailDdmenuBtn01PaneG1,
        ECmailDdmenuBtn01PaneT1,
        ECmailDdmenuBtn01PaneG2,
        ECmailDdmenuBtn01PaneCP,
        
        ECmailDdmenuBtn02Pane,
        ECmailDdmenuBtn02PaneG1,
        ECmailDdmenuBtn02PaneG2,
        ECmailDdmenuBtn02PaneT1,
        ECmailDdmenuBtn02PaneT2,
        ECmailDdmenuBtn02PaneCP,

        ECmailDdmenuBtn03Pane,
        ECmailDdmenuBtn03PaneG1,
        ECmailDdmenuBtn03PaneT1,
        ECmailDdmenuBtn03PaneCP
        };

    /**  Collection of data of text position and font. */
    struct TFsText
        {
        TRect iTextRect;
        const CFont *iFont; // not owned
        //TRgb iColor;
        TInt iOffset;
        CGraphicsContext::TTextAlign iAlign;
        };

    IMPORT_C CFsLayoutManager();

    /**
	 * Determine if the mirroring is in use.
	 *
     * @return ETrue If mirroring is in use. EFalse Otherwise.
     */
    IMPORT_C static TBool IsMirrored();

    /**
	 * Fills given TRect with rectangle for given layout component. Returns
	 * EFalse for status pane descendants if requested layout component is not
	 * available in current layout. For other components returns always ETrue
	 * (returned rectangle is from layout definition).
     *
     * @param aParent Rect of the parent layout.
     * @param aParam Layout component to be queried.
     * @param aRect Resulting rectangle.
     * @param aVariety Defines items variety value. Check this from layout
     * documents.
     * @param aRow Possible row value for queried item.
     * @param aCol Possible column value for queried item.
     * @return ETrue If requested value was available. EFalse Otherwise.
     */
	IMPORT_C static TBool LayoutMetricsRect(
        const TRect &aParent, TFsLayoutMetrics aParam, TRect& aRect,
        TInt aVariety = 0, TInt aRow = 0, TInt aCol = 0 );

    /**
	 * This method returns size of rectangle for given layout component.
	 * Returns EFalse for status pane descendants if requested layout
	 * component is not available in current layout. For other components
	 * returns always ETrue (returned size is from layout definition).
     *
     * @param aParent Rect of the parent layout.
     * @param aParam Layout component to be queried.
     * @param aSize Resulting size.
     * @param aVariety Defines items variety value. Check this from layout
     * documents.
     * @param aRow Possible row value for queried item.
     * @param aCol Possible column value for queried item.
     * @return ETrue If requested value was available. EFalse Otherwise.
     */
    IMPORT_C static TBool LayoutMetricsSize(
        const TRect &aParent, TFsLayoutMetrics aParam, TSize& aSize,
        TInt aVariety = 0, TInt aRow = 0, TInt aCol = 0 );

    /**
	 * This method returns position of top left corner for given layout
	 * component. Returns EFalse for status pane descendants if requested
	 * layout component is not available in current layout. For other
	 * components returns always ETrue (returned position is from layout
	 * definition).
     *
     * @param aParent Rect of the parent layout.
     * @param aParam Layout component to be queried.
     * @param aPos Resulting position.
     * @param aVariety Defines items variety value. Check this from layout
     * documents.
     * @param aRow Possible row value for queried item.
     * @param aCol Possible column value for queried item.
     * @return ETrue If requested value was available. EFalse Otherwise.
     */
    IMPORT_C static TBool LayoutMetricsPosition(
        const TRect &aParent, TFsLayoutMetrics aParan, TPoint& aPos,
        TInt aVariety = 0, TInt aRow = 0, TInt aCol = 0 );

    /**
     * This method returns text position and font information of given layout
     * component. Returns EFalse for status pane descendants if requested
     * layout component is not available in current layout. For other
     * components returns always ETrue (returned position is from layout
     * definition).
     *
     * @param aParent Rect of the parent layout.
     * @param aParam Layout component to be queried.
     * @param aText Resulting text information.
     * @param aVariety Defines items variety value. Check this from layout
     * documents.
     * @param aRow Possible row value for queried item.
     * @return ETrue If requested value was available. EFalse Otherwise.
     */
    IMPORT_C static TBool LayoutMetricsText(
        const TRect &aParent, TFsLayoutMetrics aParam, TFsText& aText,
        TInt aVariety = 0, TInt aRow = 0 );

private: // data

	};


#endif // C_FSLAYOUTMANAGER_H
