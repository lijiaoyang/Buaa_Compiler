#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

extern FILE * GraOutStream;//引用输出文件
extern FILE * GraInStream;

//头文件中变量不可以赋值
extern int index;//结构体数组下标
extern int count;//文件行数
extern int CodeFlag;//进入代码段(.text)的标志
extern int IsChar;//临时变量是char的标志
extern int ReturnFlag;

typedef enum
{
	VoidGra=0,IntGra,CharGra
}GraIdentType;

struct WordOut{
	int line;
	int OriLine;//在原文的行号
	char kind[10];
	char name[100];
}Str[409600];
struct ManySwitch{
	int switId;
	struct SwitchCaseState{
		int caseCon;	//常量值
		char SwitchGoto[200];
	}SwitchCase[1024];
}Many[4096];

int CopyIntoStruct();
void nextsym();
void Plus();
void Mult();
void Rela();
void Alpha();
void Digital();
void Chara();
void Strings();
void Program();
void ConstantDescription();
void ConstantDefinition();
void UnsignedInteger();
void Integer();
void Identifier();
void DeclareHead();
void VariableDescription();
void VariableDefinition();
void TypeIdentifier();
void ReturnValueFunctionDefinition();
void NoReturnValueFunctionDefinition();
void CompoundStatement();
void ParametersTable();
void MainFunction();
void Expression();
void Item();
void Factor();
void Statement();
void AssignmentStatement();
void ConditionalStatement();
void Condition();
void LoopStatement();
void Constant();
void SituationStatement();
void SituationTable(char tmp[]);
void SituationSubSentence(char tmp[]);
void ReturnValueFunctionCallStatement();
void NoReturnValueFunctionCallStatement();
void ValueParameterTable(char fun[]);
void StatementColumn();
void ReadSentences();
void WriteStatement();
void ReturnStatement();

int GraFindTabType(int CurStrId);
int GraFindTabObj(int CurStrId);
int findIdentT_G(char name[]);	//先从当前层次开始找，如果找不到再找全局
int findInTab(char name[]);
#endif  /*__GRAMMAR_H__*/