#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WordAnalysis.h"
#include "grammar.h"
#include "ErrorHandling.h"
#include  "KindsOfTabs.h"
#include "IntermediateCode.h"
#include "TargetCode.h"
#define	FunOff	1024

FILE * TargetCodeStream = NULL;//输出文件
FILE * TargetInStream = NULL;//读中间代码文件

int strFlag=0;
int FindStrId=0;
int LineAdd=0;
int RunPosi;
int RunIndex=0;
int switId=0;	//switch的判断条件的个数
int switCaseId=0;
char StrIdRes[100]="Str";//Prin语句中找Str下标的结果，用于输出
char *PushReg[100] = {"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7"};
int para_index=0;
int PushId=0;
/*读当前的符号表，给函数名那一行加上他的参数个数*/
void AddParaNumToFun()
{
	int m=0,funid=0;
	for(m=0;m<TabNum;m++)
	{
		if(Tab[m].obj==FUNC)
		{
			funid=m;
			m++;
			while(Tab[m].obj==PARA)
			{
				Tab[funid].AllParaNum++;
				m++;
			}
			m=funid;
		}
	}
}
/*全局部分以及main函数内的常量变量存入结构体数组RunStack*/
void IntoRunStack()
{
	int GvarId=0;
	for(GvarId=0;GvarId<TabNum;GvarId++)
	{
		if(Tab[GvarId].obj==GVAR || Tab[GvarId].obj==GARR || Tab[GvarId].obj==GCON)//全局部分存入运行栈
		{
			strcpy(RunStack[RunIndex].name,Tab[GvarId].name);
			RunStack[RunIndex].RunPosi=Tab[GvarId].adrFirst;//起始地址存进去
			RunStack[RunIndex].RunType = Tab[GvarId].type;
			RunStack[RunIndex].RunObj = Tab[GvarId].obj;
			RunIndex++;
		}
		else if(Tab[GvarId].obj==FUNC && strcmp(Tab[GvarId].name,"main")!=0)
			continue;
		else if(Tab[GvarId].obj==FUNC && strcmp(Tab[GvarId].name,"main")==0)
			break;
	}
	if(Tab[GvarId].obj==FUNC && strcmp(Tab[GvarId].name,"main")==0)
	{
		for( ; GvarId<TabNum;GvarId++)
		{
			strcpy(RunStack[RunIndex].name,Tab[GvarId].name);
			RunStack[RunIndex].RunPosi=Tab[GvarId].adrFirst;//起始地址存进去
			RunStack[RunIndex].RunType = Tab[GvarId].type;
			RunStack[RunIndex].RunObj = Tab[GvarId].obj;
			RunIndex++;
		}
	}
	RunPosi = RunStack[RunIndex-1].RunPosi;	
}
/*读中间代码，生成目标代码*/
void CalInStack()
{
	int Code_i=1;
	TargetCodeStream = fopen("TargetCode.txt","w");
	StrDef();
	fprintf(TargetCodeStream,"LineBreaks:\t.asciiz\t\"\\n\" \n");//换行
	fprintf(TargetCodeStream,".text\n");//换行
	for(Code_i=1;Code_i<CodeNum;Code_i++)
	{
		if(MediaTab[Code_i].ins == Gcon || MediaTab[Code_i].ins == Gvar || MediaTab[Code_i].ins == Garr)
			DataDef(Code_i);	
		/*不是main函数*/
		else if(MediaTab[Code_i].ins == Func && strcmp(MediaTab[Code_i].name1,"main")!=0)
		{
			fprintf(TargetCodeStream,".text\nj main\n%s:\n",MediaTab[Code_i].name1);//在mars输出这个函数，全部转小写
			fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",-1*MediaTab[Code_i].FunLen);	//进入函数时先减去他的空间
			fprintf(TargetCodeStream,"sw\t$ra\t%d($sp)\n",MediaTab[Code_i].FunLen);
			while(MediaTab[Code_i].ins != End)
			{
				Code_i++;
				if(MediaTab[Code_i].ins == Tcon)//临时常量
					TmpDataDef(Code_i,NotMain);
				else if(MediaTab[Code_i].ins == Tvar || MediaTab[Code_i].ins == Tarr ||MediaTab[Code_i].ins == Para)
					continue;
				/*比较运算定义*/
				else if(MediaTab[Code_i].ins == Grea || MediaTab[Code_i].ins == GaE ||MediaTab[Code_i].ins == Less ||
					    MediaTab[Code_i].ins == LaE || MediaTab[Code_i].ins == NaE ||MediaTab[Code_i].ins == Equ)  
					CompareOp(Code_i);
				/*为正跳转*/
				else if(MediaTab[Code_i].ins == TGoto)	//跳转到标签，标签不用转小写
					fprintf(TargetCodeStream,"beq\t$t2\t1\t%s\n",MediaTab[Code_i].result);//若name1等于name2则$t2设为1,$t2统一存放比较结果
				/*不满足条件则跳转：如if*/
				else if(MediaTab[Code_i].ins == FGoto)	//跳转到标签，标签不用转小写
					fprintf(TargetCodeStream,"bne\t$t2\t1\t%s\n",MediaTab[Code_i].result);//若name1等于name2则$t2设为1,$t2统一存放比较结果
				/*直接跳转*/
				else if(MediaTab[Code_i].ins == Goto)
					fprintf(TargetCodeStream,"j\t%s\n",MediaTab[Code_i].name1);//若name1等于name2则$t2设为1,$t2统一存放比较结果
				/*标签定义*/
				else if(MediaTab[Code_i].ins == ThisLabel && MediaTab[Code_i].type == Label)
					fprintf(TargetCodeStream,"%s:\n",MediaTab[Code_i].name1);
				/*函数调用开头*/
				else if(MediaTab[Code_i].ins == CallBegin)
					ParaIntoStack(Code_i);
				/*调用有参数函数定义*/
				else if(MediaTab[Code_i].ins == Push)
					Code_i = CallParaFun(Code_i);
				/*调用无参数函数定义*/
				else if(MediaTab[Code_i].ins == CallEnd)
					CallNoParaFun(Code_i);
				/*写语句定义*/
				else if(MediaTab[Code_i].ins == Prin)
					PrinStrs(Code_i);
				/*读语句定义*/
				else if(MediaTab[Code_i].ins == Scan)
					ScanIdent(Code_i);
				/*加减乘除、取负定义*/
				else if(MediaTab[Code_i].ins == Add ||MediaTab[Code_i].ins == Min ||MediaTab[Code_i].ins == Mul ||
						MediaTab[Code_i].ins == Div ||MediaTab[Code_i].ins == Neg)
					Arithmetic(Code_i);
				/*赋值语句定义*/
				else if(MediaTab[Code_i].ins == Ass)
					AssDef(Code_i);//表示不是main函数
				/*返回语句定义*/
				else if(MediaTab[Code_i].ins == Ret)
					RetDef(Code_i);
				/*无返回值的返回语句*/
				else if(MediaTab[Code_i].ins == NRet)
					NRetDef(Code_i);
				/*结束语句定义*/
				else if(MediaTab[Code_i].ins == End)
					ReEndDef(Code_i);
				/*赋返回值语句定义*/
				else if(MediaTab[Code_i].ins == RetS)
					RetsDef(Code_i);
				/*向数组赋值定义*/
				else if(MediaTab[Code_i].ins == ArrA)
					ArrADef(Code_i);
				/*从数组取值定义*/
				else if(MediaTab[Code_i].ins == ArrG)
					ArrGDef(Code_i);
				/*情况语句定义*/
				else if(MediaTab[Code_i].ins == Swit)
					SwitDef(Code_i);
			}
		}
		else if(MediaTab[Code_i].ins == Func && strcmp(MediaTab[Code_i].name1,"main")==0)//是main函数
		{
			fprintf(TargetCodeStream,".text\nmain:\n");
			while(MediaTab[Code_i].ins != End)
			{
				Code_i++;
				if(MediaTab[Code_i].ins == Tcon)
					TmpDataDef(Code_i,JustInMain);
				else if(MediaTab[Code_i].ins == Tvar || MediaTab[Code_i].ins == Tarr)
					continue;
				/*比较运算定义*/
				else if(MediaTab[Code_i].ins == Grea || MediaTab[Code_i].ins == GaE ||MediaTab[Code_i].ins == Less ||
					    MediaTab[Code_i].ins == LaE || MediaTab[Code_i].ins == NaE ||MediaTab[Code_i].ins == Equ)
					CompareOp(Code_i);
				/*为正跳转*/
				else if(MediaTab[Code_i].ins == TGoto)
					fprintf(TargetCodeStream,"beq\t$t2\t1\t%s\n",MediaTab[Code_i].result);//若name1等于name2则$t2设为1,$t2统一存放比较结果
				/*不满足条件则跳转：如if*/
				else if(MediaTab[Code_i].ins == FGoto)
					fprintf(TargetCodeStream,"bne\t$t2\t1\t%s\n",MediaTab[Code_i].result);//若name1等于name2则$t2设为1,$t2统一存放比较结果
				/*直接跳转*/
				else if(MediaTab[Code_i].ins == Goto)
					fprintf(TargetCodeStream,"j\t%s\n",MediaTab[Code_i].name1);//若name1等于name2则$t2设为1,$t2统一存放比较结果
				/*标签定义*/
				else if(MediaTab[Code_i].ins == ThisLabel && MediaTab[Code_i].type == Label)
					fprintf(TargetCodeStream,"%s:\n",MediaTab[Code_i].name1);
				/*函数调用开头*/
				else if(MediaTab[Code_i].ins == CallBegin)
					ParaIntoStack(Code_i);
				/*调用有参数函数定义*/
				else if(MediaTab[Code_i].ins == Push)
					Code_i = CallParaFun(Code_i);
				/*调用无参数函数定义*/
				else if(MediaTab[Code_i].ins == CallEnd)
					CallNoParaFun(Code_i);
				/*写语句定义*/
				else if(MediaTab[Code_i].ins == Prin)
					PrinStrs(Code_i);
				/*读语句定义*/
				else if(MediaTab[Code_i].ins == Scan)
					ScanIdent(Code_i);
				/*加减乘除、取负定义*/
				else if(MediaTab[Code_i].ins == Add ||MediaTab[Code_i].ins == Min ||MediaTab[Code_i].ins == Mul ||
						MediaTab[Code_i].ins == Div ||MediaTab[Code_i].ins == Neg)
					Arithmetic(Code_i);
				/*赋值语句定义*/
				else if(MediaTab[Code_i].ins == Ass)
					AssDef(Code_i);//表示是main函数
				/*无返回值的返回语句*/
				else if(MediaTab[Code_i].ins == NRet)	//main里面有return;
					NRetDef(Code_i);
				/*结束语句定义*/
				else if(MediaTab[Code_i].ins == End)	//给main添加end标签，方便遇到return;的时候跳转
					MainEndDef(Code_i);
				/*赋返回值语句定义*/
				else if(MediaTab[Code_i].ins == RetS)
					RetsDef(Code_i);
				/*向数组赋值定义*/
				else if(MediaTab[Code_i].ins == ArrA)
					ArrADef(Code_i);
				/*从数组取值定义*/
				else if(MediaTab[Code_i].ins == ArrG)
					ArrGDef(Code_i);
				/*情况语句定义*/
				else if(MediaTab[Code_i].ins == Swit)
					SwitDef(Code_i);
			}
			fprintf(TargetCodeStream,"li\t$v0\t10\nsyscall\n");	//程序结束
		}
	}
}
/*store data*/
void StoreData(int i_StoreData,char Reg[5],LoadWhat S_StoreData)
{
	JudgeMain c;
	char CurIdent[100];
	//保存当前待判断的标识符在CurIdent
	if(S_StoreData == Name1)
		strcpy(CurIdent,MediaTab[i_StoreData].name1);	
	else if(S_StoreData == Name2)
		strcpy(CurIdent,MediaTab[i_StoreData].name2);	
	else if(S_StoreData == Result)
		strcpy(CurIdent,MediaTab[i_StoreData].result);	
	c = FindIdentLoc(i_StoreData,S_StoreData);	//notmain代表在非main函数内
	if(c == NotMain)	//非main函数内
		fprintf(TargetCodeStream,"sw\t%s\t%d($sp)\n",Reg,FindTmpIdentAdr(i_StoreData,S_StoreData,Radfirst));
	else if(c == IsMain)	//全局
		fprintf(TargetCodeStream,"sw\t%s\t%d($gp)\n",Reg,FindAdr(i_StoreData,S_StoreData,Radfirst));//name1	全局
	else if(c == JustInMain)	//main函数内
		fprintf(TargetCodeStream,"sw\t%s\t%d($gp)\n",Reg,FindTmpIdentAdr(i_StoreData,S_StoreData,Radfirst));//name1	main函数内的赋值语句
}
/*load data*/	//此处的name[]要注明是name1还是name2还是result
void LoadData(int i_LoadData,char Reg[5],LoadWhat l_LoadData,char name[])
{
	JudgeMain c;
	if(isDigit(name[0])!=-1||name[0]=='+'||name[0]=='-')//是整数
			fprintf(TargetCodeStream,"li\t%s\t%d\n",Reg,(atoi)(name));//name1
	else if(name[0]=='\'')//是字符
			fprintf(TargetCodeStream,"li\t%s\t%d\n",Reg,name[1]);//name1
	else
	{
		c = FindIdentLoc(i_LoadData,l_LoadData);	/*判断标识符是全局还是局部*/
		if(c == IsMain)	//全局
			fprintf(TargetCodeStream,"lw\t%s\t%d($gp)\n",Reg,FindAdr(i_LoadData,l_LoadData,Radfirst));
		else if(c == JustInMain) //main函数的部分
			fprintf(TargetCodeStream,"lw\t%s\t%d($gp)\n",Reg,FindTmpIdentAdr(i_LoadData,l_LoadData,Radfirst));
		else if(c == NotMain)	//非main函数内
			fprintf(TargetCodeStream,"lw\t%s\t%d($sp)\n",Reg,FindTmpIdentAdr(i_LoadData,l_LoadData,Radfirst));
	}
}
/*全局数据段.data的定义*/	//已经解决  全部存小写
void DataDef(int i_DataDef)
{
	if(MediaTab[i_DataDef].ins == Gcon)	//全局常量
	{
		if(MediaTab[i_DataDef].type == Int)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",atoi(MediaTab[i_DataDef].result));
		else if(MediaTab[i_DataDef].type == Char)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",MediaTab[i_DataDef].result[1]);
		fprintf(TargetCodeStream,"sw\t$t0\t%d($gp)\n",FindAdr(i_DataDef,Name1,Radfirst));
	}
}
///*临时常量定义*/	//已经解决
void TmpDataDef(int i_TmpDataDef,JudgeMain ifMain)
{
	fprintf(TargetCodeStream,"#临时常量定义\n"); //数据段
	if(MediaTab[i_TmpDataDef].ins == Tcon)//临时常量定义
	{
		if(MediaTab[i_TmpDataDef].type == Int)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",atoi(MediaTab[i_TmpDataDef].result));
		else if(MediaTab[i_TmpDataDef].type == Char)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",MediaTab[i_TmpDataDef].result[1]);
	}
	if(ifMain == NotMain)	//非main函数内
		fprintf(TargetCodeStream,"sw\t$t0\t%d($sp)\n",FindTmpIdentAdr(i_TmpDataDef,Name1,Radfirst));
	else if(ifMain == JustInMain)	//main函数内
		fprintf(TargetCodeStream,"sw\t$t0\t%d($gp)\n",FindAdr(i_TmpDataDef,Name1,Radfirst));
}
/*字符串.data段定义*/
void StrDef()
{
	int strIndex=0,StrId=0;
	char Strname[100]="Str";
	char StrTmp[3];
	//字符串定义
	if(strFlag==0)
	{
		fprintf(TargetCodeStream,"#数据段\n"); //数据段
		fprintf(TargetCodeStream,".data\n"); //数据段
		for(strIndex=0;strIndex<count;strIndex++)
		{
			if(strcmp(Str[strIndex].kind,"StrSym")==0)  //把字符串挑出来
			{
				itoa(StrId,StrTmp,10);//数字转字符串存在StrTmp里面
				strcat(Strname,StrTmp);//把标签下标和标签名拼接起来
				fprintf(TargetCodeStream,"%s:\t.asciiz\t%s\n",Strname,Str[strIndex].name);
				strcpy(Strname,"Str");
				StrId++;
			}
		}
		strFlag=1;
	}
}
/*赋值语句*/
void AssDef(int i_AssDef)	//Ass j tmp5
{
	fprintf(TargetCodeStream,"#赋值语句\n");
	LoadData(i_AssDef,"$t0",Result,MediaTab[i_AssDef].result);	//在loaddata里面有转小写
	StoreData(i_AssDef,"$t0",Name1);	//在storedata里面有转小写
}
/*有返回值的返回语句*/
void RetDef(int i_RetDef)
{
	int m=i_RetDef;
	while(MediaTab[m].ins!=End)
		m++;
	fprintf(TargetCodeStream,"#返回语句\n");
	LoadData(i_RetDef,"$t0",Name1,MediaTab[i_RetDef].name1);
	fprintf(TargetCodeStream,"addi\t$v0\t$t0\t0\n");//返回值存在$v0
	if(MediaTab[m].ins==End)
		fprintf(TargetCodeStream,"j\t%s\n",MediaTab[m].name1);//endid是name1
}
/*无返回值的返回语句*/
void NRetDef(int i_NRetDef)
{
	int m=i_NRetDef;
	while(MediaTab[m].ins!=End)
		m++;
	fprintf(TargetCodeStream,"#返回语句\n");
	if(MediaTab[m].ins==End)
		fprintf(TargetCodeStream,"j\t%s\n",MediaTab[m].name1);//endid是name1
}
/*结束语句*/	//End End1518_0 fun
void ReEndDef(int i_EndDef)
{
	int i=0,funlength=0;
	fprintf(TargetCodeStream,"#结束语句\n");
	fprintf(TargetCodeStream,"%s:\n",MediaTab[i_EndDef].name1);//输出End1518_0
	//fprintf(TargetCodeStream,"addi\t$v0\t$v0\t0\n");//算出$v0
	for(i=i_EndDef;i>=0;i--)	//往回倒，去找此函数的声明，那里有他的长度
	{
		if(MediaTab[i].ins == Func && strcmp(MediaTab[i].name1,MediaTab[i_EndDef].result)==0)
		{
			funlength = MediaTab[i].FunLen;
			break;
		}
	}
	fprintf(TargetCodeStream,"#ra测试\nlw\t$ra\t%d($sp)\n",funlength);//先取出ra再取sp
	fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",funlength);//取出sp
	fprintf(TargetCodeStream,"jr\t$ra\n");
}
/*main函数结束语句，不用加载返回值，直接加end标签就好*/
void MainEndDef(int i_MainEndDef)
{
	fprintf(TargetCodeStream,"#main结束语句\n");
	fprintf(TargetCodeStream,"%s:\n",MediaTab[i_MainEndDef].name1);//输出End1518_0
}
/*赋返回值语句*/
void RetsDef(int i_RetsDef)	//RetS tmp6	取出返回值v0
{
	fprintf(TargetCodeStream,"#赋返回值语句\n");
	StoreData(i_RetsDef,"$v0",Name1);
}
/*向数组赋值定义*/
void ArrADef(int i_ArrADef)
{
	JudgeMain c;
	fprintf(TargetCodeStream,"#向数组赋值\n");
	c = FindIdentLoc(i_ArrADef,Name1);	/*判断数组是全局还是局部*/
	if(c == NotMain && FindTmpIdentAdr(i_ArrADef,Name1,Robj) == TARR)	//非main函数内
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindTmpIdentAdr(i_ArrADef,Name1,Radfirst));	//t0存放数组基地址
		LoadData(i_ArrADef,"$t1",Name2,MediaTab[i_ArrADef].name2);	//数组下标存在$t1
		LoadData(i_ArrADef,"$t2",Result,MediaTab[i_ArrADef].result);	//要存入数组的值存在$t2
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//数组下标乘以4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//真正相对sp的偏移存在t0
		fprintf(TargetCodeStream,"addu\t$sp\t$sp\t$t0\n");	//再把偏移的sp加回来
		fprintf(TargetCodeStream,"sw\t$t2\t0($sp)\n");	
		fprintf(TargetCodeStream,"subu\t$sp\t$sp\t$t0\n");	
	}
	else if( (c == IsMain && FindAdr(i_ArrADef,Name1,Robj)==GARR) || 
		     (c==JustInMain && FindTmpIdentAdr(i_ArrADef,Name1,Robj)==TARR) )	//全局或main函数内
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindAdr(i_ArrADef,Name1,Radfirst));//name1	
		LoadData(i_ArrADef,"$t1",Name2,MediaTab[i_ArrADef].name2);	//数组下标存在$t1
		LoadData(i_ArrADef,"$t2",Result,MediaTab[i_ArrADef].result);	//要存入数组的值存在$t2
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//数组下标乘以4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//真正相对sp的偏移存在t0
		fprintf(TargetCodeStream,"addu\t$gp\t$gp\t$t0\n");	//再把偏移的sp加回来
		fprintf(TargetCodeStream,"sw\t$t2\t0($gp)\n");	
		fprintf(TargetCodeStream,"subu\t$gp\t$gp\t$t0\n");	
	}
}
/*从数组取值定义*/
void ArrGDef(int i_ArrGDef)
{
	JudgeMain c;
	fprintf(TargetCodeStream,"#从数组取值\n");
	c = FindIdentLoc(i_ArrGDef,Name1);	/*判断数组是全局还是局部*/
	if(c == NotMain && FindTmpIdentAdr(i_ArrGDef,Name1,Robj) == TARR)	//非main函数内的局部数组
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindTmpIdentAdr(i_ArrGDef,Name1,Radfirst));
		LoadData(i_ArrGDef,"$t1",Name2,MediaTab[i_ArrGDef].name2);	//数组下标存在$t1
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//数组下标乘以4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//真正相对fp的偏移存在t0
		fprintf(TargetCodeStream,"addu\t$sp\t$sp\t$t0\n");	//再把偏移的sp加回来
		fprintf(TargetCodeStream,"lw\t$t2\t0($sp)\n");	//从数组取值存在$t2
		fprintf(TargetCodeStream,"subu\t$sp\t$sp\t$t0\n");	//求出地址
		StoreData(i_ArrGDef,"$t2",Result);
	}
	else if((c == IsMain && FindAdr(i_ArrGDef,Name1,Robj)==GARR) ||		//全局数组
			(c==JustInMain && FindTmpIdentAdr(i_ArrGDef,Name1,Robj)==TARR) )	//main内的局部数组
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindAdr(i_ArrGDef,Name1,Radfirst));//name1	
		LoadData(i_ArrGDef,"$t1",Name2,MediaTab[i_ArrGDef].name2);	//数组下标存在$t1
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//数组下标乘以4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//真正相对sp的偏移存在t0
		fprintf(TargetCodeStream,"addu\t$gp\t$gp\t$t0\n");	//再把偏移的sp加回来
		fprintf(TargetCodeStream,"lw\t$t2\t0($gp)\n");	
		fprintf(TargetCodeStream,"subu\t$gp\t$gp\t$t0\n");	
		StoreData(i_ArrGDef,"$t2",Result);
	}
}
/*情况语句定义*/
int SwitDef(int i_SwitDef)
{
	int tmp;
	LoadData(i_SwitDef,"$t0",Name1,MediaTab[i_SwitDef].name1);	//取出表达式的值存到$t0
	tmp = WhichCase(i_SwitDef);	//把case语句后的常量以及各自的标签录入SwitCase结构体数组
	return tmp;
}
/*写语句*/
void PrinStrs(int i_PrinStr)
{
	JudgeMain c;
	if(MediaTab[i_PrinStr].name1[0]=='\"')	
	{
		fprintf(TargetCodeStream,"#输出字符串\n");
		SpliceStrId(MediaTab[i_PrinStr].name1);
		fprintf(TargetCodeStream,"li\t$v0\t4\n");	//字符串是4
		fprintf(TargetCodeStream,"la\t$a0\t%s\n",StrIdRes);
		fprintf(TargetCodeStream,"syscall\n");
		strcpy(StrIdRes,"Str");//最后把StrIdRes设置为"Str"
	}
	else if(isDigit(MediaTab[i_PrinStr].name1[0])!=-1||MediaTab[i_PrinStr].name1[0]=='+'||MediaTab[i_PrinStr].name1[0]=='-')	//是整数
	{
		fprintf(TargetCodeStream,"#输出整数\n");
		fprintf(TargetCodeStream,"li\t$t0\t%d\n",(atoi)(MediaTab[i_PrinStr].name1));
		fprintf(TargetCodeStream,"li\t$v0\t1\n");	//整数是1
		fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
		fprintf(TargetCodeStream,"syscall\n");
	}
	else if(MediaTab[i_PrinStr].name1[0]=='\'')		//是字符
	{
		fprintf(TargetCodeStream,"#输出字符\n");
		fprintf(TargetCodeStream,"li\t$t0\t%d\n",MediaTab[i_PrinStr].name1[1]);
		fprintf(TargetCodeStream,"li\t$v0\t11\n");	//字符是11
		fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
		fprintf(TargetCodeStream,"syscall\n");
	}
	else	//标识符
	{
		fprintf(TargetCodeStream,"#输出标识符\n");
		LoadData(i_PrinStr,"$t0",Name1,MediaTab[i_PrinStr].name1);
		c = FindIdentLoc(i_PrinStr,Name1);	/*判断标识符是全局还是局部*/
		if( ((c == NotMain || c ==JustInMain) && FindTmpIdentAdr(i_PrinStr,Name1,Rtype) == CHAR)||	//main或非main函数内
			( c == IsMain  && FindAdr(i_PrinStr,Name1,Rtype)==CHAR) )	//全局
		{
			fprintf(TargetCodeStream,"li\t$v0\t11\n");	//字符是11
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
		else if( ((c == NotMain || c ==JustInMain) && FindTmpIdentAdr(i_PrinStr,Name1,Rtype) == INT)||	//main或非main函数内
			(c == IsMain && FindAdr(i_PrinStr,Name1,Rtype)==INT))	//全局
		{
			fprintf(TargetCodeStream,"li\t$v0\t1\n");	//整数是1
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
		else if(MediaTab[i_PrinStr].IsCharFlag == 1 )
		{
			fprintf(TargetCodeStream,"li\t$v0\t11\n");	//字符是11
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
		else
		{
			fprintf(TargetCodeStream,"li\t$v0\t1\n");	//整数是1
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
	}
	//最后换行
	fprintf(TargetCodeStream,"#换行\n");
	fprintf(TargetCodeStream,"li\t$v0\t4\n");
	fprintf(TargetCodeStream,"la\t$a0\tLineBreaks\n");
	fprintf(TargetCodeStream,"syscall\n");
}
/*读语句*/
void ScanIdent(int i_ScanIdent)
{
	JudgeMain c;
	c = FindIdentLoc(i_ScanIdent,Name1);	/*判断标识符是全局还是局部*/
	if( ( (c == NotMain || c == JustInMain) && FindTmpIdentAdr(i_ScanIdent,Name1,Rtype) == INT)||
		  (c == IsMain && FindAdr(i_ScanIdent,Name1,Rtype)==INT) )
		fprintf(TargetCodeStream,"#读入整数\nli\t$v0\t5\n");
	else if( ((c == NotMain || c == JustInMain) && FindTmpIdentAdr(i_ScanIdent,Name1,Rtype) == CHAR)||
		(c == IsMain && FindAdr(i_ScanIdent,Name1,Rtype)==CHAR) )
		fprintf(TargetCodeStream,"#读入字符\nli\t$v0\t12\n");
	fprintf(TargetCodeStream,"syscall\n");
	StoreData(i_ScanIdent,"$v0",Name1);
}
/*比较运算*/
void CompareOp(int i_CompareOp)
{
	fprintf(TargetCodeStream,"#比较\n");
	LoadData(i_CompareOp,"$t0",Name1,MediaTab[i_CompareOp].name1);
	LoadData(i_CompareOp,"$t1",Name2,MediaTab[i_CompareOp].name2);
	if(MediaTab[i_CompareOp].ins == Grea)
		fprintf(TargetCodeStream,"slt\t$t2\t$t1\t$t0\n");//若name1大于name2则$t2设为1
	else if(MediaTab[i_CompareOp].ins == GaE)
		fprintf(TargetCodeStream,"sge\t$t2\t$t0\t$t1\n");//若name1大于等于name2则$t2设为1
	else if(MediaTab[i_CompareOp].ins == Less)
		fprintf(TargetCodeStream,"slt\t$t2\t$t0\t$t1\n");//若name1小于name2则$t2设为1
	else if(MediaTab[i_CompareOp].ins == LaE)
		fprintf(TargetCodeStream,"sle\t$t2\t$t0\t$t1\n");//若name1小于等于name2则$t2设为1
	else if(MediaTab[i_CompareOp].ins == NaE)
		fprintf(TargetCodeStream,"sne\t$t2\t$t0\t$t1\n");//若name1不等于name2则$t2设为1
	else if(MediaTab[i_CompareOp].ins == Equ)
		fprintf(TargetCodeStream,"seq\t$t2\t$t0\t$t1\n");//若name1等于name2则$t2设为1
}
/*加减乘除*/
void Arithmetic(int i_Arithmetic)
{
	fprintf(TargetCodeStream,"#加减乘除\n");
	LoadData(i_Arithmetic,"$t0",Name1,MediaTab[i_Arithmetic].name1);
	if(MediaTab[i_Arithmetic].ins != Neg)	//因为neg没有name2操作数
		LoadData(i_Arithmetic,"$t1",Name2,MediaTab[i_Arithmetic].name2);
	if(MediaTab[i_Arithmetic].ins == Add)	//Add b c tmp5
		fprintf(TargetCodeStream,"add\t$t2\t$t0\t$t1\n");
	else if(MediaTab[i_Arithmetic].ins == Min)	//Min tmp5 tmp6 tmp7
		fprintf(TargetCodeStream,"sub\t$t2\t$t0\t$t1\n");
	else if(MediaTab[i_Arithmetic].ins == Mul)	//Mul d e tmp6
		fprintf(TargetCodeStream,"mul\t$t2\t$t0\t$t1\n");
	else if(MediaTab[i_Arithmetic].ins == Div)
		fprintf(TargetCodeStream,"div\t$t2\t$t0\t$t1\n");
	else if(MediaTab[i_Arithmetic].ins == Neg)	//Neg -5 tmp3
		fprintf(TargetCodeStream,"neg\t$t2\t$t0\n");	//$t0存放的name1
	StoreData(i_Arithmetic,"$t2",Result);
}
/*调用函数定义,push*/	//Push m PARA1 gcd 
int CallParaFun(int i_CallFun)
{
	int funlength=0;
	int Cur=i_CallFun;
	while(MediaTab[Cur].ins!=CallEnd)
	{
		if(MediaTab[Cur].ins!=CallBegin && MediaTab[Cur].ins!=Push)
			return (Cur-1);
		ParaIntoStack(Cur);
		Cur++;
	}
	if(MediaTab[Cur].ins==CallEnd)
	{
		while(strcmp(PushStack[PushId-1].con,"CallBegin")!=0)
		{
			LoadData(PushStack[PushId-1].FourId,"$t0",Name1,PushStack[PushId-1].con);	//Name1  is para	取出参数的值存在$t0
			fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",-1*FindFunLen(PushStack[PushId-1].FourId));
			fprintf(TargetCodeStream,"sw\t$t0\t%d($sp)\n",FindParaAdr(PushStack[PushId-1].FourId,FindParaIndex(PushStack[PushId-1].FourId) ));//寻找name1的地址
			fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",FindFunLen(PushStack[PushId-1].FourId));
			ParaOutStack();
		}
		ParaOutStack();
	}
	return (Cur-1);
}
/*参数入栈*/
void ParaIntoStack(int i_ParaIntoStack)
{
	if(MediaTab[i_ParaIntoStack].ins!=CallBegin)
	{
		strcpy(PushStack[PushId].con,MediaTab[i_ParaIntoStack].name1);	//常量存入栈
		PushStack[PushId].ParaPosi=FindParaIndex(i_ParaIntoStack);	//参数下标入栈
		strcpy(PushStack[PushId].funName,MediaTab[i_ParaIntoStack].result);//所调用的函数名字入栈
		PushStack[PushId].FourId = i_ParaIntoStack;	//他在四元式的位置记下来
		//printf("入栈：常量\t%s\t参数下标\t%d调用函数\t%s在四元式的位置%d\n",PushStack[PushId].con,PushStack[PushId].ParaPosi,PushStack[PushId].funName,PushStack[PushId].FourId);
	}
	else
		strcpy(PushStack[PushId].con,"CallBegin");
	PushId++;
}
/*参数出栈*/
void ParaOutStack()
{
	//printf("出栈：常量\t%s\t参数下标\t%d调用函数\t%s在四元式的位置%d\n",PushStack[PushId-1].con,PushStack[PushId-1].ParaPosi,PushStack[PushId-1].funName,PushStack[PushId-1].FourId);
	strcpy(PushStack[PushId-1].con,"\0");	//常量出栈
	PushStack[PushId-1].ParaPosi=-1;	//参数下标出栈
	strcpy(PushStack[PushId-1].funName,"\0");//所调用的函数名字出栈
	PushStack[PushId-1].FourId = -1;	//他在四元是式的位置设为-1
	PushId--;
}
/*调用无参数函数定义*/	//callend
void CallNoParaFun(int i_CallNoParaFun)
{
	fprintf(TargetCodeStream,"jal\t%s\n",MediaTab[i_CallNoParaFun].name1);
}
/*输出字符串时寻找相应数据段的Str下标*/
void FindStrLabel(char CurStr[])
{
	int m=0;
	for(m=0;m<count;m++)
	{
		if(strcmp(Str[m].kind,"StrSym")==0)   //把字符串挑出来
		{
			if(strcmp(CurStr,Str[m].name)!=0)
			{
				FindStrId++;
				continue;
			}
			else
				break;
		}
	}
}
/*根据Str的下标拼接出Str的标签*/
void SpliceStrId(char CurStr[])
{
	char FindStrIdTmp[3];
	FindStrLabel(CurStr);
	itoa(FindStrId,FindStrIdTmp,10);//数字转字符串存在LabelTmp里面
	FindStrId = 0;
	strcat(StrIdRes,FindStrIdTmp);//把标签下标和标签名拼接起来
}
/*在结构体数组RunStack中寻找全局的地址或类型*/
int FindAdr(int id_findadr,LoadWhat which,ReturnWhat Rwhat)
{
	int Id=0;
	for(Id=0;Id<RunIndex;Id++)
	{
		if( strcmp(RunStack[Id].name,MediaTab[id_findadr].name1)==0 && which==Name1
			|| strcmp(RunStack[Id].name,MediaTab[id_findadr].name2)==0 && which==Name2
			|| strcmp(RunStack[Id].name,MediaTab[id_findadr].result)==0 && which==Result )
			if(Rwhat == Radfirst)
				return RunStack[Id].RunPosi;
			else if(Rwhat == Rtype)
				return RunStack[Id].RunType;
			else if(Rwhat == Robj)
				return RunStack[Id].RunObj;
			else if(Rwhat == Rischar)
				return RunStack[Id].GloIschar;
	}
}
/*在符号表中寻找临时常量、变量、数组的 地址，这样做是因为可能重名*/
int FindTmpIdentAdr(int i_FindTmpIdentAdr,LoadWhat which,ReturnWhat rewhich)
{
	int id=i_FindTmpIdentAdr;
	int m=0;
	char itsFunName[100],CurIdent[100];
	//保存当前待判断的标识符在CurIdent
	if(which == Name1)
		strcpy(CurIdent,MediaTab[i_FindTmpIdentAdr].name1);	
	else if(which == Name2)
		strcpy(CurIdent,MediaTab[i_FindTmpIdentAdr].name2);	
	else if(which == Result)
		strcpy(CurIdent,MediaTab[i_FindTmpIdentAdr].result);	
	do{
		id--;
	}while(MediaTab[id].ins!=Func && id>=0);
	if(MediaTab[id].ins == Func)
		strcpy(itsFunName,MediaTab[id].name1);	//找到离他最近的函数，然后再从这个函数开始找其中临时变量、常量、数组的地址
	while(strcmp(Tab[m].name,itsFunName)!=0)	//从符号表的头部开始读，读到他所在的函数名就停止
		m++;
	do{
		m++;
	}while(strcmp(CurIdent,Tab[m].name)!=0);	//从所在函数名继续往下读，直到读到待查的标识符就停止
	if(strcmp(CurIdent,Tab[m].name)==0)
	{
		if(rewhich == Radfirst)
			return Tab[m].adrFirst;
		else if(rewhich == Rtype)
			return Tab[m].type;
		else if(rewhich == Robj)
			return Tab[m].obj;
	}
}
/*寻找参数对应的地址*/	//已经解决
int FindParaAdr(int i_FindParaAdr,int id_Para)
{
	int ParaId=0;
	for(ParaId=0;ParaId<TabNum;ParaId++)
	{
		if(Tab[ParaId].obj == FUNC && strcmp(MediaTab[i_FindParaAdr].result,Tab[ParaId].name)==0)
			return Tab[ParaId + id_Para].adrFirst;	//id_Para从1开始，逐次递增
		else
			continue;
	}
}
/*寻找函数的长度*/
int FindFunLen(int i_FindFunLen)
{
	int m=0;
	for(m=0;m<CodeNum;m++)
	{
		if(MediaTab[m].ins == Func && strcmp(MediaTab[m].name1,MediaTab[i_FindFunLen].result)==0)	//找到被调用函数的长度
			//printf("被调用的函数是%s\t长度为%d\n",MediaTab[m].name1,MediaTab[m].FunLen);
			return MediaTab[m].FunLen;
	}
}
/*转换参数下标*/
int FindParaIndex(int i_FindParaIndex)
{
	char para_index[100];
	int h=4,g=0;
	for(h=4;MediaTab[i_FindParaIndex].name2[h]!='\0';g++)
	{
		para_index[g]=MediaTab[i_FindParaIndex].name2[h];
		g++;
		h++;
	}
	return((atoi)(para_index));
}
//判断所在层次
JudgeMain FindIdentLoc(int i_FindIdentLoc,LoadWhat l_FindIdentLoc)
{
	int Id1518=0,id=i_FindIdentLoc;
	int m=0,tmplev;
	char itsFunName[100]={'\0'},CurIdent[100]={'\0'};
	//保存当前待判断的标识符在CurIdent
	if(l_FindIdentLoc == Name1)
		strcpy(CurIdent,MediaTab[i_FindIdentLoc].name1);	
	else if(l_FindIdentLoc == Name2)
		strcpy(CurIdent,MediaTab[i_FindIdentLoc].name2);	
	else if(l_FindIdentLoc == Result)
		strcpy(CurIdent,MediaTab[i_FindIdentLoc].result);	
	while(MediaTab[id].ins!=Func)		//往前找到离他最近的函数
		id--;
	if(MediaTab[id].ins == Func)
		strcpy(itsFunName,MediaTab[id].name1);	//找到离他最近的函数，然后再从这个函数开始找其中临时变量、常量、数组的地址		
	while(strcmp(Tab[m].name,itsFunName)!=0)	//从符号表的头部开始读，读到他所在的函数名就停止,因为是从前往后找，此时找到的一定是函数定义部分
		m++;
	tmplev = Tab[m].lev;	//保存当前层次
	while(Tab[m].lev==tmplev)	//保证不是main函数
	{
		if(strcmp(CurIdent,Tab[m].name)==0  && strcmp(itsFunName,"main")!=0)	//如果不是main函数
			return NotMain;	
		else if(strcmp(CurIdent,Tab[m].name)==0  && strcmp(itsFunName,"main")==0)	//如果在main函数里面
			return JustInMain;
		else
			m++;
	}
	for(Id1518=0;Id1518<RunIndex;Id1518++)
	{
		if(l_FindIdentLoc == Name1 && strcmp(RunStack[Id1518].name,CurIdent)==0
			|| l_FindIdentLoc == Name2 && strcmp(RunStack[Id1518].name,CurIdent)==0
			|| l_FindIdentLoc == Result && strcmp(RunStack[Id1518].name,CurIdent)==0)
			return IsMain;//是全局的
	}
}
/*寻找应该跳转到哪个case*/
int WhichCase(int i_WhichCase)		//ThisLabel Label labelCon_2 2
{
	int i = i_WhichCase+1;
	char switB[100],switB1[100];
	strcpy(switB,MediaTab[i_WhichCase].result);	//switB 的样例SwitEnd_0  
	while(strcmp(switB,MediaTab[i].result)!=0)
	{
		if(strncmp(MediaTab[i].name1,"labelcon_",9)==0)		//只判断前九位是否一样, 比较字符串str1和str2的前n个字符串字典序的大小，但是不区分字母大小写
		{
			if(MediaTab[i].result[0]=='\'')		//如果这个常量是字符	ThisLabel Label labelCon_9 'E'  常量存在result里面了
				Many[switId].SwitchCase[switCaseId].caseCon = MediaTab[i].result[1];
			else if(isDigit(MediaTab[i].result[0])!=-1||MediaTab[i].result[0]=='+'||MediaTab[i].result[0]=='-')	//是整数	ThisLabel Label labelCon_7 1
				Many[switId].SwitchCase[switCaseId].caseCon = (atoi)(MediaTab[i].result);
			strcpy(Many[switId].SwitchCase[switCaseId].SwitchGoto,MediaTab[i].name1);
			fprintf(TargetCodeStream,"beq\t$t0\t%d\t%s\n",Many[switId].SwitchCase[switCaseId].caseCon,
			Many[switId].SwitchCase[switCaseId].SwitchGoto);//name1	
			switCaseId++;
		}
		else if(MediaTab[i].ins == Swit)	//处理switch内嵌套switch
		{
			strcpy(switB1,MediaTab[i].result);
			do{i++;}while(strcmp(switB1,MediaTab[i].result)!=0);
		}
		i++;
	}
	if(strcmp(switB,MediaTab[i].result)==0)		//如果与判断条件都不相等，就跳转到这个switch的结束处
		fprintf(TargetCodeStream,"j\t%s\n",MediaTab[i].name1);	//ThisLabel Label label_0 SwitEnd_0	   name1就是label_0
	return i;
}