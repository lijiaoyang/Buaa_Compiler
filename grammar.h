#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

extern FILE * GraOutStream;//��������ļ�
extern FILE * GraInStream;

//ͷ�ļ��б��������Ը�ֵ
extern int index;//�ṹ�������±�
extern int count;//�ļ�����
extern int CodeFlag;//��������(.text)�ı�־
extern int IsChar;//��ʱ������char�ı�־
extern int ReturnFlag;

typedef enum
{
	VoidGra=0,IntGra,CharGra
}GraIdentType;

struct WordOut{
	int line;
	int OriLine;//��ԭ�ĵ��к�
	char kind[10];
	char name[100];
}Str[409600];
struct ManySwitch{
	int switId;
	struct SwitchCaseState{
		int caseCon;	//����ֵ
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
int findIdentT_G(char name[]);	//�ȴӵ�ǰ��ο�ʼ�ң�����Ҳ�������ȫ��
int findInTab(char name[]);
#endif  /*__GRAMMAR_H__*/