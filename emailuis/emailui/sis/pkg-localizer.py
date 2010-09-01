# -*- coding: ISO-8859-1 -*-
"""
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
* Description: Python script that can be used to make localized pkg-files 
* based on Engineering English pkg-files. Meant only for internal use in Email.
*
*/
"""

import sys, string

LINE_FEED = "\n"
INDENTATION = "    "

# Added to converted file name, so e.g. "filename.pkg" -> filename_loc.pkg"
CONVERTED_FILE_NAME_ADDITION = "_loc"
# Path identifiers for the paths that include files to be localized
PATHS_TO_LOCALIZE = [ "resource\\" ]

# Exclude paths of ECom plugin registeration files and extended startup list ctrl files
PATHS_TO_EXCLUDE = [ "resource\\plugins\\", "private\\101f875a\\", "resource\\messaging\\", "resource\\fsmailbrandmanager.r",
                     "\\esmragnversit2strings.rsc", "\\esmrurlparserplugindata.rsc", "\\esmrpolicies.rsc", "\\esmralarminfo.rsc", # Non-localized resource files from mrui
                     "\\neascpnodenames.rsc", "\\neasicalstrings.rsc", "\\neasfreestylepluginresource.rsc" ] # Non-localized resource files from eas

# File extension identifiers for the files to be localized
EXTENSIONS_TO_LOCALIZE = [ ".rsc" ]
# Embedded sisx packages to be localized
EMBEDDED_PACKAGES_TO_LOCALIZE = [ "eas_engine.sisx", "eas_engine_udeb.sisx" ]
EMBEDDED_PACKAGE_LINE_IDENTIFIER = "@"
EMBEDDED_PACKAGE_FILE_EXTENSION = ".sisx"

# List of language codes
# LANGUAGE_CODES = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 14, 15, 16, 17, 18, 25, 26, 27, 28, 29, 30, 31, 32, 33, 37, 39, 42, 44, 45, 49, 50, 51, 54, 57, 58, 59, 67, 68, 70, 76, 78, 79, 83, 93, 94, 129, 157, 158, 159, 160, 161, 326, 327, 401, 402 ]
LANGUAGE_CODES = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 14, 15, 16, 17, 18, 25, 26, 27, 28, 29, 30, 31, 32, 33, 37, 39, 42, 44, 45, 49, 50, 51, 54, 57, 59, 67, 68, 70, 76, 78, 79, 83, 93, 94, 96, 102, 103, 129, 157, 158, 159, 160, 161, 326, 327 ]

# Comment line added with commented-out engineering english resource file
PKG_FILE_COMMENT_LINE_START_TAG = ";"
LOCALIZATION_COMMENT_LINE = PKG_FILE_COMMENT_LINE_START_TAG + " Following line is localized, so this Engineering English line is commented out\n"

LOCALIZATION_LANG_CODE_IDENTIFIER = "***"
LOCALIZATION_LANG_CODE_BLOCK_START_LINE = 'IF EXISTS( "z:\\resource\\avkon.r' + LOCALIZATION_LANG_CODE_IDENTIFIER + '" )' + LINE_FEED
LOCALIZATION_LANG_CODE_BLOCK_END_LINE = 'ENDIF' + LINE_FEED + LINE_FEED

# Identifier to identify the line of the starter app, which needs to be the last item in pkg file
STARTER_APP_IDENTIFIER = "sys\\bin\\emailservermonitor.exe"

HELP_TEXT0 = "Python script that can be used to generate localized pkg-file based on engineering English pkg-file.\nIt converts e.g. all *.rsc file names in input file to *.r01, *.r02, *.r03 etc. file names.\n"
HELP_TEXT1 = "Usage:\n    "
HELP_TEXT2 = " engineering_english_to_convert.pkg\n"
HELP_TEXT3 = "Output file name format:\n    engineering_english_to_convert"
HELP_TEXT4 = ".pkg\n"

##########################################################################
# MAIN
#
def main():
	if( len(sys.argv) == 1 ):
		ind = sys.argv[0].rfind( "\\" )
		if( ind >= 0 and len(sys.argv[0]) >= ind+2 ):
			script_file = sys.argv[0][ind+1:]
		else:
			script_file = sys.argv[0]
		
		print HELP_TEXT0
		print HELP_TEXT1 + script_file + HELP_TEXT2
		print HELP_TEXT3 + CONVERTED_FILE_NAME_ADDITION + HELP_TEXT4
		return
		
	files = sys.argv[1:]
	for file_name in files:
		ConvertFile( file_name )
	
	return


