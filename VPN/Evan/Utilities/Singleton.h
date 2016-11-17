/*
 * Singleton.h
 *
 *  Created on: Sep 8, 2014
 *      Author: ynashed
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

template <class T>
class Singleton
{
private:
	Singleton(Singleton const&);
	Singleton& operator=(Singleton const&);
	static T* m_instance;
protected:
	Singleton();
	~Singleton() {if(m_instance) delete m_instance;}
public:
	static T* getInstance()
	{
		if(!m_instance) m_instance = new T;
		return m_instance;
	}
};

template <class T> T* Singleton<T>::m_instance=0;
#endif /* SINGLETON_H_ */
