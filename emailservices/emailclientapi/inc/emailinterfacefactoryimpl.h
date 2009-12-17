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
* Description: Interface factory implementation class definition
*
*/

#ifndef EMAILINTERFACEFACTORY_H
#define EMAILINTERFACEFACTORY_H

#include <emailinterfacefactory.h>

using namespace EmailInterface;

/**
* Implements CEmailInterfaceFactory ECom interface
*
*/
NONSHARABLE_CLASS( CEmailInterfaceFactoryImpl ) : public CEmailInterfaceFactory
{
public: 

    /**
    * Creates interface factory
    * @return interface factory
    */
    static CEmailInterfaceFactoryImpl* NewL();
    
    // destructor
    ~CEmailInterfaceFactoryImpl();
    
    /** @see CEmailInterfaceFactory::InterfaceL */
    virtual MEmailInterface* InterfaceL(  const TInt aInterfaceId );
    
private:    
    // c++ constructor
    CEmailInterfaceFactoryImpl();
};

#endif // EMAILINTERFACEFACTORY_H

// End of file.
