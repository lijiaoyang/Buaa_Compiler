#ifndef __KindsOfTabs_H__
#define __KindsOfTabs_H__

#define MaxName 100
/*
	标识符类型type
	VOID 0
	INT  1
	CHAR 2
*/
typedef enum
{
	VOID=0,INT,CHAR
}IdentType;
/*
	标识符种类obj
	PARA  2    参数
	FUNC  3    函数名
	STR   4    字符串
	GVAR  5    全局变量
	GCON  6    全局常量
	GARR  7    全局数组
	TVAR  8    临时变量
	TCON  9   临时常量
	TARR  10   临时数组
	LABEL 11   标签(用于跳转)
	GTVAR 12   全局临时变量（如tmp1）
	TTVAR 13   局部临时变量（如tmp1）
*/
typedef enum
{
	PARA=0,FUNC,GVAR,GCON,GARR,TVAR,TCON,TARR,LABEL,GTVAR,TTVAR,NOREEND,REEND,MAINEND
}IdentObj;
extern int TabNum;
extern int Lev;
extern int adr;
extern int IntoFun;
extern int IntoMain;
extern int LabelNum;
extern int VarNum;
extern int EndNum;
extern int SwitLabNum;
extern int SwitEndNum;
extern char *PrinType[10];
extern char *PrinObj[30];
/*定义符号表类型*/
struct{
	char name[MaxName];//标识符名字
	IdentType type;//标识符类型  INT CHAR
	IdentObj obj;//标识符种类   VAR PARA FUNC ARR等等
	int lev;//标识符所在分程序的静态层次，规定主程序层次为1，嵌套分程序层次逐一递增
	int value;//存值
	/*对于常量，填入相应的整数值
	*对于数组， 填入所需存储单元的数目（即大小）
	*/
	int adrFirst;
	/*对于变量，该变量在运行栈S中分配存储单元的 相对地址
	*对于函数，填入目标代码入口地址
	*对于数组，填开始地址
	*/
	int AllParaNum;
}Tab[4096000];	

//录入符号表
int enterTab(char enT_name[],IdentType enT_type,IdentObj enT_obj,int enT_value,int enT_adr);
IdentType FindFuncName(char name[]); 
int findDef(char name[]);
void findResult(int i_findResult);
void findTmpSame(int i_findTmpSame);
int findArrLen(int id);
void LableIntoMTab();  
void AddCaseLabel(int i);
void makeVar(char name[]);
void makeEndLabel();
void MakeSwitchEndLabel();

#endif /*__KindsOfTabs_H__*/