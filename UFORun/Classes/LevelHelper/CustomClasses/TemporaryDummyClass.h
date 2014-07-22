//This header file was generated automatically by LevelHelper
//based on the class template defined by the user.
//For more info please visit: www.levelhelper.org


#ifndef LH_TemporaryDummyClass_H
#define LH_TemporaryDummyClass_H

#include <string>
#include <vector>
#include "LHAbstractClass.h"
class LHDictionary;

class TemporaryDummyClass :public LHAbstractClass
{

public:

	TemporaryDummyClass(void){}
	virtual ~TemporaryDummyClass(void);

	virtual std::string className(){return std::string("TemporaryDummyClass");}

	virtual void setPropertiesFromDictionary(LHDictionary* dictionary);




};

#endif