def ConstructLocalizedLines( lines_to_localize, converted_lines ):
	for lang_code in LANGUAGE_CODES:
		lang_code = str( lang_code )
		lang_code = lang_code.zfill(2)

		start_line = LOCALIZATION_LANG_CODE_BLOCK_START_LINE
		converted_lines.append( start_line.replace( LOCALIZATION_LANG_CODE_IDENTIFIER, lang_code ) )

		for line in lines_to_localize:
			extension = line[1]
			line = line[0]
			converted_lines.append( INDENTATION + line.replace( extension, extension[0:2] + lang_code ) )

		converted_lines.append( LOCALIZATION_LANG_CODE_BLOCK_END_LINE )

	return

	
def DivideConvertedLines( converted ):
	footer = []
	
	starter_app_found = False
	i = len( converted ) - 1
	while( i >= 0 ):
		line = converted[i]
		# Append all files below STARTER_APP_IDENTIFIER to footer, including
		# also the line where STARTER_APP_IDENTIFIER is found
		if not( starter_app_found ):
			footer.insert( 0, line )
			if( line.find( STARTER_APP_IDENTIFIER ) >= 0 ):
				starter_app_found = True

		# Append also all the pure comment lines before the starter app line,
		# because they are most probably related to starter app. When we
		# found first non comment line, we can quit
		else:
			if( (len(line) > 0) and (line[0] == PKG_FILE_COMMENT_LINE_START_TAG) ):
				footer.insert( 0, line )
			else:
				# First non comment line found, quit the while loop
				break

		i = i - 1
		
	# If starter app is not found, then there's no need to divide anything,
	# so keep the original converted list untouched and return empty footer
	if not( starter_app_found ):
		return []

	# If starter app is found, remove the footer lines from the original
	# converted list and then return the footer
	del converted[i+1:]
	return footer


def ConvertFile( file_name ):
	data = ReadFileToList( file_name )
	
	converted = []
	lines_to_localize = []
	for line in data:
		line_localized = False
		for path in PATHS_TO_LOCALIZE:
			# Check does this line contain path to be localized
			if( (line.find( path ) >= 0) and not(LineIncludesExcludedPath( line.lower() ))  ):
				for extension in EXTENSIONS_TO_LOCALIZE:
					if( line.lower().find( extension ) >= 0 ):
						lines_to_localize.append( [line.lower(), extension] )
						converted.append( LOCALIZATION_COMMENT_LINE )
						converted.append( PKG_FILE_COMMENT_LINE_START_TAG + line )
						line_localized = True

				break # for path in PATHS_TO_LOCALIZE
				
		if not( line_localized ):
			# Check does this line include some embedded sis-package that is localized
			if( line.find( EMBEDDED_PACKAGE_LINE_IDENTIFIER) >= 0 ):
				for embedded_pkg in EMBEDDED_PACKAGES_TO_LOCALIZE:
					if( line.lower().find( embedded_pkg.lower() ) >= 0 ):
						# This is embedded sis-package that is localized
						# -> Change the name to *_loc.sisx
						extension_pos = line.lower().find( EMBEDDED_PACKAGE_FILE_EXTENSION.lower() )
						converted.append( line[:extension_pos] + CONVERTED_FILE_NAME_ADDITION + line[extension_pos:] )
						line_localized = True

		if not( line_localized ):
			converted.append( line )
	
	converted_footer = DivideConvertedLines( converted )

	# Add extra line feed just in case as it seems to be needed with some files.
	# Not needed if adding localised list to the middle of the file.
	if( len(converted_footer) == 0 ):
		converted.append( LINE_FEED )
	
	if( len( lines_to_localize ) > 0 ):
		ConstructLocalizedLines( lines_to_localize, converted )
	
	WriteListToFile( ConvertedFileName(file_name), converted, converted_footer )
	
	return


def LineIncludesExcludedPath( line ):
	for path in PATHS_TO_EXCLUDE:
		if( line.find( path ) >= 0 ):
			return True
	
	return False
	

def ConvertedFileName( file_name ):
	ind = file_name.rfind( "." )
	if( ind < 0 ):
		converted_file_name = file_name + CONVERTED_FILE_NAME_ADDITION
	else:
		converted_file_name = file_name[:ind] + CONVERTED_FILE_NAME_ADDITION + file_name[ind:]
	
	return converted_file_name
	
	
def WriteListToFile( file_name, data1, data2 ):
	file_handle = open( file_name, 'w' )
	for line in data1:
		file_handle.write( line )

	for line in data2:
		file_handle.write( line )
	
	file_handle.close()
	
	return


def ReadFileToList( file_name ):
	file_handle = open( file_name, 'r' )
	data = [ ]
	for line in file_handle.readlines():
		data.append( line )
	
	file_handle.close()
	
	return data


##########################################################################
# The End Complete
#
if __name__ == '__main__':
    main()
