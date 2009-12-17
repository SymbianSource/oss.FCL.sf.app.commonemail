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
* Description: This file defines class TIpsSetUtilsFlags.
*
*/


#ifndef TIPSSETUTILSFLAGS_H
#define TIPSSETUTILSFLAGS_H


/**
 * Helper class for flags.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class TIpsSetUtilsFlags
    {
    public:
        /**
         * constructor
         */
        TIpsSetUtilsFlags( const TUint64 aFlags = 0 );

        /**
         * Sets flag on
         * @param aFlag: flag to be set
         */
        inline void SetFlag( const TUint64 aFlag );

        /**
         * Clear flag on
         * @param aFlag: flag to be cleared
         */
        inline void ClearFlag( const TUint64 aFlag );

        /**
         * Query for the flag
         * @param aFlag: flag to be checked
         */
        inline TBool Flag( const TUint64 aFlag ) const;

        /**
         * Sets flag on. Converts the flag to 64-bit and move 32 steps.
         * @param aFlag: flag to be set
         */
        inline void SetFlag32( const TUint32 aFlag );

        /**
         * Clear flag on. Converts the flag to 64-bit and move 32 steps.
         * @param aFlag: flag to be cleared
         */
        inline void ClearFlag32( const TUint32 aFlag );

        /**
         * Query for the flag. Converts the flag to 64-bit and move 32 steps.
         * @param aFlag: flag to be checked
         */
        inline TBool Flag32( const TUint32 aFlag ) const;

        /**
         * Modifies the state of the flag
         */
        inline void ChangeFlag(
            const TUint64 aFlag,
            const TBool aNewState );

        /**
         * Returns value based on flag status.
         *
         * @param aFlag 32-bit version of flag
         * @param aTrue Value, when flag is set
         * @param aFalse Value, when flag is not set
         */
        TInt ValueForFlag(
            const TUint32 aFlag,
            const TInt aTrue,
            const TInt aFalse ) const;

        /**
         * Copies the flags
         */
        TIpsSetUtilsFlags& operator=( const TIpsSetUtilsFlags& aFlags );

        /**
         * Copies the flags
         */
        TIpsSetUtilsFlags& operator=( const TUint64 aFlags );

    private:

        /**
         * Storage for flags
         */
        TUint64 iFlags;
    };

#include "ipssetutilsflags.inl"

#endif /* TIPSSETUTILSFLAGS_H */

// End of File

