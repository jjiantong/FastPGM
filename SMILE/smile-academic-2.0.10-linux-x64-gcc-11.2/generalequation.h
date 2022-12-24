#ifndef SMILE_GENERAL_EQUATION_H
#define SMILE_GENERAL_EQUATION_H

// {{SMILE_PUBLIC_HEADER}}

#include <string>
#include <vector>

class DSL_expression;
class DSL_evalContext;
class DSL_extFunctionFactory;
class DSL_extFunctionContainer;

class DSL_generalEquation
{
public:
	DSL_generalEquation(const char *id = NULL);
	DSL_generalEquation(const DSL_generalEquation &other);
	~DSL_generalEquation();
    
	DSL_generalEquation& operator=(const DSL_generalEquation &other);

    // set both string and parsed representation
    int SetEquation(const std::string &eq, 
        int *errPos = NULL, std::string *errMsg = NULL, 
        std::vector<std::pair<DSL_expression *, int> > *positions = NULL,
        const DSL_extFunctionFactory *extFxnFactory = NULL); 

    const DSL_expression* GetLHS() const { return lhs; }
    const DSL_expression* GetRHS() const { return rhs; }
	bool IsDeterministic() const { return !bDistribution; }
	bool IsConstant() const { return bConstant; }

	void Write(std::string &s) const;

	void AppendRHS(const std::string &var);
	void MakeSum(const std::string &lhs, const std::vector<std::string> &rhs);
    void GetVariables(std::vector<std::string> &here) const;
    void RenameVariable(const std::string &oldId, const std::string &newId); // change [oldId] to [newId] in all occurences
    int SolveFor(const std::string &variable);
    void Simplify();
	void Assign(const std::string &variable, double value);
	void GeneralForm();

	void Bind(DSL_evalContext &ctx);
    void Patch(const DSL_extFunctionContainer &extFxn);

private:
	int DoSolveFor(const std::string &Xj);

    void DetermineEquationType();

	// An equation f*w = g+3 will be stored as the following:
    DSL_expression *lhs; // lhs -> f*w
    DSL_expression *rhs; // rhs -> g+3
    bool bDistribution; // if lhs or rhs contains distributions such as normal, uniform, etc.
	bool bConstant; // if the equation contains no distributions and only one variable
};

#endif
