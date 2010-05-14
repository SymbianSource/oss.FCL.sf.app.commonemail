/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Key to Central Repository to be used by Branding utilities
*
*/

#ifndef NMCENREPKEYS_H
#define NMCENREPKEYS_H


namespace NmBrandingApi
{
    /*!
      
       Here are uids in central repository to be in use by branding.
       
       <b>General settings for branding.</b>
       
       EMAIL_CENREP = 0x2001E277
       Uid of central repository for branding api
       
       RCC_PATH = 0xfffffffe;
       Path to resource file with icons.
       Example value: epoc32/data/Z/Resource/apps/email_brand.rcc 
       
       <b>Settings value for all brand icons and mailbox names</b>
       Values in repository for brands are constructed in that way:
       BRAND;regexp;MAIL_NAME;FILE_ALIAS
       Where
       BRAND brand from domain, for example yahoo
       regexp is regular expression to match brand domain
       MAIL_NAME is name of mail, for example Yahoo! Mail
       FILE_ALIAS is file alias from resource that representant selected file,
                  for example yahoo
       Example value:
       gmail;.*gmail(\.com|\.com.pl|\.de)$;GMail;gmail
       
       <b>List of all brands in cenrep:</b>
       GOOGLE_MAIL = 0x00000001
       Value: googlemail;googlemail\.com$;Google Mail;gmail
       
       GMAIL_MAIL = 0x00000002
       gmail;gmail\.com$;Gmail;gmail
       
       YAHOO_MAIL = 0x00000003
       yahoo;.*yahoo\.(((com(\.ar|\.br|\.cn|\.hk|\.mx|\.sg|\.tw|\.my|\.vn|\.ph|\.tr|\.co|\.pe|\.ve)?)$)|
                       (co(\.kr|\.uk|\.id|\.th|\.in)$)|
                       ((de|dk|es|fr|gr|ie|it|no|se|ca|cn|in|pl|cl)$));Yahoo! Mail;yahoo"
                       
       YMAIL_MAIL = 0x00000004
       ymail;.*ymail\\.com;Yahoo! Mail;yahoo
       
       GEOCITIES_MAIL = 0x00000005
       geocities;.*geocities\\.com;Yahoo! Mail;yahoo
       
       ROCKETMAIL_MAIL = 0x00000006
       rocketmail;.*rocketmail\\.com;Yahoo! Mail;yahoo
       
       WANS_MAIL = 0x00000007
       wans;.*wans\\.net;Yahoo! Mail;yahoo
       
       AOLCHINA_MAIL = 0x00000008
       aolchina;.*aolchina\\.com;AOL Mail;aol
       
       AOL_MAIL = 0x000000009
       aol;.*aol\\.(((co\\.(uk|nz))$)|
                   ((com(\\.au|\\.ar|\\.co|\\.ve|\\.mx|\\.tr|\\.br)?)$)|
                   ((cz|ch|be|cl|kr|ru|de|nl|es|fr|dk|at|it|hk|se|tw|jp)$));AOL Mail;aol
                   
       AIM_MAIL = 0x0000000A
       aim;.*aim\\.com$;AIM Mail;aol
       
       AOL_NO_MAIL = 0x0000000B
       aol;.*aol\\.no$;AOL epost;aol
       
       AOL_PL_MAIL = 0x0000000C
       aol;.*aol\\.pl$;AOL Poczta;aol
       
       AOL_FI_MAIL = 0x0000000D
       aol;.*aol\\.fi$;AOL Sähköposti;aol
       
       Y7MAIL_MAIL = 0x0000000E
       y7mail;.*y7mail\\.com$;Yahoo!7 Mail;yahoo
       
       YAHOO_AU_MAIL = 0x0000000F
       yahoo;.*yahoo\\.com\\.au$;Yahoo!7 Mail;yahoo
       
       AMERITECH_MAIL = 0x00000010
       ameritech;ameritech\\.net$;AT&T Yahoo! Mail;yahoo
       
       FLASH_MAIL = 0x00000011
       flash;flash\\.net$;AT&T Yahoo! Mail;yahoo
       
       NVBELL_MAIL = 0x00000012
       nvbell;nvbell\\.net$;AT&T Yahoo! Mail;yahoo
       
       PACBELL_MAIL = 0x00000013
       pacbell;pacbell\\.net$;AT&T Yahoo! Mail;yahoo
       
       PRODIIGY_MAIL = 0x00000014
       prodigy;prodigy\\.net$;AT&T Yahoo! Mail;yahoo
       
       SBCGLOBAL_MAIL = 0x00000015
       sbcglobal;sbcglobal\\.net$;AT&T Yahoo! Mail;yahoo
       
       BELLSOUTH_MAIL = 0x00000016
       bellsouth;bellsouth\\.net$;AT&T Yahoo! Mail;yahoo
       
       SNET_MAIL = 0x00000017
       snet;snet\\.net$;AT&T Yahoo! Mail;yahoo
       
       SWBELL_MAIL = 0x00000018
       swbell;swbell\\.net$;AT&T Yahoo! Mail;yahoo
       
       OVI_MAIL = 0x00000019
       ovi;.*ovi\\.com;Ovi Mail;ovi
       
       TALK21_MAIL = 0x0000001a
       talk21;talk21\\.com$;BT Yahoo! Mail;yahoo
       
       BTOPENWORLD_MAIL = 0x0000001b
       btopenworld;btopenworld\\.com$;BT Yahoo! Mail;yahoo
       
       ROGERS_MAIL = 0x0000001c
       rogers;((ort|nl|uat)\\.)?rogers\\.com$;Rogers Yahoo! Mail;yahoo
       
       VERIZON_MAIL = 0x0000001d
       verizon;verizon\\.net$;Verizon Yahoo! Mail;yahoo
       
       YAHOO_CO_NZ_MAIL = 0x0000001e
       yahoo;yahoo\\.co\\.nz$;Yahoo!Xtra Mail;yahoo
       
       MSN_MAIL = 0x0000001f
       msn;.*msn\\.com$;Hotmail;microsoft
                      
       HOTMAIL_MAIL_LIVE = 0x00000020
       live;.*live\\.((com(\\.(ar|au|mx|my|pe|pk|pt|sg|ve|ph))?)|
                      (co\\.(kr|uk|za))|
                      (at|be|ca|cl|cn|de|dk|fr|hk|it|jp|nl|no|ph|ru|se|ie|in))$;Hotmail;microsoft")
       
       MYAOL_MAIL = 0x00000021
       myaol;.*myaol\\.jp$;AOL Mail;aol
       
       HOTMAIL_MAIL = 0x00000022
       hotmail;.*hotmail((-int.com)|(\\.(com|co\\.uk|co\\.jp|de|fr|it|nl|es)))$;Hotmail;microsoft
                      
       LIVEMAIL_MAIL = 0x00000023
       livemail;livemail\\.(com.br|tw)$;Hotmail;microsoft
              
       WINDOWSLIVE_ES_COM_MAIL = 0x00000024
       windowslive;windowslive\\.(es|com)$;Hotmail;microsoft
       
       MIBLI_MAIL = 0x00000025
       mibli;.*mibli\\.com$;Hotmail;microsoft
       
       OXICASH_MAIL = 0x00000026
       oxicash;.*oxicash\\.in$;Hotmail;microsoft
       
       KIMO_MAIL = 0x00000027
       kimo;.*kimo\\.com;Yahoo! Mail;yahoo

       <b>White Label Branding</b>
       WLB_BRAND_NAME = 0xfffffffb
       Domain name needed for match by WLB.
       Example:
       sonera.com
       
       WLB_MAILBOX_NAME = 0xfffffffc
       Keep mailbox name for WLB mail
       Example:
       Sonera Mail
       
       WLB_ICON_PATH = 0xfffffffd
       Path to WLB icon.
       Example:
       z:/app/resource/sonera_brand.svg
       */
      static const qint32 EMAIL_CENREP = 0x2001E277;
      
      static const quint32 RCC_PATH = 0xfffffffe;

      static const quint32 WLB_BRAND_NAME = 0xfffffffb;
      
      static const quint32 WLB_MAILBOX_NAME = 0xfffffffc;
      
      static const quint32 WLB_ICON_PATH = 0xfffffffd;

}
    
#endif /*NMCENREPKEYS_H*/
