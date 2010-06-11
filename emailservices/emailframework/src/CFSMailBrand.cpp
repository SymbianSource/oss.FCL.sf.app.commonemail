/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSMailBrand.
*
*/

#include "emailtrace.h"

//<qmail>
#include <nmcommonheaders.h>
//</qmail>

#include <barsread.h>
//<qmail> Commented out in Qmail
//#include <AknIconUtils.h>
//</qmail>
#include <gulicon.h>
#include <centralrepository.h>
//<cmail>
//<qmail> Commented out in Qmail
//#include "freestyleemailcenrepkeys.h"
//</qmail>
//</cmail>

#include "CFSMailBrand.h"

const TInt KElementArrayGranularity = 5;
//<qmail> Commented out in Qmail
//const TInt KMaxStringLenFromCenrep = 256;
//</qmail>
const TInt KMaxDesLen = 256;
_LIT(KSpace, " ");
// -----------------------------------------------------------------------------
// CFSMailBrand::NewL
// -----------------------------------------------------------------------------
CFSMailBrand* CFSMailBrand::NewL( TResourceReader& aReader, TBool aIsWhiteLabel )
	{
    NM_FUNCTION;
    
	CFSMailBrand* brManager =  CFSMailBrand::NewLC(aReader, aIsWhiteLabel);
  	CleanupStack:: Pop(brManager);
  	return brManager;
	}

// -----------------------------------------------------------------------------
// CFSMailBrand::NewLC
// -----------------------------------------------------------------------------
CFSMailBrand* CFSMailBrand::NewLC( TResourceReader& aReader, TBool aIsWhiteLabel )
	{
    NM_FUNCTION;
    
    CFSMailBrand* self = new ( ELeave ) CFSMailBrand();
    CleanupStack::PushL( self );
    if ( aIsWhiteLabel )
    	{
    	self->ConstructFromCenrepL( );
    	}
    else
    	{
    	self->ConstructFromResourceL( aReader );
    	}
    return self;
	}


// -----------------------------------------------------------------------------
// CFSMailBrand::ConstructFromCenrepL
// -----------------------------------------------------------------------------
void CFSMailBrand::ConstructFromCenrepL( )
    {
    NM_FUNCTION;
    
	/*
    TBuf<KMaxStringLenFromCenrep> tBuf; // Temporary buffer
    HBufC*    mailboxName;

    iGraphicElements = new ( ELeave )
        CArrayFixSeg< TBrandedGraphic >( KElementArrayGranularity );

    iTextElements = new ( ELeave )
        CArrayFixSeg< TBrandedText >( KElementArrayGranularity );

    iTexts = new ( ELeave ) CDesCArraySeg( KElementArrayGranularity );

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KFreestyleEmailCenRep ));
    if ( ret == KErrNone )
        {
        CleanupStack::PushL( repository );
        TInt err_count = 0;
        // Read all WLB related parameters from Central Repository
        TInt err = repository->Get( KFreestyleWLBBrandIdMatchString, tBuf );
        HBufC* buf = tBuf.AllocL();
        iBrandMatchStrings.AppendL(buf);
        if ( err != KErrNone )
            {
            err_count++;
            }
        err = repository->Get( KFreestyleWLBMailboxName, tBuf );
        mailboxName = tBuf.AllocLC();
        if ( err != KErrNone )
            {
            err_count++;
            }
        err = repository->Get( KFreestyleWLBMIFFilePathWithTargetFilename, tBuf );
        iIconFilePath = tBuf.AllocL();
        if ( err != KErrNone )
            {
            err_count++;
            }
        if ( err_count == 0 )
            {
            // Create WLB graphic elements
            TBrandedGraphic newMailboxIconElement;
            newMailboxIconElement.iElementId = ( TFSBrandElement ) EFSMailboxIcon;
            err = repository->Get( KFreestyleWLBMailboxIconID, newMailboxIconElement.iIconId );
            if ( err != KErrNone )
                newMailboxIconElement.iIconId = 0x4000; // Use default if Cenrep read fails
            newMailboxIconElement.iMaskId = newMailboxIconElement.iIconId + 1;      
            iGraphicElements->AppendL( newMailboxIconElement );

            TBrandedText newMailboxNameElement;
            newMailboxNameElement.iElementId = ( TFSBrandElement) EFSMailboxName;
            iTexts->AppendL( *mailboxName );
            newMailboxNameElement.iText.Set( iTexts->MdcaPoint( iTexts->Count() - 1 ) );
            iTextElements->AppendL( newMailboxNameElement );
            }
        CleanupStack::PopAndDestroy( mailboxName );
        CleanupStack::PopAndDestroy( repository );
        }
		*/
   }

