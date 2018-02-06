#ifndef __IntermediateCode_H__
#define __IntermediateCode_H__
#define Length 100
/*
	Gcon   全局常量定义
	Gvar   全局变量定义
	Garr   全局数组定义
	Func   函数定义
	Para   参数定义
	Tcon   临时常量定义
	Tvar   临时变量定义
	Tarr   临时数组定义
	Grea   大于
	GaE    大于等于
	Less   小于
	LaE    小于等于
	NaE    不等于
	Equ    等于
	Goto   跳转
	TGoto  为正跳转
	Ret    返回   return(x)
	RetS   返回值  m = fun(2)
	Add    加
	Min    减
	Mul    乘
	Div    除
	Neg    取负
	Ass    赋值
	Call   函数调用
	Push   值压栈
	End    函数结尾
	ArrA   数组赋值
	ArrG   数组取值
	Scan   读语句
	Prin   写语句
	Swit   情况语句
	ThisLabel 添加标签
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
	Instr ins;//指令
	CodeType type;//类型
	char name1[Length];//名字1
	char name2[Length];//名字2
	char result[Length];//结果
	int FunLen;//用地址adr来转换，它表示函数的地址长度
	int	PushFlag;
	int IsCharFlag;
}MediaTab[1024];
extern FILE * InterCodeStream;//输出文件
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

//函数部分
//void FuncEnd(Instr Interins);
void AddLabel(char Intername1[]);
void InsName1(Instr Interins,char Intername1[]);
void InsTypeName1(Instr Interins,CodeType Intertype,char Intername1[]);
void InsName1Res(Instr Interins,char Intername1[],char Interresult[]);
void InsTypeName1Res(Instr Interins,CodeType Intertype,char Intername1[],char Interresult[]);
void InsName12Res(Instr Interins,char Intername1[],char Intername2[],char Interresult[]);

#endif /*__IntermediateCode_H__*/