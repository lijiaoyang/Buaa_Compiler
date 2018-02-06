#ifndef __TargetCode_H__
#define __TargetCode_H__

extern FILE * TargetCodeStream;//����ļ�
extern FILE * TargetInStream;//���м�����ļ�

typedef enum
{
	NotMain=0,IsMain,JustInMain
}JudgeMain;
typedef enum
{
	Name1=0,Name2,Result
}LoadWhat;
typedef enum
{
	Rtype=0,Robj,Rlev,Rvalue,Radfirst,Rischar
}ReturnWhat;

struct{
	char name[100];//��ǰ����ջ������
	int RunPosi;//��ǰ���еĵ�ַ
	IdentType RunType;
	IdentObj RunObj;
	int GloIschar;
}RunStack[4096000];

struct{
	char con[100];//��ǰ����������
	int ParaPosi;//��ǰ�����ĵ�ַ
	char funName[100];
	int FourId;//��ǰ�Ƿ���Ч
}PushStack[40960];

extern int strFlag;
extern int FindStrId;
extern int LineAdd;
extern int RunPosi;
extern int RunIndex;
extern int switId;
extern int switCaseId;
extern int PushId;

extern char StrIdRes[100];
extern char *PushReg[100];
//extern int para_index;

void AddParaNumToFun();
void IntoRunStack();
void CalInStack();
void StoreData(int i_StoreData,char Reg[5],LoadWhat S_StoreData);
void LoadData(int i_LoadData,char Reg[5],LoadWhat l_LoadData,char name[]);
void DataDef(int i_DataDef);
void TmpDataDef(int i_TmpDataDef,JudgeMain ifMain);
void StrDef();
void AssDef(int i_AssDef);
void RetDef(int i_RetDef);
void NRetDef(int i_NRetDef);
void ReEndDef(int i_EndDef);
void MainEndDef(int i_MainEndDef);
void RetsDef(int i_RetsDef);
void ArrADef(int i_ArrADef);
void ArrGDef(int i_ArrGDef);
int SwitDef(int i_SwitDef);

void PrinStrs(int i_PrinStr);
void ScanIdent(int i_ScanIdent);
void CompareOp(int i_CompareOp);
void Arithmetic(int i_Arithmetic);
void SubSP(int i_SubSP);
int CallParaFun(int i_CallFun);
void ParaIntoStack(int i_ParaIntoStack);
void ParaOutStack();
void CallNoParaFun(int i_CallNoParaFun);
void FindStrLabel(char CurStr[]);
void SpliceStrId(char CurStr[]);
int FindAdr(int id_findadr,LoadWhat which,ReturnWhat Rwhat);
int FindTmpIdentAdr(int i_FindTmpIdentAdr,LoadWhat which,ReturnWhat rewhich);
int FindParaAdr(int i_FindParaAdr,int id_Para);
int FindFunLen(int i_FindFunLen);
int FindParaIndex(int i_FindParaIndex);
JudgeMain FindIdentLoc(int i_FindIdentLoc,LoadWhat l_FindIdentLoc);
int WhichCase(int i_WhichCase);


#endif /*__TargetCode_H__*/