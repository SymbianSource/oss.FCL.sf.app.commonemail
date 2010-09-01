/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing the RECAL customisation interface
*
*/

#ifndef CMRBCPLUGIN_H
#define CMRBCPLUGIN_H

// INCLUDES
#include <calencustomisation.h>

// FORWARD DECLARATIONS
class MCalenServices;
class CMRBCPluginEntryLoader;
class CMRBCPluginResourceReader;
class MCalenPreview;
class CMRBCEventPlugin;

// CLASS DECLARATIONS
/**
 * This class is used for creating implement of view through ECom framework.
 */
NONSHARABLE_CLASS( CMRBCPlugin ) : public CCalenCustomisation
    {
public: // C'tor and d'tor
    /**
    * Static constructor.
    * @param aServices 
    * @return A new instance of CMRBCPlugin
    */        
    static CMRBCPlugin* NewL( MCalenServices* aServices );
    
    /**
    * Destructor
    */
    ~CMRBCPlugin();
    
public: // Interface

protected: // From base class CCalenCustomisation
    
    void GetCustomViewsL(  RPointerArray<CCalenView>& aCustomViewArray );
            
    void GetCustomSettingsL( RPointerArray<CAknSettingItem>& aCustomSettingArray );
    
    CCoeControl* InfobarL( const TRect& aRect );
    
    const TDesC& InfobarL();
    
    MCalenPreview* CustomPreviewPaneL( TRect& aRect ); // From wk 39 onwards
    
    CCoeControl* PreviewPaneL( TRect& aRect );
    
    MCalenCommandHandler* CommandHandlerL(TInt aCommand );
    
    void RemoveViewsFromCycle( RArray<TInt>& aViews );

    TBool CustomiseMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    
    TBool CanBeEnabledDisabled();
    
    TAny* CalenCustomisationExtensionL( TUid aExtensionUid );
    
private: // Implementation
    /**
     * C++ constructor
     */
    CMRBCPlugin( MCalenServices* aServices );
    
    /**
     * constructor
     */
    void ConstructL();
    
    MCalenCommandHandler* ResolveCommandHandlerL( TInt aCommand );
    
    CMRBCEventPlugin* PluginByUidL( TUid aUid );
    
private: // Data
    /// Ref: Services interface for Calendar services
    MCalenServices* iServices; 
    /// Own: Plugin resource reader
    RPointerArray<CMRBCPluginResourceReader> iPluginResources;
    /// Own: Entry loader
    CMRBCPluginEntryLoader* iEntryLoader;
    };
    
#endif // CMRBCPLUGIN_H

// End of file
