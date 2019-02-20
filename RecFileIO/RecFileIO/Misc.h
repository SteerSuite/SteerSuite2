//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//




#ifndef __UTIL_MISC_H__
#define __UTIL_MISC_H__

/// @file Misc.h
/// @brief Declares miscellaneous utility functionality.

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

namespace Util {

	/// @name Platform-independent wrappers for string and filesystem queries.
	//@{
	/// Returns true if ch is the '/' character.
	bool isForwardSlash(char ch);
	/// Returns true if ch is the '\' character.
	bool isBackSlash(char ch);
	/// Returns true if the last segment of str is exactly equal to the extension; if you use this to test file extensions, make sure you explicitly have the extra period separator.
	bool endsWith( const std::string & str, const std::string & extension );
	/// Returns a string that without directory portion of the given path, and also strips the extension if it matches the specified extension.
	std::string basename( const std::string & path, const std::string & extension );
	/// Returns true if the path exists, whether it is a file or directory; does not test read/write/access permissions.
	bool pathExists( const std::string & pathname );
	/// Returns true if the file could be opened, false if file could not be opened; this does implicitly test permissions by explicitly trying to open the file.
	bool fileCanBeOpened(const std::string & filename);
	/// Returns true if the given string is an existing file (false if it is a directory or does not exist); but does not test read/write permissions.
	bool isExistingFile( const std::string & pathname );
	/// Returns true if the given string is a valid directory (false if it is not a directory or does not exist); but does not test read/write/access permissions.
	bool isExistingDirectory( const std::string & pathname );
	/// Returns all file names that have a given extension (or all files, if the given extension is blank); always excludes subdirectories.
	void getFilesInDirectory( const std::string & directoryName, const std::string & extension, std::vector<std::string> & fileNames);
	//@}

	/**
	 * @brief Returns a string of any data type that has the "<<" operator.
	 *
	 * Returns a string of any data type that has the "<<" operator.
	 */
	template<class T>
	static inline std::string toString( const T &value ) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	/**
	 * @brief Converts a reasonable string into a boolean, throws an exception if the string cannot be interpreted.
	 *
	 * The string is first converted to lower case, so that the tests are not case-sensitive.
	 * The following strings will convert to true: "1", "true", "yes"
	 * The following strings will convert to false: "0", "false", "no"
	 *
	 * any other strings will cause an exception to be thrown.
	 */
	bool getBoolFromString( const std::string & str );

} // end namespace Util



#endif
