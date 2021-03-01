#include "count.h"
#include "utility.h"

int Counter::paramCount = 0;
int Counter::globaCount = 0;

string Counter::GenParamCode() {
	stringstream trans;
	string paramCode;

	trans << "a" << paramCount;
	trans >> paramCode;

	RegManager::RegStore(paramCode);

	paramCount += 1;

	return paramCode;
}

string Counter::GenGlobaCode() {
	stringstream trans;
	string globaCode;

	trans << "v" << globaCount;
	trans >> globaCode;

	globaCount += 1;

	return globaCode;
}

void Counter::ParamCountInit() {
	paramCount = 0;
}
