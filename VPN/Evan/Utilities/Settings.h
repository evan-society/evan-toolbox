/*
 * Settings.h
 *
 *  Created on: Nov 9, 2015
 *      Author: ynashed
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Singleton.h"
#include <string>

class Settings
{
private:
	bool m_registered;
	bool m_running;
	std::string m_workingDirectory;

public:
	Settings(): m_registered(false), m_running(false), m_workingDirectory("")
	{}
	bool isRegistered() 				const {return m_registered;}
	bool isRunning() 				  	const {return m_running;}
	const char* getWorkingDirectory() 	const {return m_workingDirectory.c_str();}

	void setRegistered(bool f) {m_registered = f;}
	void setRunning(bool f) {m_running = f;}
	void setWorkingDirectory(const char* dir) {m_workingDirectory = dir;}
};

typedef Singleton<Settings> Globals;

#endif /* SETTINGS_H_ */
