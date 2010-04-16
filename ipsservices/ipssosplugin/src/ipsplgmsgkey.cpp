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
* Description:  Key class for message sorting
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

const TInt KLessThan = -1;
const TInt KMoreThan = 1;
const TInt KEqual = 0;

_LIT(KDefaultSubjectPrefixSeparator,": ");

// ---------------------------------------------------------------------------
// Basic sonstructor
// ---------------------------------------------------------------------------

TIpsPlgMsgKey::TIpsPlgMsgKey( 
    CMsvEntry& aFolderEntry,
    const RArray<TFSMailSortCriteria>& aSortCriteria )
    : iFolderEntry( aFolderEntry ), 
      iSortingCriteria( aSortCriteria ),
      iSubjectPrefixSeparator( KDefaultSubjectPrefixSeparator )
    {
    FUNC_LOG;
    // none
    }

// ---------------------------------------------------------------------------
// Compares the messages under the folder entry according to the sorting
// criteria.
// ---------------------------------------------------------------------------

TInt TIpsPlgMsgKey::Compare( TInt aLeft, TInt aRight ) const
    {
    FUNC_LOG;
    TInt result( KEqual );
    TInt childCount( iFolderEntry.Count() );
    if ( aLeft >= childCount || aRight >= childCount  )
        {
        return result;
        }
    const TMsvEmailEntry leftEntry( iFolderEntry[aLeft] );
    const TMsvEmailEntry rightEntry( iFolderEntry[aRight] );
    
    // Loop over the sorting criteria until the comparison result is known
    TInt i(0);
    while ( ( result == KEqual ) && ( i < iSortingCriteria.Count() ) )
        {
        switch ( iSortingCriteria[i].iField )
            {
            case EFSMailDontCare:
                {
                // If the client does not care, we can return immediately
                return result;
                }
            case EFSMailSortByDate:
                {
                if ( leftEntry.iDate < rightEntry.iDate ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.iDate > rightEntry.iDate ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            case EFSMailSortBySender:
                {
                result = 
                    leftEntry.iDetails.CompareC( rightEntry.iDetails );
                break;
                }
            case EFSMailSortByRecipient:
                {                
                result = 
                    leftEntry.iDetails.CompareC( rightEntry.iDetails );
                break;
                }
            case EFSMailSortBySubject:
                {
                result = CompareSubjects( 
                    leftEntry.iDescription, rightEntry.iDescription );
                break;
                }
            case EFSMailSortByPriority:
                // The values of TMsvPriority are defined so that the highest
                // priority has the smallest value
                if ( leftEntry.Priority() > rightEntry.Priority() ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.Priority() < rightEntry.Priority() ) 
                    {
                    result = KMoreThan;
                    }
                break;
            case EFSMailSortByFlagStatus:
                {
                // The 'flagged' state is supported only in IMAP4
                if ( leftEntry.iMtm == KUidMsgTypeIMAP4 )
                    {
                    if ( !leftEntry.FlaggedIMAP4Flag() && 
                         rightEntry.FlaggedIMAP4Flag() ) 
                        {
                        result = KLessThan;
                        }
                    else if ( leftEntry.FlaggedIMAP4Flag() && 
                              !rightEntry.FlaggedIMAP4Flag() ) 
                        {
                        result = KMoreThan;
                        }
                    }
                break;
                }
            case EFSMailSortByUnread:
                {
                // In this context, an unread message is 'greater' than
                // a read one
                if ( !leftEntry.Unread() && rightEntry.Unread() ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.Unread() && !rightEntry.Unread() ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            case EFSMailSortBySize:
                {
                if ( leftEntry.iSize < rightEntry.iSize ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.iSize > rightEntry.iSize ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            case EFSMailSortByAttachment:
                {
                // In this context, a message with an attachment is 'greater'
                // than one without an attachment
                if ( !leftEntry.Attachment() && rightEntry.Attachment() ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.Attachment() && !rightEntry.Attachment() ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            default:
                break;
            }
            
        // Switch the order if the messages are sorted in the descending order
        if ( iSortingCriteria[i].iOrder == EFSMailDescending ) 
            {
            result = -result;
            }
        
        i++;
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// Returns a message based on the index.
// Note, that KIndexPtr index value is not supported as the class is not 
// intended to be used with User::BinarySearch().
// ---------------------------------------------------------------------------

TAny* TIpsPlgMsgKey::At( TInt anIndex ) const
    {
    FUNC_LOG;
    return (TAny*) &iFolderEntry[ anIndex ];
    }

// ---------------------------------------------------------------------------
// Strips the subject prefixes before comparing the strings
// ---------------------------------------------------------------------------

TInt TIpsPlgMsgKey::CompareSubjects( 
    const TDesC& aLeft, 
    const TDesC& aRight ) const
    {
    FUNC_LOG;
    TInt  result( KEqual );
    TPtrC leftPtr( aLeft );
    TPtrC rightPtr( aRight );
    TInt  leftOffset( FindSubjectStart( aLeft ) );
    TInt  rightOffset( FindSubjectStart( aRight ) );
    
    leftPtr.Set( 
        leftPtr.Ptr() + leftOffset, leftPtr.Length() - leftOffset );
    rightPtr.Set( 
        rightPtr.Ptr() + rightOffset, rightPtr.Length() - rightOffset );
        
    result = leftPtr.CompareC( rightPtr );
        
    return result;
    }
    
// ---------------------------------------------------------------------------
// Finds the starting point of the actual subject text after the subject
// prefixes (Re:, Fwd: etc).
// The algorithm search the last separator string (': ') in most western
// languages and returns the offset from the beginnig of the string to
// the first character following the last separator.
// ---------------------------------------------------------------------------

TInt TIpsPlgMsgKey::FindSubjectStart( const TDesC& aSubject ) const
    {
    FUNC_LOG;
	TPtrC ptr(aSubject);
	TInt offset(0);
	TInt current(0);
	TInt skipLength = iSubjectPrefixSeparator.Length();
	
	// Loop while separators are found
	do
		{
		current = ptr.FindF( iSubjectPrefixSeparator );

		if ( current != KErrNotFound )
			{
			offset += current + skipLength;
			ptr.Set( aSubject.Ptr() + offset, aSubject.Length() - offset );
			}
		} while ( current != KErrNotFound );

	return offset;
    }





