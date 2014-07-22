//This header file was generated automatically by LevelHelper
//based on the class template defined by the user.
//For more info please visit: www.levelhelper.org


#ifndef LH_ABSTRACT_CLASS_H
#define LH_ABSTRACT_CLASS_H

class LHDictionary;

class LHAbstractClass{

public:
	virtual ~LHAbstractClass(){};
	virtual std::string className() { return std::string(); }
	virtual void setPropertiesFromDictionary(LHDictionary* dictionary) {}
};

#endif
