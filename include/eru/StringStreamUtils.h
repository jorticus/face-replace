//////////////////////////////////////////////////////////////////////
/**

\file    StringStreamUtils.h  

\brief   String and stream utilities within the namespace eru.

*/
//////////////////////////////////////////////////////////////////////

#ifndef __ERU_STRINGSTREAM_H__
#define __ERU_STRINGSTREAM_H__

//////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>

//////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////

/// Maximum length of paths/filenames.
#ifndef ERU_MAX_PATH
#ifdef MAX_PATH
#define ERU_MAX_PATH MAX_PATH
#else
#define ERU_MAX_PATH 255
#endif
#endif

namespace eru {

//////////////////////////////////////////////////////////////////////
//
// Stream utils.
//
//////////////////////////////////////////////////////////////////////


  /// \brief Get the next line from a stream.
  ///
  /// The line is read to and returned as a string.
  /// A maximum of 10000 empty lines is ignored.
  
	inline std::string getNextLine(std::istream& is) {
		std::string line;
		//is.ignore(10000, '\n');
		do {
			std::getline(is, line);
		}while(line.length() == 0);
		return line;
	}


  /// \brief Read past commented lines in a stream.
  ///
  /// A commented line is a line starting with a given character (defaults to '#').
  /// Empty lines are skipped as well.
	inline void skipComments(std::istream& is, char comment = '#') {
	    char next = is.peek();
	    while( next == '\n' || next == comment ) {
		    is.ignore( 10000, '\n' );
	        next = is.peek();
        }
	}


//////////////////////////////////////////////////////////////////////
//
// Filename utils.
//
//////////////////////////////////////////////////////////////////////


	/// \brief Extracts the path from a path+filename.
	///
	/// Ruthlessly stolen from osgDB.
	inline std::string getFilePath(const std::string& fileName) {
			// try unix directory slash first.
			std::string::size_type slash = fileName.find_last_of('/');
			if (slash==std::string::npos) {
					// then try windows directory slash.
					slash = fileName.find_last_of('\\');
					if (slash==std::string::npos) return std::string();
			}
			return std::string(fileName,0,slash);
	}


	/// \brief Extracts the filename from a path+filename.
	///
	/// Ruthlessly stolen from osgDB.
	inline std::string getSimpleFileName(const std::string& fileName) {
			// try unix directory slash first.
			std::string::size_type slash = fileName.find_last_of('/');
			if (slash==std::string::npos) {
					// then try windows directory slash.
					slash = fileName.find_last_of('\\');
					if (slash==std::string::npos) return fileName;
			}
			return std::string(fileName.begin()+slash+1,fileName.end());
	}


	/// \brief Extracts the filename extension from a path+filename.
	///
	/// Ruthlessly stolen from osgDB.
	inline std::string getFileExtension(const std::string& fileName) {
			std::string::size_type dot = fileName.find_last_of('.');
			if (dot==std::string::npos) 
				return std::string("");
			return std::string(fileName.begin()+dot+1,fileName.end());
	}


	/// \brief Extracts the filename extension (in lower case) from a path+filename.
	///
	/// Ruthlessly stolen from osgDB.
	inline std::string getLowerCaseFileExtension(const std::string& filename) {
	    std::string ext = eru::getFileExtension(filename);
		for (std::string::iterator itr=ext.begin(); itr!=ext.end(); ++itr) {
		   * itr = tolower(*itr);
		}
		return ext;
	}


	/// \brief Extracts the stripped filename (without extension) from a path+filename.
	///
	/// Ruthlessly stolen from osgDB.
	inline std::string getStrippedName(const std::string& fileName) {
	    std::string simpleName = getSimpleFileName(fileName);
		std::string::size_type dot = simpleName.find_last_of('.');
		if (dot==std::string::npos) 
			return simpleName;
		return std::string(simpleName.begin(),simpleName.begin()+dot);
	}


  /// Leading zeros before a number: 1->"0", 11->""
  inline const char* leadzeros2(int i) { return ( i<10 ? "0" : "" ); }

  /// Leading zeros before a number: 1->"00", 11->"0", 111->""
  inline const char* leadzeros3(int i) { return ( i<10 ? "00" : ( i<100 ? "0" : "" ) ); }

  /// Leading zeros before a number: 1->"0000", 111->"00", 11111->""
  inline const char* leadzeros5(int i) { return ( i<10 ? "0000" : ( i<100 ? "000" : ( i<1000 ? "00" : ( i<10000 ? "0" : "" )))); }

//////////////////////////////////////////////////////////////////////
//
// Conversion utils.
//
//////////////////////////////////////////////////////////////////////
  
  /// \brief Converting int to std::string
  inline std::string itos(int arg) {
      std::ostringstream buffer;
      buffer << arg; // send the int to the ostringstream
      return buffer.str(); // capture the string
  }


//////////////////////////////////////////////////////////////////////
//
// Time and date strings
//
//////////////////////////////////////////////////////////////////////


#ifdef _WINBASE_

  /// \brief Returns a string containing the current date (YYYY-MM-DD).
	///
	/// Only available under Windows.
	inline std::string getDateString() {
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		std::stringstream ss;
		ss
			<< sysTime.wYear
			<< "-"
			<< leadzeros2( sysTime.wMonth ) << sysTime.wMonth
      << "-"
			<< leadzeros2( sysTime.wDay ) << sysTime.wDay;
		return ss.str();
	}

  /// Returns a string containing the current time (HH:MM:SS).
	inline std::string getTimeString() {
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		std::stringstream ss;
		ss
			<< leadzeros2( sysTime.wHour ) << sysTime.wHour
			<< ":"
			<< leadzeros2( sysTime.wMinute ) << sysTime.wMinute
			<< ":"
			<< leadzeros2( sysTime.wSecond ) << sysTime.wSecond;
		return ss.str();
	}

  /// \brief Pointer to a string containgin the current time (HH:MM:SS).
	///
	/// Obsolete; use getDateString() instead.
	inline const char*  TimeString() {
		static char s[20];
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		sprintf_s(s, "%s%d%s%d%s%d",
			(sysTime.wHour<10?" 0":" "),   sysTime.wHour,
			(sysTime.wMinute<10?":0":":"), sysTime.wMinute,
			(sysTime.wSecond<10?":0":":"), sysTime.wSecond);
		return s;
	}

  /// \brief Pointer to a string containing the current date (YYYY-MM-DD).
	///
	/// Obsolete; use getDateString() instead.
	inline const char*  DateString() {
		static char s[20];
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		sprintf_s(s, "%d%s%d%s%d", 
			sysTime.wYear,
			(sysTime.wMonth<10?"-0":"-"), sysTime.wMonth,
			(sysTime.wDay<10?"-0":"-"), sysTime.wDay);
		return s;
	}

#endif // _WINBASE_

} // namespace

#endif // __ERU_STRINGSTREAM_H__
