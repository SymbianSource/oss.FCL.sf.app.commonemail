/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Input mode observer class definition
*
*/


#ifndef FREESTYLEEMAILUIINPUTMODEOBSERVER_H
#define FREESTYLEEMAILUIINPUTMODEOBSERVER_H

// INCLUDES
#include <e32base.h>				// CTimer
#include <cenrepnotifyhandler.h>	// MCenRepNotifyHandlerCallback

// FORWARD DECLARATION
class CFSEmailUiClsListsHandler;
class CRepository;

// CLASS DECLARATION

/**
*  Freestyle email application input mode observer class
*
*  
*/
class CFSEmailUiInputModeObserver : public CBase, public MCenRepNotifyHandlerCallback
	{
	public:
	
	    /**
		* Destructor
		*/
		virtual ~CFSEmailUiInputModeObserver();

		/**
		* CFSEmailUiInputModeObserver two phase construction
		* @param aCr reference to aknfepcenrep session
		* @param aListHandler reference to cls list handler
	    * @return A pointer to the created object
		*/
		static CFSEmailUiInputModeObserver* NewL( CRepository& aCr,
													CFSEmailUiClsListsHandler& aListHandler );

	private:
	
		void HandleNotifyInt( TUint32 aId, TInt aNewValue );
		void HandleNotifyReal( TUint32 aId, TReal aNewValue );
		void HandleNotifyString( TUint32 aId, const TDesC16& aNewValue );

		void HandleNotifyBinary( TUint32 aId, const TDesC8& aNewValue );
		void HandleNotifyGeneric( TUint32 aId );
		void HandleNotifyError( TUint32 aId, TInt aError, CCenRepNotifyHandler* aHandler );

		/**
		* Constructor
		* @param aCr reference to aknfepcenrep session
		* @param aListHandler reference to cls list handler		
		*/
		CFSEmailUiInputModeObserver( CRepository& aCr, CFSEmailUiClsListsHandler& aListHandler );
		
		/**
		* Symbian 2nd phase constructor
	    * @since 3.0
		*/
		void ConstructL();
		

	private:
	
		// Handle to parrot server client side
        CRepository&				iAkvkFepCenRep;
     	CCenRepNotifyHandler*		iCenRepNotifyWrapper;
        
        CFSEmailUiClsListsHandler&	iClsListHandler;
	};


#endif // FREESTYLEEMAILUIINPUTMODEOBSERVER_H

// End of file

