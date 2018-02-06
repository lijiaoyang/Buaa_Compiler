#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KindsOfTabs.h"
#include "WordAnalysis.h"
#include "grammar.h"
#include "IntermediateCode.h"
#include "TargetCode.h"
#include "ErrorHandling.h"

int TabNum=0;//表的下标
int Lev = 0;
int adr = 0;
int IntoFun = 0;//表示进入函数的标记
int IntoMain = 0;//表示进入main函数
int LabelNum = 0;//标签名字
int VarNum = 0;//临时变量下标
int EndNum = 0;//End的下标
int SwitLabNum = 0;//Switch的case语句的label
int SwitEndNum = 0;
char *PrinType[10] = {"VOID","INT","CHAR"};
char *PrinObj[30] = {"PARA","FUNC","GVAR","GCON","GARR","TVAR","TCON","TARR","LABEL","GTVAR","TTVAR","NOREEND","REEND","MAINEND"};

int enterTab(char enT_name[],IdentType enT_type,IdentObj enT_obj,int enT_value,int enT_adr)
{
	strcpy(Tab[TabNum].name,enT_name);//分别送入新节点的数据域  标识符名称
	Tab[TabNum].type = enT_type;//标识符种类 INT CHAR VOID TMPVAR
	Tab[TabNum].obj = enT_obj;//标识符类型
	Tab[TabNum].lev = Lev;
	Tab[TabNum].value = enT_value;
	Tab[TabNum].adrFirst = enT_adr;
	//printf("-.-.-.-.-.-%d\n名称:%s已经录入符号表\n类型:%d\n种类:%d\n层次:%d\n常量值或数组大小:%d\n地址: %d\n",TabNum,enT_name,enT_type,enT_obj,Tab[TabNum].lev,enT_value,enT_adr);
	TabNum++;
	return TabNum;//返回地址，地址从1开始算
}
//查符号表返回函数类型 VOID 1  INT 2  CHAR 3
IdentType FindFuncName(char name[]) 
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(strcmp(strlwr(name),Tab[i].name)==0 && Tab[i].type == VOID)//表示是无返回值的函数
			return VOID;
		else if(strcmp(strlwr(name),Tab[i].name)==0 && Tab[i].type == INT)//表示是无返回值的函数
			return INT;
		else if(strcmp(strlwr(name),Tab[i].name)==0 && Tab[i].type == CHAR)//表示是无返回值的函数
			return CHAR;
	}
}
/*
1.局部和全局可以重名
2.函数名和内部变量名不可以重复
*/
int findDef(char name[])
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(stricmp(name,Tab[i].name)==0)//表示找到重名的了
			return i;
	}
	return -1; //没找到同名的
}
void findResult(int i_findResult)	//找全局常量变量或函数名重名
{
	int i;
	i = findDef(Str[i_findResult].name);
	if(i>=0)//表示在全局常量变量或函数名重名,他们不能与任一元素同名
	{
		fprintf(ErrorOut,"第%d行与%s有重名\n",Str[i_findResult].OriLine,Str[i_findResult].name);
	}
}
void findTmpSame(int i_findTmpSame)
{
	int i = 0,n=0,g=0;
	for(g=0;g<TabNum;g++)
	{
		if(Tab[g].obj == FUNC && strcmp(Tab[g].name,Str[i_findTmpSame].name)==0)
			fprintf(ErrorOut,"第%d行与函数名%s重名\n",Str[i_findTmpSame].OriLine,Str[i_findTmpSame].name);
	}
	while(Tab[n].lev != Lev)
		n++;
	if(Tab[n].lev == Lev)	//现在到达了当前层次
	{
		for(i = n+1;Tab[i].lev == Lev && i<TabNum; i++)	//从当前层次的函数往后开始找看看有没有重复定义
		{
			if(strcmp(Str[i_findTmpSame].name,Tab[i].name)==0)//表示在该层次找到重名的了
			{
				fprintf(ErrorOut,"第%d行与%s有重名\n",Str[i_findTmpSame].OriLine,Str[i_findTmpSame].name);
				break;
			}
		}
	}
}
//找到数组长度，判断访问越界问题
int findArrLen(int id)
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(strcmp(Str[id].name,Tab[i].name)==0 && (Tab[i].obj == TARR|| Tab[i].obj == GARR))//表示找到重名的了
			return Tab[i].value;
	}
	return -1; //没找到这个数组
}
//为跳转添加标签，形如label_1  label_2  label_3
void LableIntoMTab()  
{
	char name[100]="label_";
	char LabelTmp[6];
	itoa(LabelNum,LabelTmp,10);//数字转字符串存在LabelTmp里面
	strcat(name,LabelTmp);//把标签下标和标签名拼接起来
	strcpy(LabelNumber,name);//标签名存在LabelNumber字符串里面
	fprintf(InterCodeStream,"%s\n",LabelNumber);
	LabelNum++;
}
//为switch的case语句添加标签
void AddCaseLabel(int i)
{
	char name[100]="labelcon_";//把每一个case加上label
	char LabelTmp[6],NegTmp[1024]="-",FinInt[1024]={'\0'};
	if(strcmp(Str[index-2].kind,"Minus")==0)	//如果是负数，在前面加上负号
	{
		strcat(NegTmp,Str[index-1].name);
		strcpy(FinInt,NegTmp);
	}
	else
		strcpy(FinInt,Str[index-1].name);	//否则，直接复制到FinInt
	itoa(SwitLabNum,LabelTmp,10);
	strcat(name,LabelTmp);
	strcpy(CaseNumber,name);
	fprintf(InterCodeStream,"%s\n",CaseNumber);
	InsTypeName1Res(ThisLabel,Label,CaseNumber,FinInt);
	SwitLabNum++;
}
//生成临时的变量，形如tmp1  tmp2  tmp3
void makeVar(char name[])
{
	char tmp[15]="tmp";
	char VarTmp[6];
	itoa(VarNum,VarTmp,10);//数字转字符串
	strcat(tmp,VarTmp);
	strcpy(name,tmp);
	VarNum++;
}
//生成End1518_1  end1518_2
void makeEndLabel()
{
	char EndTmp[6];
	strcpy(EndId,"end1518_");
	itoa(EndNum,EndTmp,10);//数字转字符串
	strcat(EndId,EndTmp);
	EndNum++;
}
//生成switch的id,用于嵌套switch时的配对
void MakeSwitchEndLabel()
{
	char EndTmp[6];
	strcpy(SwitBEid,"switend_");
	itoa(SwitEndNum,EndTmp,10);
	strcat(SwitBEid,EndTmp);
	SwitEndNum++;
}