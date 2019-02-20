//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#include "LogManager.h"
#include "Logger.h"

LogManager* LogManager::_instance = new LogManager();

LogManager * LogManager::getInstance ()
{
	return _instance;
}

Logger * LogManager::createLogger ( const std::string &logName, LoggerType loggerType)
{
	switch (loggerType)
	{
	case LoggerType::BASIC_READ:
		_loggers[logName] = new Logger(logName, LogMode::Read);
		break;
	case LoggerType::BASIC_WRITE:
		_loggers[logName] = new Logger(logName, LogMode::Write);
		break;
	case LoggerType::BASIC_APPEND:
		_loggers[logName] = new Logger(logName, LogMode::Append);
		break;
	default:
		std::cerr << "Specified log type not supported \n\n";
		break;
	}

	return _loggers[logName];

}
