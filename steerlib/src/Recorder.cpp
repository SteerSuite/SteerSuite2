#include "Recorder.h"
#include <fstream>
#include <iostream>
#include <string>

namespace Util {

	Recorder* Recorder::rec = 0;

	Recorder* Recorder::instance()
	{
		if (!rec)
			rec = new Recorder;
		return rec;
	}

	void Recorder::record(std::string field, float val)
	{
		auto it = db.insert(std::pair<std::string, std::vector<float>>(field, std::vector<float>()));
		it.first->second.push_back(val);
	}

	std::vector<float> Recorder::restore(std::string field)
	{
		auto it = db.find(field);
		if (it == db.end())
			return std::vector<float>();
		else
			return it->second;
	}

	void Recorder::printall(std::string filename)
	{
		std::ofstream of;
		of.open(filename, std::ofstream::out | std::ofstream::trunc);
		std::cout << "**************************HELLO" << std::endl;
		for (auto it = db.begin(); it != db.end(); it++)
		{
			of << it->first << " ";
			for (auto vit = it->second.begin(); vit != it->second.end(); vit++)
				of << *vit << " ";
			of << "\n";
		}
		of.close();
	}
}