#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WordAnalysis.h"
#include "grammar.h"
#include "ErrorHandling.h"
#include  "KindsOfTabs.h"
#include "IntermediateCode.h"
#include "TargetCode.h"

FILE * GraOutStream = NULL;//输出文件
FILE * GraInStream = NULL;

int index= 0;//结构体数组下标
int count=0;//文件行数
int CodeFlag=0;//进入代码段(.text)的标志
int IsChar=-1;//临时变量是字符的标志
int ReturnFlag=0;

int CopyIntoStruct()
{
	if((GraInStream = fopen("15182615_李娇阳_词法分析结果.docx", "r+"))==NULL)
	{
          printf("Open WordResultFile Failed!\n"); 
          return -1; 
    } 
	while(!feof(GraInStream))
	{
		fscanf(GraInStream,"%d%d%s\n%[^\n]",&Str[index].line,&Str[index].OriLine,Str[index].kind,Str[index].name);
		index++;
		//\n%[^\n]
	}
	return index;
}
//获取下一个单词
void nextsym()
{
	index++;
}
//<加法运算符>		AddOperator
void Plus()
{
	if (strcmp(Str[index].name,"'+'")==0|| strcmp(Str[index].name,"'-'") == 0)  //如果是字符
	{
		nextsym();
		printf("it is a AddOperator!\n");
	}
	else if(strcmp(Str[index].kind,"Plus")==0|| strcmp(Str[index].kind,"Minus") == 0)  //考虑++3-2的情况，如果在表达式中以运算符的形式存在
	{
		nextsym();
		printf("it is a AddOperator!\n");
	}
}
//<乘法运算符>		MultiOperator
void Mult()
{
	if(strcmp(Str[index].name,"'*'") == 0 || strcmp(Str[index].name,"'/'") == 0)//如果是字符
	{
		nextsym();
		printf("it is a MultiOperator!\n");
	}
	else if (strcmp(Str[index].kind,"Star") == 0 || strcmp(Str[index].kind,"Div") == 0) //考虑3*2的情况，如果在项中以运算符的形式存在
	{
		nextsym();
		printf("it is a MultiOperator!\n");
	}
}
//<关系运算符>		RelaOperators  < <= > >= != ==
void Rela()
{
	if (strcmp(Str[index].kind, "Less") == 0 || strcmp(Str[index].kind, "L_E") == 0 || strcmp(Str[index].kind, "Grea") == 0 || strcmp(Str[index].kind, "G_E") == 

0 || strcmp(Str[index].kind, "N_E") == 0 || strcmp(Str[index].kind, "Equ") == 0)
	{
		nextsym();
		printf("it is a RelationalOperator!\n");
	}
}
//<字母>		Alpha     只能有一个字符   ＿｜a｜．．．｜z｜A｜．．．｜Z  之一
void Alpha()
{
	if(strcmp(Str[index].kind, "CHSym") == 0)
	{
		if(isLetter(Str[index].name[1])==1)  // 'a'
		{
			printf("It is an Alpha!\n");
			nextsym();
		}
	}
}
//<数字>		Digital   ?????  只能有一个数字  0|1|....|9
void Digital()
{
	if(strcmp(Str[index].kind, "CHSym") == 0)
	{
		if(isDigit(Str[index].name[1])!=-1) // '2'
		{
			printf("It is a Digital!\n");
			nextsym();
		}
	}
}
//<字符>		Character
void Chara()
{
	if(strcmp(Str[index].kind, "CHSym") == 0)
	{
		if (strcmp(Str[index].name,"'+'")==0|| strcmp(Str[index].name,"'-'") == 0)
			Plus();
		else if(strcmp(Str[index].name,"'*'") == 0 || strcmp(Str[index].name,"'/'") == 0)
			Mult();
		else if(isLetter(Str[index].name[1])==1 && Str[index].name[0]=='\'' && Str[index].name[2]=='\'')
			Alpha();//因为是这样存储的 'a' 所以判断的下标是1
		else if(isDigit(Str[index].name[1])!=-1 && Str[index].name[0]=='\'' && Str[index].name[2]=='\'')
			Digital();
		printf("It is a Character!\n");
	}
}
//<字符串>		Strings
void Strings()
{
	int i=1;
	if(strcmp(Str[index].kind,"StrSym") == 0 && Str[index].name[0]=='\"')
	{
		for(i=1;Str[index].name[i]!='\"';i++)
		{
			if(Str[index].name[i]>=32 && Str[index].name[i]<=126 && Str[index].name[i]!=34)
				continue;
			else  //字符串不符合要求	23
				Error(23);
		}
		printf("It is a Strings!\n");
		nextsym();
	}
}
//程序		Program	
void Program()
{
	//输出到需要写入中间代码的文件
	InterCodeStream = fopen("InterCode.txt", "w");
	if(strcmp(Str[index].kind,"constSym")==0)
		ConstantDescription();
	if((strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 ) && strcmp(Str[index+2].kind,"L_S_Par")!=0)
		VariableDescription();
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 || strcmp(Str[index].kind,"voidSym")==0 )
		CodeFlag=1;//进入代码段的标志
	while(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 || (strcmp(Str[index].kind,"voidSym")==0 && strcmp(Str[index+1].kind,"mainSym")!=0)  )
	{
		if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
			ReturnValueFunctionDefinition();
		else if(strcmp(Str[index].kind,"voidSym")==0 && strcmp(Str[index+1].kind,"mainSym")!=0)
			NoReturnValueFunctionDefinition();
	}
	MainFunction();
	printf("It is a Program!\n");
}
//<常量说明>		Constant Description  能进入这个函数说明已经判断过第一个保留字是const
void ConstantDescription()
{
	do{
		nextsym();
		ConstantDefinition();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a ConstantDescription!\n");
		}
		else  //缺少分号		6
		{
			Error(6);
			if(IntoMain==0)
				Jump(6);	//跳到下一个const或int或char或void
			else
				Jump(7);
		}
	}while(strcmp(Str[index].kind,"constSym")==0);
}
//<常量定义>	Constant Definition   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
void ConstantDefinition()
{
	char tmp[1024]={'\0'},NegTmp[1024]="-",FinInt[1024];
	if(strcmp(Str[index].kind,"intSym")==0)
	{
		nextsym();
		Identifier();
		if(ErrorFlag==20)	//缺少合法常量名
			Jump(5);	//跳到下一个等号
		strcpy(tmp,Str[index-1].name);//把标识符名字存在tmp里面
		if(IntoFun==0)	//表示还没进入函数，是全局常量？
			findResult(index-1);//分为全局常量和局部常量考虑
		else if(IntoFun==1)	//表示进入函数了
			findTmpSame(index-1);
		if(strcmp(Str[index].kind,"Assign")==0)
		{
			nextsym();
			Integer();
			if(strcmp(Str[index-2].kind,"Minus")==0)	//如果是负数，在前面加上负号
			{
				strcat(NegTmp,Str[index-1].name);
				strcpy(FinInt,NegTmp);
			}
			else
				strcpy(FinInt,Str[index-1].name);	//否则，直接复制到FinInt
			//写入中间代码
			if(IntoFun==0)//表示是全局常量
			{
				TabNum = enterTab(tmp,INT,GCON,(atoi)(Str[index-1].name),adr);//常量地址adr
				InsTypeName1Res(Gcon,Int,tmp,FinInt);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//表示是临时常量,进入函数了
			{
				TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),FunLen);//常量地址FunLen
				InsTypeName1Res(Tcon,Int,tmp,FinInt);//临时常量构成地址的一部分
				FunLen+=4;
			}
			else if(IntoFun==1 && IntoMain==1)//表示进入main函数
			{
				TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),adr);//常量地址FunLen
				InsTypeName1Res(Tcon,Int,tmp,FinInt);//临时常量构成地址的一部分
				adr+=4;
			}
			strcpy(NegTmp,"-");
			memset(FinInt,'\0',1024);
			while(strcmp(Str[index].kind,"Comma")==0)
			{
				nextsym();
				Identifier();
				if(ErrorFlag==20)	//如果不是合法标识符那么跳到下一个逗号或分号
					Jump(5);	//跳到下一个等号
				strcpy(tmp,Str[index-1].name);//把标识符名字存在tmp里面
				if(IntoFun==0)	//表示还没进入函数，是全局常量？
					findResult(index-1);//分为全局常量和局部常量考虑
				else if(IntoFun==1)	//表示进入函数了
					findTmpSame(index-1);
				if(strcmp(Str[index].kind,"Assign")==0)
				{
					nextsym();
					Integer();
					if(strcmp(Str[index-2].kind,"Minus")==0)	//如果是负数，在前面加上负号
					{
						strcat(NegTmp,Str[index-1].name);
						strcpy(FinInt,NegTmp);
					}
					else
						strcpy(FinInt,Str[index-1].name);	//否则，直接复制到FinInt
					//写入中间代码
					if(IntoFun==0)//表示是全局常量
					{
						TabNum = enterTab(tmp,INT,GCON,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Gcon,Int,tmp,FinInt);
						adr+=4;
					}
					else if(IntoFun==1&& IntoMain==0)//表示是临时常量,进入函数了
					{
						TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),FunLen);
						InsTypeName1Res(Tcon,Int,tmp,FinInt);
						FunLen+=4;
					}
					else if(IntoFun==1 && IntoMain==1)//表示进入main函数
					{
						TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),adr);//常量地址FunLen
						InsTypeName1Res(Tcon,Int,tmp,FinInt);//临时常量构成地址的一部分
						adr+=4;
					}
					strcpy(NegTmp,"-");		
					memset(FinInt,'\0',1024);	//清零
				}
				else //如果不是=		7
				{
					Error(7);
					Jump(2);
				}
			}
			printf("It is a ConstantDefinition!\n");
		}
		else //如果不是=		7
		{
			Error(7);
			Jump(2);	//跳到下一个逗号或分号
		}
	}
	else if(strcmp(Str[index].kind,"charSym")==0)
	{
		nextsym();
		Identifier();
		if(ErrorFlag==20)	//如果不是合法标识符那么跳到下一个逗号或分号
			Jump(5);	//跳到下一个等号
		strcpy(tmp,Str[index-1].name);//把标识符名字存在tmp里面
		if(IntoFun==0)	//表示还没进入函数，是全局常量？
			findResult(index-1);//分为全局常量和局部常量考虑
		else if(IntoFun==1)	//表示进入函数了
			findTmpSame(index-1);
		if(strcmp(Str[index].kind,"Assign")==0)
		{
			nextsym();
			Chara();//字符
			//写入中间代码
			if(IntoFun==0)//表示是全局常量
			{
				TabNum = enterTab(tmp,CHAR,GCON,(int)(Str[index-1].name[1]),adr);
				InsTypeName1Res(Gcon,Char,tmp,Str[index-1].name);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//表示是临时常量,进入函数了
			{
				TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),FunLen);
				InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
				FunLen+=4;
			}
			else if(IntoFun==1 && IntoMain==1)//表示进入main函数
			{
				TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),adr);
				InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
				adr+=4;
			}
			while(strcmp(Str[index].kind,"Comma")==0)
			{
				nextsym();
				Identifier();
				strcpy(tmp,Str[index-1].name);//把标识符名字存在tmp里面
				if(IntoFun==0)	//表示还没进入函数，是全局常量？
					findResult(index-1);//分为全局常量和局部常量考虑
				else if(IntoFun==1)	//表示进入函数了
					findTmpSame(index-1);
				if(strcmp(Str[index].kind,"Assign")==0)
				{
					nextsym();
					Chara();//字符
					//写入中间代码
					if(IntoFun==0)//表示是全局常量
					{
						TabNum = enterTab(tmp,CHAR,GCON,(int)(Str[index-1].name[1]),adr);
						InsTypeName1Res(Gcon,Char,tmp,Str[index-1].name);
						adr+=4;
					}
					else if(IntoFun==1 && IntoMain==0)//表示是临时常量,进入函数了
					{
						TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),FunLen);
						InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
						FunLen+=4;
					}
					else if(IntoFun==1 && IntoMain==1)//表示进入main函数
					{
						TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),adr);
						InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
						adr+=4;
					}
				}
				else //如果不是=		7
				{
					Error(7);
					Jump(2);	//跳到下一个逗号或分号
				}
			}
			printf("It is a ConstantDefinition!\n");
		}
		else //如果不是=		7
		{
			Error(7);
			Jump(2);	//跳到下一个逗号或分号
		}
	}
	else //缺少int或char		13
	{
		Error(13);
		Jump(1);	//跳到下一个分号,因为缺少类型，就算录入符号表也没有意义，所以干脆跳到常量定义结尾处
	}
}
//<无符号整数>	Unsigned Integer  进入这个函数默认了首字符是0到9
void UnsignedInteger()
{
	int i=1;
	if (strcmp(Str[index].kind,"Num") == 0)
	{
		if (isDigit(Str[index].name[0])>=1 && isDigit(Str[index].name[0])<=9)  //首位不是0，其实按照词法分析处理后首位不可能是0
		{
			printf("It is a NotZeroDigital!\n");
			for(i=1;Str[index].name[i]!='\0';i++)
			{
				if(isDigit(Str[index].name[i])!=-1) //0到9
				{
					printf("It is a Digital!\n");
					continue;
				}
				else //非法无符号整数	19
					Error(19);
			}
			printf("It is a UnsignedInteger!\n");
			nextsym();
		}
	}
}
//<整数>		Integer  不知道0前面不可以有+-怎么表达
void Integer()
{
	char tmp[1024]={'\0'};
	int NegFlag=0;
	//IsChar=0;
	if (strcmp(Str[index].kind,"Plus")==0|| strcmp(Str[index].kind,"Minus") == 0)
	{
		if(strcmp(Str[index].kind,"Minus") == 0)
			NegFlag=1;
		Plus();
	}
	if (isDigit(Str[index].name[0])>=1 && isDigit(Str[index].name[0])<=9)
	{
		UnsignedInteger();
		if(NegFlag==1)	//排除是减号而不是负号的情况，如果前面是标识符就是减号
		{
			makeVar(ReNeg);
			if(IntoMain==0)
			{
				enterTab(ReNeg,INT,TTVAR,0,FunLen);	//此时的临时变量一定是整数
				FunLen+=4;
			}
			else if(IntoMain==1)
			{
				enterTab(ReNeg,INT,GTVAR,0,adr);
				adr+=4;
			}
			InsName1Res(Neg,Str[index-1].name,ReNeg);
			strcpy(ReInt,ReNeg);
		}
		else
			strcpy(ReInt,Str[index-1].name);
		printf("It is a Integer!\n");
	}
	else if(isDigit(Str[index].name[0])==0 && Str[index].name[1]=='\0') //“单独”的0
	{	
		printf("It is a Integer!\n");
		nextsym();
		strcpy(ReInt,Str[index-1].name);
	}
}
//<标识符>		Identifier	?????    9abc的情况还没考虑
void Identifier()
{
	int i = 1;
	if(strcmp(Str[index].kind,"IdSym")==0)  //是标识符
	{
		if(isLetter(Str[index].name[0])==1) //第一位是字母！
		{
			printf("It is an Alpha!\n"); //第一位是字母！
			for(i=1;Str[index].name[i]!='\0';i++)
			{
				if(isLetter(Str[index].name[i])==1)//字母
				{
					printf("It is an Alpha!\n");
					continue;
				}
				else if(isDigit(Str[index].name[i])!=-1)//数字
				{
					printf("It is a Digital!\n");
					continue;
				}	
				else  //非法标识符   20
					Error(20);
			}
			printf("It is a Identifier!\n");
			nextsym();
		}
		else  //非法标识符   20
			Error(20);
	}
	else  //非法标识符   20
		Error(20);
}
//<声明头部>		DeclareHead   地址adr还没搞  进入声明头部，说明是在有返回值的函数定义里面   进入这里说明不是main函数
void DeclareHead()
{
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 )
	{
		nextsym();
		Identifier();
		Lev++;
		findResult(index-1);//函数名一定是全局考虑
		if(strcmp(Str[index-2].kind,"intSym")==0)
		{
			FunLen = 0;//进入一个新的int函数，设置此函数的长度是0
			TabNum = enterTab(Str[index-1].name,INT,FUNC,0,FunLen);//函数名的值设为0，地址 FunLen
			CurFun_CodeNum = CodeNum;//记下当前函数的codenum
			InsTypeName1(Func,Int,Str[index-1].name);//函数定义加入中间代码
			FunLen+=4;
		}
		else if(strcmp(Str[index-2].kind,"charSym")==0)
		{
			FunLen = 0;//进入一个新的int函数，设置此函数的长度是0
			TabNum = enterTab(Str[index-1].name,CHAR,FUNC,0,FunLen);//函数名的值设为0，地址FunLen
			CurFun_CodeNum = CodeNum;//记下当前函数的codenum
			InsTypeName1(Func,Char,Str[index-1].name);//函数定义加入中间代码
			FunLen+=4;
		}
		IntoFun = 1;
		printf("It is a DeclareHead!\n");
	}
	else //缺少int或char   13
		Error(13);
}
//<变量说明>		Variable Description	＜变量定义＞;{＜变量定义＞;}
void VariableDescription()
{
	do{
		VariableDefinition();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a VariableDescription!\n");
		}
		else //缺分号		6
		{
			Error(6);
			if(IntoMain==0)
				Jump(3);	//跳到下一个int或char或void
			else if(IntoMain==1 && strcmp(Str[index].kind,"Assign")!=0)
				Jump(7);	//跳到下一个const或int或char或语句的头符号,虽然此时不可能出现const了，但是还是不再单独开一个case来写了
			else if(IntoMain==1 && strcmp(Str[index].kind,"Assign")==0)
				Jump(8);	//跳到下一个分号的后一个位置
		}
	}while((strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 ) && strcmp(Str[index+2].kind,"L_S_Par")!=0);
}
//<变量定义>		VariableDefinition	?????   地址adr还没搞
//＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
void VariableDefinition()
{
	char TypeTmp[100]={'\0'};
	TypeIdentifier();
	if(ErrorFlag==13)	//缺少类型标识符int或char
		Jump(1);	//跳到下一个分号	＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
	strcpy(TypeTmp,Str[index-1].kind);
	Identifier();
	if(ErrorFlag==20)	//缺少变量名
		Jump(2);	//跳到下一个逗号或分号
	if(IntoFun==0)	//表示还没进入函数，是全局变量？
		findResult(index-1);//分为全局变量和局部变量考虑
	else if(IntoFun==1)	//表示进入函数了
		findTmpSame(index-1);
	if(strcmp(Str[index].kind,"L_M_Par")!=0) //表示是<类型标识符><标识符>，如int a
	{
		if(strcmp(TypeTmp,"intSym")==0)
		{
			//写入中间代码
			if(IntoFun==0)//表示是全局变量
			{
				TabNum = enterTab(Str[index-1].name,INT,GVAR,0,adr);
				InsTypeName1(Gvar,Int,Str[index-1].name);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//表示是函数内部的变量定义
			{
				TabNum = enterTab(Str[index-1].name,INT,TVAR,0,FunLen);
				InsTypeName1(Tvar,Int,Str[index-1].name);
				FunLen+=4;
			}
			else if(IntoFun==1 && IntoMain==1)
			{
				TabNum = enterTab(Str[index-1].name,INT,TVAR,0,adr);
				InsTypeName1(Tvar,Int,Str[index-1].name);
				adr+=4;
			}
		}
		else if(strcmp(TypeTmp,"charSym")==0)//如char melon
		{
			//写入中间代码
			if(IntoFun==0)//表示是全局变量或者是main函数
			{
				TabNum = enterTab(Str[index-1].name,CHAR,GVAR,0,adr);
				InsTypeName1(Gvar,Char,Str[index-1].name);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//表示是函数内部的变量定义
			{
				TabNum = enterTab(Str[index-1].name,CHAR,TVAR,0,FunLen);
				InsTypeName1(Tvar,Char,Str[index-1].name);
				FunLen+=4;
			}
			else if(IntoFun==1 && IntoMain==1)
			{
				TabNum = enterTab(Str[index-1].name,CHAR,TVAR,0,adr);
				InsTypeName1(Tvar,Char,Str[index-1].name);
				adr+=4;
			}
		}
	}
	else if(strcmp(Str[index].kind,"L_M_Par")==0) //int a[2]  数组
	{
		nextsym();
		UnsignedInteger();
		if(strcmp(Str[index-1].kind,"Num") != 0)	//非法数组长度
		{
			Error(19);
			Jump(2);	//跳到下一个逗号或分号
		}
		if(strcmp(Str[index].kind,"R_M_Par")==0)
		{
			if(strcmp(TypeTmp,"intSym")==0)
			{
				//写入中间代码
				if(IntoFun==0)//表示是全局数组或者是main函数
				{
					TabNum = enterTab(Str[index-3].name,INT,GARR,(atoi)(Str[index-1].name),adr);
					InsTypeName1Res(Garr,Int,Str[index-3].name,Str[index-1].name);
					adr+=(atoi)(Str[index-1].name)*4;
				}
				else if(IntoFun==1 && IntoMain==0)//表示是函数内部的数组定义
				{
					TabNum = enterTab(Str[index-3].name,INT,TARR,(atoi)(Str[index-1].name),FunLen);
					InsTypeName1Res(Tarr,Int,Str[index-3].name,Str[index-1].name);
					FunLen+=(atoi)(Str[index-1].name)*4;
				}
				else if(IntoFun==1 && IntoMain==1)
				{
					TabNum = enterTab(Str[index-3].name,INT,TARR,(atoi)(Str[index-1].name),adr);
					InsTypeName1Res(Tarr,Int,Str[index-3].name,Str[index-1].name);
					adr+=(atoi)(Str[index-1].name)*4;
				}
			}
			else if(strcmp(TypeTmp,"charSym")==0)
			{
				//写入中间代码
				if(IntoFun==0)//表示是全局数组或是main函数里面的数组
				{
					TabNum = enterTab(Str[index-3].name,CHAR,GARR,(atoi)(Str[index-1].name),adr);
					InsTypeName1Res(Garr,Char,Str[index-3].name,Str[index-1].name);
					adr+=(atoi)(Str[index-1].name)*4;
				}
				else if(IntoFun==1 && IntoMain==0)//表示是函数内部的数组定义
				{
					TabNum = enterTab(Str[index-3].name,CHAR,TARR,(atoi)(Str[index-1].name),FunLen);
					InsTypeName1Res(Tarr,Char,Str[index-3].name,Str[index-1].name);
					FunLen+=(atoi)(Str[index-1].name)*4;
				}
				else if(IntoFun==1 && IntoMain==1)
				{
					TabNum = enterTab(Str[index-3].name,CHAR,TARR,(atoi)(Str[index-1].name),adr);
					InsTypeName1Res(Tarr,Char,Str[index-3].name,Str[index-1].name);
					adr+=(atoi)(Str[index-1].name)*4;
				}
			}
			nextsym();
		}
		else  //缺右中括号	3
		{
			Error(3);
			Jump(2);	//跳到下一个逗号或分号
		}
	}
	while(strcmp(Str[index].kind,"Comma")==0)
	{
		nextsym();
		Identifier();
		if(ErrorFlag==20)	//缺少变量名
			Jump(2);	//跳到下一个逗号或分号
		if(IntoFun==0)	//表示还没进入函数，是全局常量？
			findResult(index-1);//分为全局常量和局部常量考虑
		else if(IntoFun==1)	//表示进入函数了
			findTmpSame(index-1);
		if(strcmp(Str[index].kind,"L_M_Par")!=0) //表示是<类型标识符><标识符>，如int a,b
		{
			if(strcmp(TypeTmp,"intSym")==0)
			{
				//写入中间代码
				if(IntoFun==0)//表示是全局变量
				{
					TabNum = enterTab(Str[index-1].name,INT,GVAR,0,adr);
					InsTypeName1(Gvar,Int,Str[index-1].name);
					adr+=4;
				}
				else if(IntoFun==1 && IntoMain == 0)//表示是函数内部的变量定义
				{
					TabNum = enterTab(Str[index-1].name,INT,TVAR,0,FunLen);
					InsTypeName1(Tvar,Int,Str[index-1].name);
					FunLen+=4;
				}
				else if(IntoFun==1 && IntoMain==1)
				{
					TabNum = enterTab(Str[index-1].name,INT,TVAR,0,adr);
					InsTypeName1(Tvar,Int,Str[index-1].name);
					adr+=4;
				}
			}
			else if(strcmp(TypeTmp,"charSym")==0)
			{
				//写入中间代码
				if(IntoFun==0)//表示是全局变量
				{
					TabNum = enterTab(Str[index-1].name,CHAR,GVAR,0,adr);
					InsTypeName1(Gvar,Char,Str[index-1].name);
					adr+=4;
				}
				else if(IntoFun==1 && IntoMain == 0)//表示是函数内部的变量定义
				{
					TabNum = enterTab(Str[index-1].name,CHAR,TVAR,0,FunLen);
					InsTypeName1(Tvar,Char,Str[index-1].name);
					FunLen+=4;
				}
				else if(IntoFun==1 && IntoMain==1)
				{
					TabNum = enterTab(Str[index-1].name,CHAR,TVAR,0,adr);
					InsTypeName1(Tvar,Char,Str[index-1].name);
					adr+=4;
				}
			}
		}
		else if(strcmp(Str[index].kind,"L_M_Par")==0)
		{
			nextsym();
			UnsignedInteger();
			if(strcmp(Str[index-1].kind,"Num") != 0)	//非法数组长度
			{
				Error(19);
				Jump(2);	//跳到下一个逗号或分号
			}
			if(strcmp(Str[index].kind,"R_M_Par")==0) //int a,b[2]
			{
				if(strcmp(TypeTmp,"intSym")==0)
				{
					//写入中间代码
					if(IntoFun==0)//表示是全局数组
					{
						TabNum = enterTab(Str[index-3].name,INT,GARR,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Garr,Int,Str[index-3].name,Str[index-1].name);
						adr+=(atoi)(Str[index-1].name)*4;
					}
					else if(IntoFun==1 && IntoMain == 0)//表示是函数内部的数组定义
					{
						TabNum = enterTab(Str[index-3].name,INT,TARR,(atoi)(Str[index-1].name),FunLen);
						InsTypeName1Res(Tarr,Int,Str[index-3].name,Str[index-1].name);
						FunLen+=(atoi)(Str[index-1].name)*4;
					}
					else if(IntoFun==1 && IntoMain==1)
					{
						TabNum = enterTab(Str[index-3].name,INT,TARR,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Tarr,Int,Str[index-3].name,Str[index-1].name);
						adr+=(atoi)(Str[index-1].name)*4;
					}
				}
				else if(strcmp(TypeTmp,"charSym")==0)
				{
					//写入中间代码
					if(IntoFun==0)//表示是全局数组
					{
						TabNum = enterTab(Str[index-3].name,CHAR,GARR,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Garr,Char,Str[index-3].name,Str[index-1].name);
						adr+=(atoi)(Str[index-1].name)*4;
					}
					else if(IntoFun==1 && IntoMain==0)//表示是函数内部的数组定义
					{
						TabNum = enterTab(Str[index-3].name,CHAR,TARR,(atoi)(Str[index-1].name),FunLen);
						InsTypeName1Res(Tarr,Char,Str[index-3].name,Str[index-1].name);
						FunLen+=(atoi)(Str[index-1].name)*4;
					}
					else if(IntoFun==1 && IntoMain==1)
					{
						TabNum = enterTab(Str[index-3].name,CHAR,TARR,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Tarr,Char,Str[index-3].name,Str[index-1].name);
						adr+=(atoi)(Str[index-1].name)*4;
					}
				}
				nextsym();
			}
			else  //缺右中括号	3
			{
				Error(3);
				Jump(2);	//跳到下一个逗号或分号
			}
		}
	}
	printf("It is a VariableDefinition!\n");
}
//<类型标识符>	TypeIdentifier	
void TypeIdentifier()
{
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
	{
		printf("It is a TypeIdentifier!\n");
		nextsym();
	}
	else  //缺少int或char		13
		Error(13);
}
//<有返回值函数定义>		ReturnValueFunctionDefinition
void ReturnValueFunctionDefinition()
{
	char FunName[100];
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
	{
		DeclareHead();
		strcpy(FunName,Str[index-1].name);	//保存函数名字
		if(strcmp(Str[index].kind,"L_S_Par")==0)
		{
			nextsym();
			ParametersTable();
			if(strcmp(Str[index].kind,"R_S_Par")==0)
			{
				nextsym();
				if(strcmp(Str[index].kind,"L_B_Par")==0)
				{
					nextsym();
					CompoundStatement();
					if(ReturnFlag==0)
						Error(15);	//缺少返回值
					ReturnFlag=0;//用完以后初始化为0
					if(strcmp(Str[index].kind,"R_B_Par")==0)
					{
						printf("It is a ReturnValueFunctionDefinition!\n");
						IntoFun = 0;//表示小函数结束
						MediaTab[CurFun_CodeNum].FunLen = FunLen;//把此函数的长度付给函数定义的标识符的中间代码结构体数组，然后在把funlen设为0
						FunLen=0;
						makeEndLabel();
						InsName1Res(End,EndId,FunName);//把函数结尾写入中间代码
						enterTab(EndId,VOID,REEND,0,adr);	//一定是在main函数之外的,END写入符号表
						adr+=4;
						nextsym();
					}
					else //缺少右大括号	5
					{
						Error(5);
						Jump(3); //跳到下一个int或char或void
					}
				}
				else  //缺少左大括号	4
				{
					Error(4);
					Jump(3); //跳到下一个int或char或void
				}
			}
			else  //缺少右小括号	2
			{
				Error(2);
				Jump(3); //跳到下一个int或char或void
			}
		}
		else  //缺少左小括号	1
		{
			Error(1);
			Jump(3); //跳到下一个int或char或void
		}
	}
}
//<无返回值函数定义>		NoReturnValueFunctionDefinition   地址adr还没搞
//＜无返回值函数定义＞  ::= void＜标识符＞‘(’＜参数表＞‘)’‘{’＜复合语句＞‘}’
void NoReturnValueFunctionDefinition()
{
	char FunName[100];
	if(strcmp(Str[index].kind,"voidSym")==0)
	{
		nextsym();
		Identifier();
		strcpy(FunName,Str[index-1].name);	//保存函数名字
		CurFun_CodeNum = CodeNum;
		InsTypeName1(Func,Void,Str[index-1].name);//函数定义写入中间代码
		findResult(index-1);//函数名不能和符号表所有元素重名
		FunLen = 0;//进入一个新的void函数，设置此函数的长度是0
		Lev++;
		TabNum = enterTab(Str[index-1].name,VOID,FUNC,0,FunLen);//地址是FunLen
		FunLen+=4;
		IntoFun = 1;
		if(strcmp(Str[index].kind,"L_S_Par")==0)
		{
			nextsym();
			ParametersTable();
			if(strcmp(Str[index].kind,"R_S_Par")==0)
			{
				nextsym();
				if(strcmp(Str[index].kind,"L_B_Par")==0)
				{
					nextsym();
					CompoundStatement();
					if(ReturnFlag!=0)
						Error(27);	//缺少返回值
					if(strcmp(Str[index].kind,"R_B_Par")==0)
					{
						printf("It is a NoReturnValueFunctionDefinition!\n");
						IntoFun = 0;//表示小函数结束
						MediaTab[CurFun_CodeNum].FunLen = FunLen;//把此函数的长度付给函数定义的标识符的中间代码结构体数组，然后在把funlen设为0
						FunLen=0;
						makeEndLabel();
						InsName1Res(End,EndId,FunName);//把函数结尾写入中间代码
						enterTab(EndId,VOID,NOREEND,0,adr);	//一定是在main函数之外的
						adr+=4;
						nextsym();
					}
					else //缺少右大括号	5
						Error(5);
				}
				else  //缺少左大括号	4
					Error(4);
			}
			else  //缺少右小括号	2
				Error(2);
		}
		else  //缺少左小括号	1
			Error(1);
	}
}
//<复合语句>		CompoundStatement
void CompoundStatement()
{
	if(strcmp(Str[index].kind,"constSym")==0)
		ConstantDescription();
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
		VariableDescription();
	StatementColumn();
	printf("It is a CompoundStatement!\n");
}
//<参数表>	ParametersTable	 地址adr还没搞
//＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
void ParametersTable()
{
	char TypeTmp[100]={'\0'}; 
	int TotalPara=0;
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
	{
		TypeIdentifier();
		strcpy(TypeTmp,Str[index-1].kind);
		Identifier();
		if(ErrorFlag==20)	//缺少参数名，跳到下一个逗号或分号
			Jump(2);
		if(strcmp(TypeTmp,"intSym")==0)
		{
			TabNum = enterTab(Str[index-1].name,INT,PARA,0,FunLen);//此处value还需考虑,地址FunLen
			FunLen+=4;
			InsTypeName1(Para,Int,Str[index-1].name);
		}
		else if(strcmp(TypeTmp,"charSym")==0)
		{
			TabNum = enterTab(Str[index-1].name,CHAR,PARA,0,FunLen);
			FunLen+=4;
			InsTypeName1(Para,Char,Str[index-1].name);
		}
		while(strcmp(Str[index].kind,"Comma")==0)
		{
			nextsym();
			TypeIdentifier();
			strcpy(TypeTmp,Str[index-1].kind);
			Identifier();
			if(ErrorFlag==20)	//缺少参数名，跳到下一个逗号或分号
				Jump(2);
			if(strcmp(TypeTmp,"intSym")==0)
			{
				TabNum = enterTab(Str[index-1].name,INT,PARA,0,FunLen);
				FunLen+=4;
				InsTypeName1(Para,Int,Str[index-1].name);
			}
			else if(strcmp(TypeTmp,"charSym")==0)
			{
				TabNum = enterTab(Str[index-1].name,CHAR,PARA,0,FunLen);
				FunLen+=4;
				InsTypeName1(Para,Int,Str[index-1].name);
			}
		}
		printf("It is a ParametersTable!\n");
	}
	//没有加else的情况是为了配合<空>
	else if(strcmp(Str[index].kind,"R_S_Par")==0)
	{
		printf("It is a ParametersTable!\n");//这里没有nextsym因为后续有统一的nextsym
	}
}
//<主函数>		MainFunction	  地址adr还没搞
void MainFunction()
{
	if(strcmp(Str[index].kind,"voidSym")==0)
	{
		nextsym();
		if(strcmp(Str[index].kind,"mainSym")==0)
		{
			Lev++;
			TabNum = enterTab("main",VOID,FUNC,0,adr);
			CurFun_CodeNum = CodeNum;//记下当前函数的codenum
			InsTypeName1(Func,Void,Str[index].name);//当前指向main，把他存入中间代码
			adr+=4;
			nextsym();
			if(strcmp(Str[index].kind,"L_S_Par")==0)
			{
				nextsym();
				if(strcmp(Str[index].kind,"R_S_Par")==0)
				{
					nextsym();
					IntoFun = 1;//进入主函数
					IntoMain = 1;
					if(strcmp(Str[index].kind,"L_B_Par")==0)
					{
						nextsym();
						CompoundStatement();
						if(strcmp(Str[index].kind,"R_B_Par")==0)
						{
							nextsym(); //这里应该是程序的结束了
							IntoFun = 0;//离开主函数
							IntoMain = 0; //这里把它注释掉是因为后面要把main写入运行栈
							makeEndLabel();
							InsName1(End,EndId);//把函数结尾写入中间代码
							enterTab(EndId,VOID,MAINEND,0,adr);	//一定是在main函数之外的
							adr+=4;
							printf("It is a MainFunction!\n");
						}
						else //缺少右大括号	5
							Error(5);
					}
					else  //缺少左大括号	4
						Error(4);
				}
				else  //缺少右小括号	2
					Error(2);
			}
			else  //缺少左小括号	1
				Error(1);
		}
		else  //缺少main	9
			Error(9);
	}
	else  //缺少void	10
		Error(10);
}
//<表达式>		Expression	
void Expression()
{
	char plustmp[10]={'\0'},ItemTmp1[1024]={'\0'},ItemTmp2[1024]={'\0'},FirstPlus[10]={'\0'};
	char *NoNeg;//去除负号，因为出现负负得正的情况
	if(strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0||strcmp(Str[index].kind,"IdSym")==0
		||strcmp(Str[index].kind,"Num")==0||strcmp(Str[index].kind,"CHSym")==0||strcmp(Str[index].kind,"L_S_Par")==0)
	{
		if(strcmp(Str[index].kind,"Minus")==0)
			strcpy(FirstPlus,"-");
		if(strcmp(Str[index].kind,"Plus")==0|| strcmp(Str[index].kind,"Minus") == 0)
			Plus();
		Item();
		strcpy(ReExpr,ReTerm);
		if(strcmp(FirstPlus,"-")==0)
		{
			strcpy(ItemTmp1,ReExpr);//-tmp3(假如是3)copy到ItemTmp1
			makeVar(ReNeg);
			if(IntoMain==0)
			{
				enterTab(ReNeg,VOID,TTVAR,0,FunLen);
				FunLen+=4;
			}
			else if(IntoMain==1)
			{
				enterTab(ReNeg,VOID,GTVAR,0,adr);
				adr+=4;
			}
			InsName1Res(Neg,ItemTmp1,ReNeg);
			strcpy(ReExpr,ReNeg);	//最后再把取反后的值付给表达式
		}
		while(strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0)
		{
			strcpy(ItemTmp1,ReExpr);
			Plus();
			strcpy(plustmp,Str[index-1].kind);
			Item();
			strcpy(ItemTmp2,ReTerm);
			if(strcmp(plustmp,"Plus")==0)
			{
				makeVar(ReExpr);
				if( (ItemTmp1[0]=='\''&&ItemTmp2[0]=='\'' && ItemTmp1[1]+ItemTmp2[1]>=32 && ItemTmp1[1]+ItemTmp2[1]<=126)
					|| IsChar==1&&(ItemTmp1[0]=='\''||ItemTmp2[0]=='\'') )
					//保证字符在ASCII可显示范围内
						IsChar=1;
				else 
					IsChar=0;
				if(IntoMain==0)
				{
					if(IsChar==1)
						enterTab(ReExpr,CHAR,TTVAR,0,FunLen);
					if(IsChar==0)
						enterTab(ReExpr,INT,TTVAR,0,FunLen);
					FunLen+=4;
				}
				else if(IntoMain==1)
				{
					if(IsChar==1)
						enterTab(ReExpr,CHAR,GTVAR,0,adr);
					if(IsChar==0)
						enterTab(ReExpr,INT,GTVAR,0,adr);
					adr+=4;
				}
				InsName12Res(Add,ItemTmp1,ItemTmp2,ReExpr);
			}
			else if(strcmp(plustmp,"Minus")==0)  //如果是 3---7的情况？
			{
				makeVar(ReExpr);
				if(ItemTmp2[0]=='-')
				{
					NoNeg=ItemTmp2 + 1;
					strcpy(ItemTmp2,NoNeg);
					memset(NoNeg,'\0',sizeof(NoNeg));
				}
				if(ItemTmp1[0]=='\''&&ItemTmp2[0]=='\'' || IsChar==1&&(ItemTmp1[0]=='\''||ItemTmp2[0]=='\''))
				{
					if(ItemTmp1[1]-ItemTmp2[1]>=32 && ItemTmp1[1]-ItemTmp2[1]<=126)	//保证字符在ASCII可显示范围内
						IsChar=1;
					else 
						IsChar=0;
				}
				else 
					IsChar=0;
				//临时变量录入符号表
				if(IntoMain==0)
				{
					if(IsChar==1)
						enterTab(ReExpr,CHAR,TTVAR,0,FunLen);
					else if(IsChar==0)
						enterTab(ReExpr,INT,TTVAR,0,FunLen);
					FunLen+=4;
				}
				else if(IntoMain==1)
				{
					if(IsChar==1)
						enterTab(ReExpr,CHAR,GTVAR,0,adr);
					else if(IsChar==0)
						enterTab(ReExpr,INT,GTVAR,0,adr);
					adr+=4;
				}
				InsName12Res(Min,ItemTmp1,ItemTmp2,ReExpr);
			}
		}
		printf("It is a Expression!\n");
	}
}
//<项>		Item
void Item()
{
	char FacTmp1[100]={'\0'};
	char FacTmp2[100]={'\0'};
	char OpTmp[10] = {'\0'};
	Factor();
	strcpy(ReTerm,ReFact);
	while(strcmp(Str[index].kind,"Star")==0 || strcmp(Str[index].kind,"Div")==0)
	{
		strcpy(FacTmp1,ReTerm);
		strcpy(OpTmp,Str[index].kind);
		nextsym();
		Factor();
		strcpy(FacTmp2,ReFact);
		makeVar(ReTerm);
		//临时变量录入符号表
		if(IntoMain==0)
		{
			if(IsChar==1)
				enterTab(ReTerm,CHAR,TTVAR,0,FunLen);
			else if(IsChar==0)
				enterTab(ReTerm,INT,TTVAR,0,FunLen);
			FunLen+=4;
		}
		else if(IntoMain==1)
		{
			if(IsChar==1)
				enterTab(ReTerm,CHAR,GTVAR,0,adr);
			else if(IsChar==0)
				enterTab(ReTerm,INT,GTVAR,0,adr);
			adr+=4;
		}
		if(strcmp(OpTmp,"Star")==0)
			InsName12Res(Mul,FacTmp1,FacTmp2,ReTerm);
		else if(strcmp(OpTmp,"Div")==0)
			InsName12Res(Div,FacTmp1,FacTmp2,ReTerm);
	}
	printf("It is a Item!\n");
}
//<因子>		Factor    ?????????
void Factor()
{
	char idTmp[1024]={'\0'};
	int ischartmp;
	IdentType arrType;
	if(strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0)	//有返回值的函数调用
	{
		ReturnValueFunctionCallStatement();
		strcpy(ReFact,ReRetS);//把又返回值的函数的返回值付给因子
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"IdSym")==0)
	{
		Identifier();	//数组名或变量名
		if(GraFindTabType(index-1) == IntGra)
		{
			IsChar=0;	//如果是int类型，ischar是0
			arrType = INT;
		}
		else if(GraFindTabType(index-1) == CharGra)
		{
			IsChar=1;	//如果是int类型，ischar是0
			arrType = CHAR;
		}
		ischartmp = IsChar;
		strcpy(idTmp,Str[index-1].name);//标识符或数组名字存在idTmp里面
		if(strcmp(Str[index].kind,"L_M_Par")==0)	//表示是数组
		{
			if(findIdentT_G(idTmp)!=GARR && findIdentT_G(idTmp)!=TARR)	//首先判断是否定义过这个数组
				fprintf(ErrorOut,"数组未定义：第%d行%s\n",Str[index-1].OriLine,Str[index-1].name);
			nextsym();
			Expression();	//这里存放数组下标，不能影响ischar的判断，因此判断不作数
			IsChar=ischartmp;
			makeVar(ReArrG);
			//临时变量录入符号表
			if(IntoMain==0)
			{
				if(arrType == INT)
					enterTab(ReArrG,INT,TTVAR,0,FunLen);
				else if(arrType = CHAR)
					enterTab(ReArrG,CHAR,TTVAR,0,FunLen);
				FunLen+=4;
			}
			else if(IntoMain==1)
			{
				if(arrType == INT)
					enterTab(ReArrG,INT,GTVAR,0,adr);
				if(arrType == CHAR)
					enterTab(ReArrG,CHAR,GTVAR,0,adr);
				adr+=4;
			}
			InsName12Res(ArrG,idTmp,ReExpr,ReArrG);//ReArrG[100]存从数组取来的值
			strcpy(ReFact,ReArrG);//从数组取来的值存入ReFact
			if(strcmp(Str[index].kind,"R_M_Par")==0)
				nextsym();
			else  //缺少右中括号	3
				Error(3);
		}
		else  //表示是单独的<标识符>，绝对不能是数组名，因为前面已经判断过了
		{
			if(findIdentT_G(idTmp)==-1 || findIdentT_G(idTmp)==GARR || findIdentT_G(idTmp)==TARR)	//首先判断是否定义过这个标识符
				fprintf(ErrorOut,"标识符未定义：第%d行%s\n",Str[index-1].OriLine,Str[index-1].name);
			strcpy(ReFact,idTmp);//当前的标识符存入ReFact
		}
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"Num")==0 || strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0) //整数
	{
		Integer();
		strcpy(ReFact,ReInt);//当前的整数存入ReFact
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"CHSym") == 0) //字符
	{
		Chara();
		strcpy(ReFact,Str[index-1].name);//当前的字符存入ReFact
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Expression();
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			strcpy(ReFact,ReExpr);
			nextsym();
			printf("It is a Factor!\n");
		}
		else  //缺少)		2
			Error(2);
	}
}
//<语句>		Statement   ????????
void Statement()
{
	if(strcmp(Str[index].kind,"Semi")==0)
	{
		printf("It is a Statement!\n");
		nextsym();
	}
	else if(strcmp(Str[index].kind,"ifSym")==0)
	{
		ConditionalStatement();
		printf("It is a Statement!\n");
	}
	else if(strcmp(Str[index].kind,"doSym")==0)
	{
		LoopStatement();
		printf("It is a Statement!\n");
	}
	else if(strcmp(Str[index].kind,"switchSym")==0)
	{
		SituationStatement();
		printf("It is a Statement!\n");
	}
	else if(strcmp(Str[index].kind,"L_B_Par")==0)
	{
		nextsym();
		StatementColumn();
		if(strcmp(Str[index].kind,"R_B_Par")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else //缺少}		5
			Error(5);
	}
	else if(strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0)
	{
		if( FindFuncName(Str[index].name)==INT || FindFuncName(Str[index].name)==CHAR )//查找当前标识符所代表的函数类型
			ReturnValueFunctionCallStatement();//此处有bug，有返回值函数和无返回值函数无法区分
		else if( FindFuncName(Str[index].name)== VOID)
			NoReturnValueFunctionCallStatement();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else  //缺少;		6
		{
			Error(6);	//运行到此处有可能是缺少分号，也有可能是函数未定义直接引用，导致上述两个小函数都没进入，运行到此处还是函数名，所以没有分号
			Jump(1);	//缺少分号，跳到下一个分号
		}
	}
	else if(strcmp(Str[index].kind,"IdSym")==0 && (strcmp(Str[index+1].kind,"Assign")==0 || strcmp(Str[index+1].kind,"L_M_Par")==0) )
	{
		AssignmentStatement();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else  //缺少;		6
		{
			Error(6);
			Jump(1);	//缺少分号，跳到下一个分号
		}
	}
	else if(strcmp(Str[index].kind,"scanfSym")==0)
	{
		ReadSentences();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else  //缺少;		6
		{
			Error(6);
			Jump(1);	//缺少分号，跳到下一个分号
		}
	}
	else if(strcmp(Str[index].kind,"printfSym")==0)
	{
		WriteStatement();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else  //缺少;		6
		{
			Error(6);
			Jump(1);	//缺少分号，跳到下一个分号
		}
	}
	else if(strcmp(Str[index].kind,"Semi")==0)
	{
		nextsym();
		printf("It is a Statement!\n");
	}
	else if(strcmp(Str[index].kind,"returnSym")==0)
	{
		ReturnStatement();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else  //缺少;		6
		{
			Error(6);
			Jump(1);	//缺少分号，跳到下一个分号
		}
	}
}
//<赋值语句>		AssignmentStatement
void AssignmentStatement()
{
	char IdentTmp[1024]={'\0'};
	char ReExTmp1[1024]={'\0'};
	char ReExTmp2[1024]={'\0'};
	int arrlen=0;
	IdentType Left,Right;
	Identifier();
	if(GraFindTabType(index-1)==IntGra)
		Left = INT;
	else if(GraFindTabType(index-1)==CharGra)
		Left = CHAR;
	arrlen = findArrLen(index-1);	//数组长度保存在arrlen里面
	strcpy(IdentTmp,Str[index-1].name);//首先把标识符名字存入IdentTmp
	if(strcmp(Str[index].kind,"Assign")==0)//变量赋值
	{
		nextsym();
		Expression();
		if(strcmp(Str[index-1].kind,"Num")==0 || findInTab(ReExpr)==INT )
			Right = INT;
		else if(findInTab(ReExpr)==CHAR)
			Right = CHAR;
		else if(strcmp(Str[index-1].kind,"CHSym")==0)
				Right = Left;
		if(Left == CHAR && Right == INT)	//不报错，只是写在报错文件里面，相当于警告
			fprintf(ErrorOut,"第%d行%s\t此处有int向char类型的转换\n",Str[index-1].OriLine,Str[index-1].name);
		InsName1Res(Ass,IdentTmp,ReExpr);
		printf("It is a AssignmentStatement!\n");
	}
	else if(strcmp(Str[index].kind,"L_M_Par")==0)//数组赋值
	{
		nextsym();
		Expression();
		if(isDigit(ReExpr[0])!=-1 && atoi(ReExpr)>=arrlen)  //如果下标刚好是数字，可以在这里判断是否访问越界;数组下标不可能为负
			Error(28);
		strcpy(ReExTmp1,ReExpr);
		if(strcmp(Str[index].kind,"R_M_Par")==0)
		{
			nextsym();
			if(strcmp(Str[index].kind,"Assign")==0)
			{
				nextsym();
				Expression();
				if(strcmp(Str[index-1].kind,"Num")==0 || findInTab(ReExpr)==INT )
					Right = INT;
				else if(findInTab(ReExpr)==CHAR)
					Right = CHAR;
				else if(strcmp(Str[index-1].kind,"CHSym")==0)
					 Right = Left;
				if(Left == CHAR && Right == INT)
					fprintf(ErrorOut,"第%d行%s\t此处有int向char类型的转换\n",Str[index-1].OriLine,Str[index-1].name);
				strcpy(ReExTmp2,ReExpr);
				InsName12Res(ArrA,IdentTmp,ReExTmp1,ReExTmp2);//数组赋值
				printf("It is a AssignmentStatement!\n");
			}
			else  //缺少等号		7
			{
				Error(7);
				Jump(1);	//跳到下一个分号
			}
		}
		else  //缺少右中括号		3
		{
			Error(3);
			Jump(1);	//跳到下一个分号
		}
	}
	else  //非法赋值，应该只能是 = [ 之一		21
	{
		Error(21);
		Jump(1);	//跳到下一个分号
	}
}
//<条件语句>		ConditionalStatement	能进入这个函数说明已经判断过if了
void ConditionalStatement()
{
	//实现提前用后面的label
	char TmpLabel[100]="label_";
	char LabelAdd[3];
	itoa(LabelNum,LabelAdd,10);//数字转字符串存在LabelTmp里面
	strcat(TmpLabel,LabelAdd);//把标签下标和标签名拼接起来
	nextsym();
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Condition();
		InsName1Res(FGoto,ReCond,TmpLabel); //已经写入MediaTab了
		LabelNum++;
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			nextsym();
			Statement();
			//enterTab(TmpLabel,VOID,LABEL,0,adr);//把当前标签写入符号表
			fprintf(InterCodeStream,"%s\n",TmpLabel);
			AddLabel(TmpLabel);//标签写入MediaTab
			printf("It is a ConditionalStatement!\n");
		}
		else  //缺少右小括号		2
		{
			Error(2);
			Jump(1);	//跳到下一个分号
		}
	}
	else  //缺少左小括号		1
	{
		Error(1);
		Jump(1);	//跳到下一个分号
	}
}
//<条件>		Condition   ？？？？？
void Condition()
{
	char ExTmp1[1024]={'\0'};
	char ExTmp2[1024]={'\0'};
	char rela[100]={'\0'};
	Expression();
	strcpy(ExTmp1,ReExpr);
	if(strcmp(Str[index].kind,"R_S_Par")!=0) //表示是有关系运算符的，否则不进入此分支
	{
		Rela(); //关系运算符
		strcpy(rela,Str[index-1].kind);//关系运算符存到rela
		Expression();
		strcpy(ExTmp2,ReExpr);
		makeVar(ReCond);
		//临时变量录入符号表
		if(IntoMain==0)
		{
			enterTab(ReCond,VOID,TTVAR,0,FunLen);
			FunLen+=4;
		}
		else if(IntoMain==1)
		{
			enterTab(ReCond,VOID,GTVAR,0,adr);
			adr+=4;
		}
		if(strcmp(rela, "Less") == 0)
			InsName12Res(Less,ExTmp1,ExTmp2,ReCond);
		else if(strcmp(rela, "L_E") == 0)
			InsName12Res(LaE,ExTmp1,ExTmp2,ReCond);
		else if(strcmp(rela, "Grea") == 0)
			InsName12Res(Grea,ExTmp1,ExTmp2,ReCond);
		else if(strcmp(rela, "G_E") == 0)
			InsName12Res(GaE,ExTmp1,ExTmp2,ReCond);
		else if(strcmp(rela, "N_E") == 0)
			InsName12Res(NaE,ExTmp1,ExTmp2,ReCond);
		else if(strcmp(rela, "Equ") == 0)
			InsName12Res(Equ,ExTmp1,ExTmp2,ReCond);
	}
	else if(strcmp(Str[index].kind,"R_S_Par")==0)//表示<条件>只是一个表达式
	{
		makeVar(ReCond);
		//临时变量录入符号表
		if(IntoMain==0)
		{
			enterTab(ReCond,VOID,TTVAR,0,FunLen);
			FunLen+=4;
		}
		else if(IntoMain==1)
		{
			enterTab(ReCond,VOID,GTVAR,0,adr);
			adr+=4;
		}
		InsName12Res(Grea,ExTmp1,"0",ReCond);  //表示<表达式>为正
	}
	printf("It is a Condition!\n");
}
//<循环语句>		LoopStatement
void LoopStatement()
{
	char TmpLabel[100]="label_";
	LableIntoMTab();//添加标签
	AddLabel(LabelNumber);//标签写入MediaTab
	strcpy(TmpLabel,LabelNumber);
	nextsym();
	Statement();
	if(strcmp(Str[index].kind,"whileSym")==0)
	{
		nextsym();
		if(strcmp(Str[index].kind,"L_S_Par")==0)
		{
			nextsym();
			Condition();
			InsName1Res(TGoto,ReCond,TmpLabel);//为正跳转
			if(strcmp(Str[index].kind,"R_S_Par")==0)
			{
				printf("It is a LoopStatement!\n");
				nextsym();
			}
			else  //缺少右小括号		2
			{
				Error(2);
				ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
			}
		}
		else  //缺少左小括号		1
		{
			Error(1);
			ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
		}
	}
	else  //缺少while		11
	{
		Error(11);
		ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
	}
}
//<常量>		Constant
void Constant()
{
	if(strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0 || isDigit(Str[index].name[0])!=-1)
	{
		Integer();
		printf("It is a Constant!\n");
	}
	else if(strcmp(Str[index].kind, "CHSym") == 0) //如果不添加这个可能会在Chara()中报错
	{
		Chara();	
		printf("It is a Constant!\n");
	}
	else  //常量错误		22
		Error(22);
}
//<情况语句>		SituationStatement
void SituationStatement()
{
	//实现提前用后面的label
	char SwitchTmp[100],SwitchPair[100];	//SwitchPair用于switch的配对
	char LabelAdd[3];
	strcpy(SwitchTmp,"label_");
	itoa(LabelNum,LabelAdd,10);//数字转字符串存在LabelTmp里面
	strcat(SwitchTmp,LabelAdd);//把标签下标和标签名拼接起来
	MakeSwitchEndLabel();
	strcpy(SwitchPair,SwitBEid);	//当前的switchend的id 存起来
	nextsym();
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Expression();  //需要被判断的表达式
		InsName1Res(Swit,ReExpr,SwitchPair);//留作后续判断，此处仅仅输出
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			nextsym();
			if(strcmp(Str[index].kind,"L_B_Par")==0)
			{
				LabelNum++;
				nextsym();
				SituationTable(SwitchTmp);
				if(strcmp(Str[index].kind,"R_B_Par")==0)
				{
					InsName1(SwitEnd,emmEndSwit);
					fprintf(InterCodeStream,"%s\n",SwitchTmp);
					InsTypeName1Res(ThisLabel,Label,SwitchTmp,SwitchPair);
					printf("It is a SituationStatement!\n");
					nextsym();
				}
				else  //缺少右大括号	5
				{
					Error(5);
					ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
				}
			}
			else  //缺少左大括号	4
			{
				Error(4);
				ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
			}
		}
		else  //缺少右小括号	2
		{
			Error(2);
			ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
		}
	}
	else  //缺少左小括号	1
	{
		Error(1);
		ErrorFlag=0;	//这样处理是因为我不知道该一直跳过跳到哪
	}
}
//<情况表>		SituationTable	第一个必须是case才行
void SituationTable(char tmp[])
{
	if(strcmp(Str[index].kind,"caseSym")==0)
	{
		SituationSubSentence(tmp);
		while(strcmp(Str[index].kind,"caseSym")==0)
		{
			SituationSubSentence(tmp);
		}
		printf("It is a SituationTable!\n");
	}
	else  //缺少case		12
	{
		Error(12);
		Jump(4);	//跳到下一个大括号，离开switch
	}
}
//<情况子语句>		SituationSubSentence
void SituationSubSentence(char tmp[])
{
	nextsym();
	Constant();
	AddCaseLabel(index);
	if(strcmp(Str[index].kind,"Colon")==0)
	{
		nextsym();
		Statement();
		InsName1(Goto,tmp);//一个case结束以后就跳转到switch结尾处
		printf("It is a SituationSubSentence!\n");
	}
	else  //缺少冒号		8
		Error(8);
}
//<有返回值函数调用语句>		ReturnValueFunctionCallStatement
void ReturnValueFunctionCallStatement()
{
	char FuncName[100]={'\0'};
	IdentType funcType;
	if(GraFindTabObj(index)!=FUNC)	//首先判断是否定义过这个函数
		fprintf(ErrorOut,"有返回值的函数调用中函数未定义：第%d行%s\n",Str[index].OriLine,Str[index].name);
	Identifier();	//函数名
	if(ErrorFlag==20)
		Jump(1);	//如果没有合法函数名，就跳到下一个分号
	if(GraFindTabType(index-1)==IntGra)	//查函数的类型
	{
		IsChar=0;
		funcType = INT;
	}
	else if(GraFindTabType(index-1)==CharGra)	//查函数的类型
	{
		IsChar=1;
		funcType = CHAR;
	}
	else
		Error(30);
	strcpy(FuncName,Str[index-1].name);//先把函数名保存起来
	InsName1(CallBegin,FuncName);
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		ValueParameterTable(FuncName);
		//写入中间代码
		InsName1(CallEnd,FuncName);
		makeVar(ReRetS);
		//临时变量录入符号表
		if(IntoMain==0)
		{
			if(funcType == INT)	//查函数的类型
				enterTab(ReRetS,INT,TTVAR,0,FunLen);
			else if(funcType == CHAR)	//查函数的类型
				enterTab(ReRetS,CHAR,TTVAR,0,FunLen);
			FunLen+=4;
		}
		else if(IntoMain==1)
		{
			if(funcType == INT)	//查函数的类型
				enterTab(ReRetS,INT,GTVAR,0,adr);
			else if(funcType == CHAR)	//查函数的类型
				enterTab(ReRetS,CHAR,GTVAR,0,adr);
			adr+=4;
		}
		if(funcType == INT)	//查函数的类型
			InsTypeName1(RetS,Int,ReRetS);	//定义返回值类型
		else if(funcType == CHAR)	//查函数的类型
			InsTypeName1(RetS,Char,ReRetS);	//定义返回值类型
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			printf("It is a ReturnValueFunctionCallStatement!\n");
			nextsym();
		}
		else  //缺少右小括号		2
			Error(2);
	}
	else  //缺少左小括号		1
		Error(1);
}
//<无返回值函数调用语句>		NoReturnValueFunctionCallStatement
void NoReturnValueFunctionCallStatement()
{
	char FuncName[100]={'\0'};
	Identifier();
	if(ErrorFlag==20)
		Jump(1);	//如果没有合法函数名，就跳到下一个分号
	strcpy(FuncName,Str[index-1].name);//先把函数名保存起来
	InsName1(CallBegin,FuncName);
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		ValueParameterTable(FuncName);  //把参数Push进去就好，不用在返回值了
		//写入中间代码
		InsName1(CallEnd,FuncName);
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			printf("It is a NoReturnValueFunctionCallStatement!\n");
			nextsym();
		}
		else  //缺少右小括号		2
		{
			Error(2);
			Jump(1);	//跳到下一个分号
		}
	}
	else  //缺少左小括号		1
	{
		Error(1);
		Jump(1);	//跳到下一个分号
	}
}
//<值参数表>		ValueParameterTable  包含<空>的情况
void ValueParameterTable(char fun[])
{
	char ParaName[100]="PARA";
	char tmp[100]={'\0'};
	int TmpParaNum;
	ParaNum=1;	//初始化为1
	if(strcmp(Str[index].kind,"R_S_Par")!=0) //如果是空的值参数表的情况就别进入下面的分支了
	{
		TmpParaNum = ParaNum;
		Expression();
		ParaNum = TmpParaNum;
		itoa(ParaNum,tmp,10);	
		strcat(ParaName,tmp);
		InsName12Res(Push,ReExpr,ParaName,fun);
		strcpy(ParaName,"PARA");	//存好以后PARA的下标清零
		memset(tmp,'\0',100);	//tmp清零
		ParaNum++;
		while(strcmp(Str[index].kind,"Comma")==0)
		{
			nextsym();
			TmpParaNum = ParaNum;
			Expression();
			ParaNum = TmpParaNum;
			itoa(ParaNum,tmp,10);	
			strcat(ParaName,tmp);
			InsName12Res(Push,ReExpr,ParaName,fun);	//Name1存要push的参数	Name2表示这个函数中参数的下标（从0开始） Result存要push进哪个函数
			strcpy(ParaName,"PARA");	//存好以后PARA的下标清零
			memset(tmp,'\0',100);	//tmp清零
			ParaNum++;
		}
	}
	printf("It is a ValueParameterTable!\n");
}
//<语句列>		StatementColumn   ?????
void StatementColumn()
{
	if(strcmp(Str[index].kind,"ifSym")==0 || strcmp(Str[index].kind,"doSym")==0 || strcmp(Str[index].kind,"switchSym")==0 ||
		 strcmp(Str[index].kind,"L_B_Par")==0 || (strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0) ||
		 (strcmp(Str[index].kind,"IdSym")==0 && (strcmp(Str[index+1].kind,"Assign")==0 || strcmp(Str[index+1].kind,"L_M_Par")==0)) ||
		 strcmp(Str[index].kind,"scanfSym")==0 || strcmp(Str[index].kind,"printfSym")==0 || strcmp(Str[index].kind,"Semi")==0 ||
		 strcmp(Str[index].kind,"returnSym")==0
		)
	{
		while(strcmp(Str[index].kind,"ifSym")==0 || strcmp(Str[index].kind,"doSym")==0 || strcmp(Str[index].kind,"switchSym")==0 ||
			 strcmp(Str[index].kind,"L_B_Par")==0 || (strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0) ||
			 (strcmp(Str[index].kind,"IdSym")==0 && (strcmp(Str[index+1].kind,"Assign")==0 || strcmp(Str[index+1].kind,"L_M_Par")==0)) ||
			 strcmp(Str[index].kind,"scanfSym")==0 || strcmp(Str[index].kind,"printfSym")==0 || strcmp(Str[index].kind,"Semi")==0 ||
			 strcmp(Str[index].kind,"returnSym")==0
			)
			Statement();
		printf("It is a StatementColumn!\n");
	}
}
//<读语句>	ReadStatement	
void ReadSentences()
{
	if(strcmp(Str[index].kind,"scanfSym")==0)
	{
		nextsym();
		if(strcmp(Str[index].kind,"L_S_Par")==0)
		{
			nextsym();
			Identifier();
			InsName1(Scan,Str[index-1].name);//把读标识符写入中间代码
			while(strcmp(Str[index].kind,"Comma")==0)
			{
				nextsym();
				Identifier();
				InsName1(Scan,Str[index-1].name);//把读标识符写入中间代码
			}
			if(strcmp(Str[index].kind,"R_S_Par")==0)
			{
				printf("It is a ReadSentences!\n");
				nextsym();
			}
			else  //缺少右小括号		2
				Error(2);
		}
		else  //缺少左小括号		1
			Error(1);
	}
	else  //缺少scanf		17
		Error(17);
}
//<写语句>		WriteStatement   ?????
void WriteStatement()
{
	char tmpStr[1024]={'\0'};
	if(strcmp(Str[index].kind,"printfSym")==0)
	{
		nextsym();
		if(strcmp(Str[index].kind,"L_S_Par")==0)
		{
			nextsym();
			if(strcmp(Str[index].kind,"StrSym")==0)
			{
				Strings();
				strcpy(tmpStr,Str[index-1].name);	//暂时保存当前字符串
				if(strcmp(Str[index].kind,"Comma")==0)
				{
					nextsym();
					Expression();
					if(ReExpr[0]=='\'')
						IsChar=1;
					InsName1(Prin,tmpStr);	//调整四元式顺序，保证先输出表达式后输出字符串
					InsName1(Prin,ReExpr);
					IsChar=0;
					if(strcmp(Str[index].kind,"R_S_Par")==0)
					{
						printf("It is a WriteStatement!\n");
						nextsym();
					}
					else  //缺少右小括号		2
						Error(2);
				}
				else if(strcmp(Str[index].kind,"R_S_Par")==0)
				{
					InsName1(Prin,tmpStr);
					printf("It is a WriteStatement!\n");
					nextsym();
				}
				else  //缺少右小括号		2
					Error(2);
			}
			else if(strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0||strcmp(Str[index].kind,"IdSym")==0
			||strcmp(Str[index].kind,"Num")==0||strcmp(Str[index].kind,"CHSym")==0||strcmp(Str[index].kind,"L_S_Par")==0)  //表达式first集合
			{
				Expression();
				if(ReExpr[0]=='\'')
					IsChar=1;
				InsName1(Prin,ReExpr);
				IsChar=0;	//用完以后初始化为-1
				if(strcmp(Str[index].kind,"R_S_Par")==0)
				{
					printf("It is a WriteStatement!\n");
					nextsym();
				}
				else  //缺少右小括号		2
					Error(2);
			}
		}
		else  //缺少左小括号		1
			Error(1);
	}
	else  //缺少printf		18
		Error(18);
}
//<返回语句>		ReturnStatement	能进这个函数说明return已经判断过了
void ReturnStatement()
{
	nextsym();
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Expression();
		if(strcmp(Str[index-1].kind,"Num")!=0 && strcmp(Str[index-1].kind,"CHSym")!=0 && findDef(ReExpr)==-1)
			Error(27);	//在返回值不是数字或字符的前提下去查符号表，没找到则报错，非法返回值
		InsName1(Ret,ReExpr);	//Ret不用写入符号表
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			printf("It is a ReturnStatement!\n");
			nextsym();
		}
		else  //缺少右小括号		2
			Error(2);
	}
	else  //return 后面不跟()也符合文法
	{
		InsName1(NRet,"NullStatement");
		printf("It is a ReturnStatement!\n");
	}
	ReturnFlag=1;
}
int GraFindTabType(int CurStrId)
{
	int s=0;
	for(s=0;s<TabNum;s++)
	{
		if(strcmp(Tab[s].name,Str[CurStrId].name)==0 && (Tab[s].obj == GARR || Tab[s].obj == TARR || Tab[s].obj == FUNC ||
			Tab[s].obj == GVAR||Tab[s].obj == GCON ||Tab[s].obj == TVAR||Tab[s].obj == TCON	||Tab[s].obj == PARA ||
			Tab[s].obj == GTVAR ||Tab[s].obj == TTVAR))	
		{
			if(Tab[s].type == CHAR)
				return CharGra;
			else if(Tab[s].type == INT)
				return IntGra;
			else if(Tab[s].type == VOID)
				return VoidGra;
		}
	}
}
int GraFindTabObj(int CurStrId)
{
	int s=0;
	for(s=0;s<TabNum;s++)
	{
		if(strcmp(Tab[s].name,Str[CurStrId].name)==0)	
				return Tab[s].obj;
	}
	return -1;
}
int findIdentT_G(char name[])	//先从当前层次开始找，如果找不到再找全局
{
	int i = 0,n=0,u=0;
	while(Tab[n].lev != Lev)	//先从当前层次开始找
		n++;
	if(Tab[n].lev == Lev)	//先从当前层次开始找
	{
		for(i = n;Tab[i].lev == Lev && i<TabNum; i++)	
			if(strcmp(name,Tab[i].name)==0)//表示在该层次找到了
				return Tab[i].obj;
	}
	//还能运行到这说明没找到
	while(Tab[u].lev==0)
	{
		if(strcmp(name,Tab[u].name)==0)//表示在全局找到了
			return Tab[u].obj;
		else
			u++;
	}
	return -1;	//运行到这说明没找到
}
int findInTab(char name[])
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(stricmp(name,Tab[i].name)==0)
			return Tab[i].type;
	}
	return -1; //没找到
}
int main(){
	FILE *TabToTxt;
	int pu=0;
	WordAnalysis();
	printf("语法分析部分开始\n");
	count = CopyIntoStruct()-1;
	index = 0;
	ErrorOut = fopen("错误信息.txt", "w+");	//错误打印到txt里面
	Program();
	printf("语法分析部分结束\n");
	AddParaNumToFun();
	TabToTxt = fopen("Tab.txt","w+");
	fprintf(TabToTxt,"下标\t名字\t\t地址\t层数\t种类\t类型\t值\t参个数\n");
	fprintf(TabToTxt,"----------------------------------------------------------------------\n");
	for(pu=0;pu<TabNum;pu++)
		fprintf(TabToTxt,"%d\t%s\t\t%d\t%d\t%s\t%s\t%d\t%d\n",pu,Tab[pu].name,Tab[pu].adrFirst,Tab[pu].lev,PrinObj[Tab[pu].obj],PrinType[Tab[pu].type],Tab[pu].value,Tab[pu].AllParaNum);
	fclose(ErrorOut);
	fclose(TabToTxt);
	//if(ErrorNum==0)
	//{
	//全局变量和全局数组存入全局结构体数组
	IntoRunStack();
	CalInStack();
	printf("目标代码已生成\n");
	//}
	return 0;
}