// -----------------------------------------------------------------------------
// CFSMailBrand::ConstructFromResourceL
// -----------------------------------------------------------------------------
void CFSMailBrand::ConstructFromResourceL( TResourceReader& aReader )
    {
    NM_FUNCTION;

    // read icon filepath
    iIconFilePath = aReader.ReadHBufCL();

    // read match strings
    TInt count = aReader.ReadInt16();
    for ( TInt i = 0; i < count; i++ )
        {
        iBrandMatchStrings.AppendL(aReader.ReadHBufCL());        
        }   

    // read graphics
    iGraphicElements = new ( ELeave )
        CArrayFixSeg< TBrandedGraphic >( KElementArrayGranularity );
    count = aReader.ReadInt16();
    for ( TInt i = 0; i < count; i++ )
        {
        TBrandedGraphic newElement;
        newElement.iElementId = ( TFSBrandElement ) aReader.ReadInt32();
        newElement.iIconId = aReader.ReadInt32();
        newElement.iMaskId = aReader.ReadInt32();
        iGraphicElements->AppendL( newElement );
        }

    // read texts
    iTexts = new ( ELeave ) CDesCArraySeg( KElementArrayGranularity );
    iTextElements = new ( ELeave )
        CArrayFixSeg< TBrandedText >( KElementArrayGranularity );
    count = aReader.ReadInt16();
    for ( TInt i = 0; i < count; i++ )
        {
        TBrandedText newElement;
        newElement.iElementId = ( TFSBrandElement) aReader.ReadInt32();
        HBufC* text = aReader.ReadTPtrC().AllocLC();
        iTexts->AppendL( *text );
        CleanupStack::PopAndDestroy( text );
        newElement.iText.Set( iTexts->MdcaPoint( iTexts->Count() - 1 ) );
        iTextElements->AppendL( newElement );
        }

    // read colors
    iColorElements = new ( ELeave )
        CArrayFixSeg< TBrandedColor >( KElementArrayGranularity );
    count = aReader.ReadInt16();
    for ( TInt i = 0; i < count; i++ )
        {
        TBrandedColor newElement;
        newElement.iElementId = ( TFSBrandElement ) aReader.ReadInt32();
        newElement.iColor.SetRed( aReader.ReadInt16() );
        newElement.iColor.SetGreen( aReader.ReadInt16() );
        newElement.iColor.SetBlue( aReader.ReadInt16() );
        newElement.iColor.SetAlpha( aReader.ReadInt16() );
        iColorElements->AppendL( newElement );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBrand::~CFSMailBrand()
// -----------------------------------------------------------------------------
CFSMailBrand::~CFSMailBrand()
	{
    NM_FUNCTION;
    
	iBrandMatchStrings.ResetAndDestroy();
	delete iIconFilePath;
	delete iGraphicElements;
	delete iTextElements;
	delete iColorElements;
	delete iTexts;
	delete iEmpty;
	}

// -----------------------------------------------------------------------------
// CFSMailBrand::CFSMailBrand()
// -----------------------------------------------------------------------------
CFSMailBrand::CFSMailBrand()
	{
    NM_FUNCTION;
    
		// prepare null empty descriptor
	iEmpty = HBufC::New(1);
	iEmpty->Des().Copy(KNullDesC());

	}

// -----------------------------------------------------------------------------
// CFSMailBrand::IsMatching
// -----------------------------------------------------------------------------
TBool CFSMailBrand::IsMatching( const TDesC& aBrandId )
    {
    NM_FUNCTION;

    TInt count = iBrandMatchStrings.Count();
    for(TInt i=0;i<count;i++)
        {
        if ( aBrandId.MatchC( *iBrandMatchStrings[i] ) == KErrNone )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CFSMailBrand::GetText
// -----------------------------------------------------------------------------
TDesC& CFSMailBrand::GetText( TFSBrandElement aElementId )
    {
    NM_FUNCTION;
    
    TInt textCount( iTextElements->Count() );

    for ( TInt i( 0 ); i < textCount; i++ )
        {
        if ( iTextElements->At( i ).iElementId == aElementId )
            {
            return iTextElements->At( i ).iText;
            }
        }
        
    return *iEmpty;
    }

// -----------------------------------------------------------------------------
// CFSMailBrand::GetColor
// -----------------------------------------------------------------------------
TInt CFSMailBrand::GetColor( TFSBrandElement aElementId, TRgb& aColor )
    {
    NM_FUNCTION;
    
    TInt colorCount( iColorElements->Count() );

    for ( TInt i( 0 ); i < colorCount; i++ )
        {
        if ( iColorElements->At( i ).iElementId == aElementId )
            {
            aColor = iColorElements->At( i ).iColor;
            return KErrNone;
            }
        }
    
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CFSMailBrand::GetGraphicL
// -----------------------------------------------------------------------------
CGulIcon* CFSMailBrand::GetGraphicL( TFSBrandElement aElementId )
    {
    NM_FUNCTION;
    
    TInt graphicsCount( iGraphicElements->Count() );

    for ( TInt i( 0 ); i < graphicsCount; i++ )
        {
        TBrandedGraphic element = iGraphicElements->At( i );

        if ( element.iElementId == aElementId )
            {
            CFbsBitmap* icon( NULL );
            CFbsBitmap* mask( NULL );            

            if ( iIconFilePath == NULL )
                {
                // If graphic element is found but no path is defined it is
                // deemed as an error situation.
                User::Leave( KErrNotFound );
                }
                
            TFileName dllFileName;
            Dll::FileName( dllFileName );
		 	TParse parse;
		    User::LeaveIfError( parse.Set( *iIconFilePath, &dllFileName, NULL) );
		    TFileName iconFileName( parse.FullName() );
             
//<qmail>
            // Get icon and mask with above info
            /*AknIconUtils::CreateIconLC( icon,
                                        mask,
                                        iconFileName,
                                        element.iIconId,
                                        element.iMaskId );*/
//</qmail>
            CGulIcon* gulIcon = CGulIcon::NewL( icon, mask );

            CleanupStack::Pop( 2 ); // icon, mask
            
            return gulIcon;
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CFSMailBrand::GetGraphicIdsL
// -----------------------------------------------------------------------------
TInt CFSMailBrand::GetGraphicIdsL( TFSBrandElement aElementId,
                                   TDes& aIconIds)
    {
    NM_FUNCTION;
    
    aIconIds.Zero();
    TInt graphicsCount( iGraphicElements->Count() );

    for ( TInt i( 0 ); i < graphicsCount; i++ )
        {
        TBrandedGraphic element = iGraphicElements->At( i );

        if ( element.iElementId == aElementId )
            {
            if ( iIconFilePath == NULL )
                {
                // If graphic element is found but no path is defined it is
                // deemed as an error situation.
                User::Leave( KErrNotFound );
                }
                
            TFileName dllFileName;
            Dll::FileName( dllFileName );
		 	TParse parse;
		    User::LeaveIfError( parse.Set( *iIconFilePath, &dllFileName, NULL) );
		    TFileName iconFileName( parse.FullName() );
            
		    TBuf<KMaxDesLen> id;
		    aIconIds.Copy(iconFileName);
		    aIconIds.Append(KSpace);
		    id.Num(element.iIconId);
		    aIconIds.Append(id);
		    aIconIds.Append(KSpace);
		    id.Num(element.iMaskId);
            aIconIds.Append(id);
            
            return KErrNone;
            }
        }

    return KErrNotFound;
    }

