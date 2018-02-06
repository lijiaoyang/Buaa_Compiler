#ifndef __IntermediateCode_H__
#define __IntermediateCode_H__
#define Length 100
/*
	Gcon   ȫ�ֳ�������
	Gvar   ȫ�ֱ�������
	Garr   ȫ�����鶨��
	Func   ��������
	Para   ��������
	Tcon   ��ʱ��������
	Tvar   ��ʱ��������
	Tarr   ��ʱ���鶨��
	Grea   ����
	GaE    ���ڵ���
	Less   С��
	LaE    С�ڵ���
	NaE    ������
	Equ    ����
	Goto   ��ת
	TGoto  Ϊ����ת
	Ret    ����   return(x)
	RetS   ����ֵ  m = fun(2)
	Add    ��
	Min    ��
	Mul    ��
	Div    ��
	Neg    ȡ��
	Ass    ��ֵ
	Call   ��������
	Push   ֵѹջ
	End    ������β
	ArrA   ���鸳ֵ
	ArrG   ����ȡֵ
	Scan   �����
	Prin   д���
	Swit   ������
	ThisLabel ��ӱ�ǩ
*/
typedef enum{
	Gcon=0,Gvar,Garr,Func,Para,Tcon,Tvar,Tarr,Grea,GaE,Less,LaE,NaE,Equ,Goto,TGoto,FGoto,
	Ret,NRet,RetS,Add,Min,Mul,Div,Neg,Ass,CallBegin,CallEnd,Push,End,ArrA,ArrG,Scan,Prin,Swit,SwitEnd,ThisLabel
}Instr;
typedef enum
{
	Void=0,Int,Char,Label
}CodeType;
struct{
	Instr ins;//ָ��
	CodeType type;//����
	char name1[Length];//����1
	char name2[Length];//����2
	char result[Length];//���
	int FunLen;//�õ�ַadr��ת��������ʾ�����ĵ�ַ����
	int	PushFlag;
	int IsCharFlag;
}MediaTab[1024];
extern FILE * InterCodeStream;//����ļ�
extern int CodeNum;
extern int FunLen;
extern int CurFun_CodeNum;
extern int ParaNum;
extern char *PrinInstr[50];
extern char *PrinCodeType[10];
extern char ReArrG[100];
extern char ReFact[100];
extern char ReTerm[100];
extern char ReExpr[100];
extern char ReCond[100];
extern char LabelNumber[100];
extern char CaseNumber[100];
extern char ReRetS[100];
extern char ReNeg[100];
extern char ReInt[100];
extern char EndId[100];
extern char SwitBEid[100];
extern char emmEndSwit[100];

//��������
//void FuncEnd(Instr Interins);
void AddLabel(char Intername1[]);
void InsName1(Instr Interins,char Intername1[]);
void InsTypeName1(Instr Interins,CodeType Intertype,char Intername1[]);
void InsName1Res(Instr Interins,char Intername1[],char Interresult[]);
void InsTypeName1Res(Instr Interins,CodeType Intertype,char Intername1[],char Interresult[]);
void InsName12Res(Instr Interins,char Intername1[],char Intername2[],char Interresult[]);

#endif /*__IntermediateCode_H__*/