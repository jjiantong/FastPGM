#ifndef SMILE_GENERAL_H
#define SMILE_GENERAL_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <cfloat>
#include "idarray.h"

class DSL_node;

// definition of all SMILE status codes
#define DSL_OKAY                   0
#define DSL_GENERAL_ERROR         (-1)
#define DSL_OUT_OF_RANGE          (-2)
#define DSL_NO_ITEM               (-3)
#define DSL_INVALID_VALUE         (-4)
#define DSL_NO_USEFUL_SAMPLES     (-5)
#define DSL_CANT_SOLVE_EQUATION   (-6)
#define DSL_CYCLE_DETECTED        (-11)
#define DSL_WRONG_NUM_STATES      (-19)
#define DSL_CONFLICTING_EVIDENCE  (-26)
#define DSL_ILLEGAL_ID            (-30)
#define DSL_DUPLICATED_ID         (-32)
#define DSL_OUT_OF_MEMORY         (-42)
#define DSL_ZERO_POTENTIAL		  (-43)
#define DSL_WRONG_NODE_TYPE       (-51)
#define DSL_WRONG_ELEMENT_TYPE    (-52)
#define DSL_INTERRUPTED           (-99)

//textfile
#define DSL_FILE_READ         (-100)
#define DSL_FILE_WRITE        (-101)
#define DSL_END_OF_FILE       (-102)  
#define DSL_WRONG_FILE        (-103)

// lexical/syntactical
#define DSL_NO_MORE_TOKENS        (-111)
#define DSL_LEXICAL_ERROR         (-126)
#define DSL_SYNTAX_ERROR          (-127)
#define DSL_UNEXPECTED_EOF        (-128)
#define DSL_FIELD_NOT_FOUND       (-129)

#define DSL_UNDEFINED_INT (INT_MIN+1) 
#define DSL_UNDEFINED_DOUBLE (DBL_MIN * 2) 
#define DSL_ILLEGAL_DOUBLE (DBL_MIN * 3) 
#define DSL_NOT_AVAILABLE (DBL_MIN * 4)
#define DSL_NOT_RELEVANT (DBL_MIN * 5)

#define DSL_FALSE 0
#define DSL_TRUE 1

#define DSL_FILE_LINE_LENGTH 1021
#define DSL_FILE_TOKEN_LENGTH (10 * DSL_FILE_LINE_LENGTH)

#define DSL_NUMBER_PRECISION 8
#define DSL_EPSILON 0.000005 

#if defined(_WIN32)
	#define DSL_stricmp _stricmp
	#define DSL_strnicmp _strnicmp
	inline bool DSL_isnan(double x) { return _isnan(x) != 0; }
#else
	#define DSL_stricmp strcasecmp
	#define DSL_strnicmp strncasecmp
	inline bool DSL_isnan(double x) { return std::isnan(x) != 0; }
#endif

inline bool DSL_isFinite(double x)
{
	return !DSL_isnan(x) && x <= DBL_MAX && x >= -DBL_MAX; 
}

double DSL_nan();
double DSL_inf();

bool DSL_isCharValidForIdentifier(char c, bool start);
bool DSL_isValidIdentifier(const char *id);
bool DSL_makeValidId(char *id);
bool DSL_nameToIdentifier(std::string &s, int outcomeIndex);

int DSL_stringToDouble(const char *s, double &out);
int DSL_doubleToString(double n, char *out, int precission = DSL_NUMBER_PRECISION);
int DSL_intToString(int theNumber, char *out);
int DSL_stringToInt(const char *s, int &out);
void DSL_appendInt(std::string &s, int x);
void DSL_appendDouble(std::string &s, double x);

bool DSL_isPermutation(const int* permutation, int size);


class DSL_header
{
public:
	DSL_header(const char* id);
	DSL_header(const DSL_header& src);
	virtual ~DSL_header();
	virtual int SetId(const char* id);
	int SetName(const char* name);
	int SetComment(const char* comment);
	const char* GetId() const { return id; }
	const char* GetName() const { return name; }
	const char* GetComment() const { return comment; }
	static int IsThisIdValid(const char* id);
	static int MakeValidId(char* id);

protected:
	void ReplaceId(const char* newId);

private:
	const char* id;       // For quick reference (including formulae), no spaces.
	const char* name;     // Longer name for display and printing, spaces allowed.
	const char* comment;  // A possibly longer text
};


class DSL_rectangle
{
public:
	DSL_rectangle() { center_X = center_Y = width = height = 0; }
	int FillDefaultValues(const DSL_rectangle& fromHere);
	void Set(int cX, int cY, int W, int H) { center_X = cX; center_Y = cY, width = W; height = H; }
	int center_X;
	int center_Y;
	int width;
	int height;
};


class DSL_screenInfo
{
public:
	DSL_screenInfo();
	int FillDefaultValues(const DSL_screenInfo& fromHere);

	DSL_rectangle position;
	int color;
	int selColor;
	int font;
	int fontColor;
	int borderThickness;
	int borderColor;
	bool barchartActive;
	int barchartWidth;
	int barchartHeight;
};


// DSL_textBoxList is just a typedef
typedef std::vector<std::pair<std::string, DSL_rectangle> > DSL_textBoxList;


class DSL_userProperties
{
public:
	int GetNumberOfProperties() const;
	const char* GetPropertyName(int index) const { return names[index]; }
	const char* GetPropertyValue(int index) const { return values[index]; }

	int AddProperty(const char* name, const char* value);
	int InsertProperty(int index, const char* name, const char* value);
	int ChangePropertyName(int index, const char* name);
	int ChangePropertyValue(int index, const char* value);
	int FindProperty(const char* name) const;
	int DeleteProperty(int index);

	void Clear();
#ifndef SMILE_NO_V1_COMPATIBILITY
	void CleanUp() { Clear(); }
#endif
private:
	DSL_idArray names;
	DSL_stringArray values;
};


class DSL_documentation
{
public:
	int GetNumberOfDocuments() const;
	const char* GetDocumentTitle(int index) const { return titles[index]; }
	const char* GetDocumentPath(int index) const { return paths[index]; }

	int AddDocument(const char* title, const char* path);
	int InsertDocument(int index, const char* title, const char* path);
	int ChangeDocumentTitle(int index, const char* title);
	int ChangeDocumentPath(int index, const char* path);
	int FindDocument(const char* title) const;
	int DeleteDocument(int thisOne);

	void Clear();
#ifndef SMILE_NO_V1_COMPATIBILITY
	void CleanUp() { Clear(); }
#endif
private:
	DSL_stringArray titles;
	DSL_stringArray paths;
};

#endif 
