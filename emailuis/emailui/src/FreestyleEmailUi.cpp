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
* Description:  FreestyleEmailUi application entry point implementation
*
*/


#include "emailtrace.h"
#include "FreestyleEmailUiApplication.h"
#include "FreestyleEmailUiDocument.h"
#include "FreestyleEmailUiConstants.h"
#include <eikstart.h>
#include <u32std.h>
#include <MemoryManager.h>

//#define __USE_FAST_ALLOCATOR

CApaDocument* CFreestyleEmailUiApplication::CreateDocumentL()
    {
    FUNC_LOG;
    CApaDocument* document = CFreestyleEmailUiDocument::NewL(*this);
    return document;
    }

TUid CFreestyleEmailUiApplication::AppDllUid() const
    {
    FUNC_LOG;
    return KFSEmailUiUid;
    }

LOCAL_C CApaApplication* NewApplication()
    {
    FUNC_LOG;
    return new CFreestyleEmailUiApplication;
    }

#ifdef __USE_FAST_ALLOCATOR
EXPORT_C TInt UserHeap::SetupThreadHeap( TBool aSubThread, SStdEpocThreadCreateInfo& aInfo )
    {
    TInt error( KErrNone );
    if (!aInfo.iAllocator && aInfo.iHeapInitialSize>0)
        {
        // new heap required
        RHeap* pH = NULL;
        error = CreateThreadHeap(aInfo, pH);
        if (!error && !aSubThread)
            {
            // main thread - new allocator created and set as default heap
            MemoryManager::CreateFastAllocator();
            }
        }
    else if (aInfo.iAllocator)
        {
        // sharing a heap
        RAllocator* pA = aInfo.iAllocator;
        error = pA->Open();
		if (!error)
			{
			User::SwitchAllocator(pA);
			}
        }
    return error;
    }
#endif // __USE_FAST_ALLOCATOR

GLDEF_C TInt E32Main()
    {
#ifdef __USE_FAST_ALLOCATOR
    // initialize MemmoryManager
    MemoryManager::InitFastAllocator();
#endif // __USE_FAST_ALLOCATOR
    return EikStart::RunApplication(NewApplication);
    }


