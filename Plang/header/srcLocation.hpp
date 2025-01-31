#ifndef PIETRA_SRC_LOCATION
#define PIETRA_SRC_LOCATION

class SrcLocation {
public:
	const char*				name;
	int						lineNumber;
	int						lineOffset;
	
public:
	SrcLocation();
	SrcLocation(const char* name, int lineNumber);
	void advanceLine();
	void addOffset();
	void newContext(const char* filename);
};

#endif /*PIETRA_SRC_LOCATION*/