/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for reporting errors during the import of Agenda data from an
*  external source
*
*
*/


#ifndef MESMRAGNIMPORTOBSERVER_H
#define MESMRAGNIMPORTOBSERVER_H


/**
Interface for reporting errors during the import of Agenda data from an
external source.
@publishedAll
*/
class MESMRAgnImportObserver
	{
public:	// Enumerations.

	enum TImpResponse
		{
		/** Import as much as possible.*/
		EImpResponseContinue,
		/** Skip the broken component and continue with the next.*/
		EImpResponseSkip,
		/** Stop parsing and return.*/
		EImpResponseStop,
		/** Stop parsing and leave with KErrAbort.*/
		EImpResponseLeave
		};

	enum TImpError
		{
		/** No error to report.*/
		EImpErrorNoError,
		/** The data being imported is corrupt and cannot be interpreted.*/
		EImpErrorBadSyntax,
		/** The data being imported can be parsed, but has invalid values.*/
		EImpErrorInvalidData,
		/** The data being imported is missing values which must be present.*/
		EImpErrorMissingData,
		/** The data being imported is correct, but the Agenda model cannot store it.*/
		EImpErrorNotSupported
		};

public:		// Interface.
	/**
	This function will be called by AgendaExternalInterface plugins when they
	encounter an error for which they feel the controlling application should
	determine the appropriate course of action.
	@param aType The type of the error
	@param aUid The UID of the entry in which the error occurred (if known)
	@param aContext A textual indication of the actual error
	@return a TImpResponse indication the action to take
	@see CAgnExternalInterface::ImportL
	@publishedAll
	*/
	virtual TImpResponse AgnImportErrorL(TImpError aType, const TDesC8& aUid, const TDesC& aContext) = 0;
	};

#endif // MESMRAGNIMPORTOBSERVER_H

// End of file.