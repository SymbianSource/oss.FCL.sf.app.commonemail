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
* Description:  Common defines for Contact Action Menu UI
*
*/


#ifndef C_FSCCONTACTACTIONMENUUIDEFINES_H
#define C_FSCCONTACTACTIONMENUUIDEFINES_H

#include <e32base.h>

// DEFINES
      
#define KCORNERSIZE1 TSize( 2, 2 );    
#define KCORNERSIZE2 TSize( 3, 3 ); 
#define KCORNERSIZE3 TSize( 4, 4 ); 
#define KCORNERSIZE4 TSize( 8, 9 ); 

// FORWARD DECLARATIONS

// TYPEDEFS
//<cmail>
// CONSTS
const TInt KGranularity = 15;
const TInt KMaxItemsVisible = 7;
//needed to show listbox correctly
const TInt KEmptyMenuHeight = 100; //default value
const TInt KZero = 0;
//used to add some additional pixels after the text
//const TInt KExtraTextWidth = 7; 
//const TInt KBaselinePosOffset = 4; //Baseline offset

//Column ids
enum TECAListColumnId
    {
    ELeftMarginColumn,
    EIconColumn,
    EMidleMarginColumn,
    ETextColumn
    };

const TInt KMenuSlideSpeed = 800;
//</cmail>

#endif // C_FSCCONTACTACTIONMENUUIDEFINES_H

