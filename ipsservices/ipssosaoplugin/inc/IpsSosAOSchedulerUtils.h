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
* Description: 
*     Functions to calculate timer intervals
*
*/


#ifndef IPSSOSAOSCHEDULERUTILS_H_
#define IPSSOSAOSCHEDULERUTILS_H_

#include <e32base.h>
//<Qmail>

//</Qmail>
enum TIpsSosAOConnectionType
    {
    EAOCDisconnectAfter = 0,
    EAOCConnectAfter
    };
        
enum TIpsSosAOValidConnectionDay
    {
    EAOVCDNoConnection = 0,
    EAOVCDConnectionAnyDay,
    EAOVCDConnectionCurrentDay,
    EAOVCDConnectionPreviousDayOnly
    };

/**
*  Class CIpsSosAOSchedulerUtils 
*  
*  calculates time to next interval according to 
*  mailbox's always online settings
* 
*  NOTE: this class is taken from S60 Email client,
*  Some parts of this class may not be used in FSEmail
*    
*  @since FSEmail 2.0
*/
NONSHARABLE_CLASS (CIpsSosAOSchedulerUtils) : public CBase
    {
public:  // Constructors and destructor

    /**
    * NewLC
    * @return, Constructed object
    */
    //<Qmail>
    static CIpsSosAOSchedulerUtils* NewLC();
    //</Qmail>
    /**
    * NewL
    * @return, Constructed object
    */
    //<Qmail>
    static CIpsSosAOSchedulerUtils* NewL();
    //</Qmail>
    /**
    * Destructor
    */
    virtual ~CIpsSosAOSchedulerUtils();

public: 
        
    /**
    * checks is mailbox allowed to connect from settings
    * NOTE: not used in FSEmail 2.0
    */
    TBool CanConnect();
        
    /**
    * Calculates seconds to next interval time according to
    * mailbox settings
    */
    TIpsSosAOConnectionType SecondsToNextMark( 
        TTimeIntervalSeconds& aSeconds );
    
private:

    /**
    * Constructor
    */
    //<Qmail>
    CIpsSosAOSchedulerUtils();
    //</Qmail>
    /**
    * Symbian 2-phase constructor
    */
    void ConstructL();

    /**
    *
    */
    void PrepareScheduling(
        TTime& aClock,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );

    /**
    *
    */
    TBool CanConnect(
        const TTime& aClock,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );
                        
    /**
    *
    */
    TIpsSosAOValidConnectionDay GetValidConnectionDay( 
        const TTime& aClock );
        
    /**
    *
    */
    TDay GetNextDay( 
        const TDay aToday, 
        const TInt aNth = 1 );
              
    /**
    *
    */
    TDay GetPrevDay( 
        const TDay aToday, 
        const TInt aNth = 1 );
        
    /**
    *
    */
    TBool IsValidTimeToConnect( 
        const TIpsSosAOValidConnectionDay aValidDay,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );

    /**
    *
    */
    TDateTime AdjustHomeTime( 
        const TTime& aClock );
            
    /**
    *
    */
    void Times2Seconds(
        const TTime& aClock,
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );
                
    /**
    *
    */
    void OrganizeStartAndStopTimes(
        TTimeIntervalSeconds& aHome,
        TTimeIntervalSeconds& aStart,
        TTimeIntervalSeconds& aStop );
                    
    /**
    *
    */
    void CalculateSecondsToNextMark(
        const TIpsSosAOConnectionType aConnectionStatus,
        TTimeIntervalSeconds& aSeconds,
        const TTime& aClock,
        const TTimeIntervalSeconds& aHome,
        const TTimeIntervalSeconds& aStart,
        const TTimeIntervalSeconds& aStop );

    /**
    *
    */
    TTimeIntervalSeconds CalcSecsToNextScheduledDay(
        const TTime& aClock, 
        const TTimeIntervalSeconds& aHome,
        const TBool aScheduledDay );

    /**
    *
    */
    void CalcSecsToMark(
        TTimeIntervalSeconds& aSeconds,
        const TTime& aClock,
        const TTimeIntervalSeconds& aHome,
        const TTimeIntervalSeconds& aBonus,
        const TBool aScheduledDay );

    /**
    *
    */
    TBool IsDaySelected( const TUint aDay ) const;
        
private:

    // Reference to extended settings
    //<Qmail>
    
    //</Qmail>
    };


#endif /*IPSSOSAOSCHEDULERUTILS_H_*/
