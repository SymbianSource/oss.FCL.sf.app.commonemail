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
* Description:  ESMR GUI Location plugin handler
*
*/

#ifndef CESMRLOCATIONPLUGINHANDLER_H
#define CESMRLOCATIONPLUGINHANDLER_H

#include <e32base.h>
#include "mesmrlocationpluginobserver.h"

class CESMRLocationPlugin;
class MESMRCalEntry;
class CESMRUrlParserPlugin;
class CESMREditorDialog;
class CESMRLocationHistoryManager;
class MObjectProvider;

/**
* CESMRLocationPluginHandler encapsulates the handling of esmr location commands
* with ESMR Location Plugin
* 
* @lib esmrgui.lib
*/
NONSHARABLE_CLASS(CESMRLocationPluginHandler) : public CBase,
												public MESMRLocationPluginObserver					
    {
public: // Construction and destruction    
    /**
     * Creates new CESMRViewerCmdHandler object. Ownership
     * is transferred to caller.
     * @return Pointer to created CESMRViewerCmdHandler object.
     */
     static CESMRLocationPluginHandler* NewL( MObjectProvider& aParent );     
     
    /**
     * C++ destructor
     */
    ~CESMRLocationPluginHandler();

    /**
     * Handles a Location plugin command from editor.
     * 
     * @param aCommand the command to handle
     * @param aEntry   the entry to handle via command
     * @param aIgnoreQuery if ETrue
     * "Replace previous location" query is not shown.
     * @return ETrue if entry was updated during command processing
     */
    TBool HandleCommandL( TInt aCommand,
                         MESMRCalEntry& aEntry,
                         TBool aIgnoreQuery = EFalse );
    
    /**
     * Verifies if given command can be handled for given entry.
     * 
     * @param aCommand the command to verify
     * @param aEntry the entry for command
     * @return ETrue if the command can be handled using HandleCommandL
     *               for the entry.
     */
    TBool IsCommandAvailableL( TInt aCommand, const MESMRCalEntry& aEntry );
    
    /**
     * Stores calendar entry's location into location history.
     * 
     * @param aEntry calendar entry which's location is stored.
     */
    void StoreLocationToHistoryL( const MESMRCalEntry& aEntry );
    
    /**
     * Updates entry location field.
     * @param aEntry entry to update the location field
     * @param aLocation location text to update into entry
     * @param aIgnoreQuery if ETrue, "Replace previous location" query is
     * not displayed.
     * @param aLocationReplaced on return, ETrue if previous location was replaced
     * @return ETrue if aEntry location field was modified. 
     */
    static TBool UpdateEntryLocationL( MESMRCalEntry& aEntry,
                                       const TDesC& aLocation,
                                       TBool aIgnoreQuery,
                                       TBool& aLocationReplaced ); 
    
protected: // From MESMRLocationPluginObserver
	void SelectFromMapCompleted( TInt aError,
                                 CPosLandmark* aLandmark );
	
private: // Implementaton
	CESMRLocationPluginHandler( MObjectProvider& aParent );
	void ConstructL();
	CESMRLocationPlugin& LocationPluginL();
	void UpdateEntryFromLandmarkL( MESMRCalEntry& aEntry,
                                   const CPosLandmark& aLandmark );
	HBufC* CreateAddressFromLandmarkL( const CPosLandmark& aLandmark );	
	void SearchFromLandmarksL( MObjectProvider* aParent,
                               MESMRCalEntry& aEntry );
	void ShowSelectPreviousLocationQueryL( MESMRCalEntry& aEntry );
	void UpdateEntryLocationL( MESMRCalEntry& aEntry, const TDesC& aLocation );
	void UpdateEntryDescriptionL( MESMRCalEntry& aEntry,
                                  const TDesC& aLocationUrl );
	CESMRUrlParserPlugin& UrlParserL();
	CESMRLocationHistoryManager& LocationHistoryManagerL();
	
private: // Data
	
	/**
     * Query ignore flag 
     */
    TBool iIgnoreQuery;
    
	/**
     * Entry update flag
     * 
     */
    TBool iEntryUpdated;
    
    /**
     * Location replace flag
     */
    TBool iLocationReplaced;
    
	/**
	 *  Location plugin.
	 *  Own.
	 */
	CESMRLocationPlugin* iLocationPlugin;
	
	/**
	 * Location Plugin synchronizer.
	 * Own.
	 */
	CActiveSchedulerWait* iWait;
	
	/**
	 * Location url parser.
	 * Own.
	 */
	CESMRUrlParserPlugin* iUrlParser;
	
    /**
     * Pointer to result of landmark fetch.
     * Own.
     */
	CPosLandmark* iLandmark;
	
	/**
	 * Pointer to CLmkLandmarkSelectorDlg parent object.
	 * Ref.
	 */
	MObjectProvider* iParent;
	
	/**
     * Location history manager.
     * Own.
	 */
	CESMRLocationHistoryManager* iLocationHistoryManager;
	
    };

#endif // CESMRLOCATIONPLUGINHANDLER_H

