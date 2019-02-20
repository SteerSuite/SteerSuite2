#ifndef __UTIL_RECORDER_H__
#define __UTIL_RECORDER_H__

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "Globals.h"

namespace Util {

	/**
	 * @brief A singleton class that can record anything anywhere
	 * 
	 * call record with record(fieldname, value) to add to the fieldname
	 * note that fielnames are shared accross the whole process
	 * call printall(path-to-file) to print all the recordings to file
	 * call restore(fieldname) to get back all the stored values of that field
	 * 
	*/
	class UTIL_API Recorder
	{
	public:
		void record(std::string field, float val);
		std::vector<float> restore(std::string field);
		void printall(std::string filename);
		static Recorder* instance();

	private:
		std::map < std::string, std::vector<float>> db;
		bool _isInst;
		static Recorder* rec;
	};

}

#endif