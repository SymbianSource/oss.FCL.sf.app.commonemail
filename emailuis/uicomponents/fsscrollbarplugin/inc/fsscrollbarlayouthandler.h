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
* Description: This file defines class CFsScrollbarLayoutHandler.
*
*/



#ifndef C_FSSCROLLBARLAYOUTHANDLER_H
#define C_FSSCROLLBARLAYOUTHANDLER_H

//<cmail> SF
#include <alf/alflayouthandlers.h>
//</cmail>

/**
 *  ?one_line_short_description
 *  ?more_complete_description
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFsScrollbarLayoutHandler : CAlfLayoutHandler
    {

public:

    ~CFsScrollbarLayoutHandler() {}

// from base class CAlfLayoutHandler

    static MAlfExtension* NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout);

    void ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout);

    void Release() { delete this; }

    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    CFsScrollbarLayoutHandler( MAlfInterfaceProvider& aResolver ) : CAlfLayoutHandler( aResolver ) {}

private: // data

    };


#endif // C_FSSCROLLBARLAYOUTHANDLER_H
