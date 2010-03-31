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
* Description: MBUtils ECOM definition
*
*/

#include "cmrbcpluginresourcereader.h"
#include "cmrbceventplugin.h"

#include <coemain.h>
#include <barsread.h>

#include "emailtrace.h"

namespace { //codescanner::namespace

// BC Plugin resource file offset
const TInt KMRBCPluginResourceFileOffset( 2 );

} // namespace

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::CMRBCPluginResourceReader
// Static constructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPluginResourceReader::CMRBCPluginResourceReader(
        MCalenServices& aCalenServices )
:   iCalenServices( aCalenServices )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::~CMRBCPluginResourceReader
// -----------------------------------------------------------------------------
//
CMRBCPluginResourceReader::~CMRBCPluginResourceReader()
    {
    FUNC_LOG;
    
    iPluginCommands.Close();
    iBCPluginResourceFile.Close();
    delete iPlugin;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::NewL
// -----------------------------------------------------------------------------
//
CMRBCPluginResourceReader* CMRBCPluginResourceReader::NewL(
        MCalenServices& aCalenServices )
    {
    FUNC_LOG;
    
    CMRBCPluginResourceReader* self = 
            new (ELeave) CMRBCPluginResourceReader( aCalenServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::ConstructL
// -----------------------------------------------------------------------------
//
void CMRBCPluginResourceReader::ConstructL()
    {  
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::ReadFromResourceFileL
// -----------------------------------------------------------------------------
//
void CMRBCPluginResourceReader::ReadFromResourceFileL(
        TUid aUid,
        const TDesC& aResourceFile )
    {
    FUNC_LOG;
    
    CCoeEnv* coeEnv( CCoeEnv::Static() );
    
    iBCPluginResourceFile.Close();
    iBCPluginResourceFile.OpenL( coeEnv->FsSession(), aResourceFile );
    iBCPluginResourceFile.ConfirmSignatureL();
    
    iOffset = iBCPluginResourceFile.Offset() + KMRBCPluginResourceFileOffset;
    
    iPluginCommands.Reset();
    iSupportedEventTypes.ClearAll();
        
    HBufC8* resourceIdBuffer =
        iBCPluginResourceFile.AllocReadLC( iOffset );
    
    // Construct resource reader
    TResourceReader reader;
    reader.SetBuffer(resourceIdBuffer);

    iPluginImplementationUid = aUid;
    iVersion.iMajor = reader.ReadInt32();
    iSupportedEventTypes = reader.ReadUint32();
    
    TInt commandCount( reader.ReadInt16() );
    iPluginCommands.ReserveL( commandCount );
    
    for (TInt i(0); i < commandCount; ++i )
        {
        TMRBCPluginCommand pluginCommand;
        
        pluginCommand.iCommand = reader.ReadInt32();
        pluginCommand.iCheckEntryType = reader.ReadInt32();
        iPluginCommands.Append( pluginCommand  );
        }

    iBCPluginResourceFile.Close();
    CleanupStack::PopAndDestroy( resourceIdBuffer );
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::Version
// -----------------------------------------------------------------------------
//
TVersion CMRBCPluginResourceReader::Version() const
    {
    FUNC_LOG;
    
    return iVersion;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::ReadFromResourceFileL
// -----------------------------------------------------------------------------
//
TBool CMRBCPluginResourceReader::SupportsType( 
        TBCPluginEventType aEventType ) const
    {
    FUNC_LOG;
    
    return iSupportedEventTypes.IsSet( aEventType );
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::ReadFromResourceFileL
// -----------------------------------------------------------------------------
//
TBool CMRBCPluginResourceReader::SupportsCommand( 
        TInt aCommand ) const
    {
    FUNC_LOG;
    
    TInt pos( KErrNotFound );
    TInt commandCount( iPluginCommands.Count() );
    for ( TInt i(0); i < commandCount && KErrNotFound == pos; ++i )
        {
        if ( iPluginCommands[i].Command() == aCommand )
            {
            pos = i;
            }
        }

    return KErrNotFound != pos;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::CommandL
// -----------------------------------------------------------------------------
//
TInt CMRBCPluginResourceReader::Command( 
        TInt aCommand, 
        TMRBCPluginCommand& aPluginCommand ) const
    {
    FUNC_LOG;
    
    TInt retValue( KErrNotFound );
    TInt commandCount( iPluginCommands.Count() );
    for ( TInt i(0); i < commandCount && KErrNotFound == retValue; ++i )
        {
        if ( iPluginCommands[i].Command() == aCommand )
            {
            retValue = KErrNone;
            aPluginCommand = iPluginCommands[i];
            }
        }    
    
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::PluginImplUid
// -----------------------------------------------------------------------------
//
TUid CMRBCPluginResourceReader::PluginImplUid() const
    {
    FUNC_LOG;
    
    return iPluginImplementationUid;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginResourceReader::PluginImplUid
// -----------------------------------------------------------------------------
//
CMRBCEventPlugin& CMRBCPluginResourceReader::PluginL()
    {
    FUNC_LOG;
    
    if ( !iPlugin )
        {
        iPlugin = CMRBCEventPlugin::NewL(
                        iPluginImplementationUid,
                        iCalenServices );        
        }
    
    return *iPlugin;
    }

// EOF
