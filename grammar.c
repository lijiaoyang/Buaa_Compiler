#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WordAnalysis.h"
#include "grammar.h"
#include "ErrorHandling.h"
#include  "KindsOfTabs.h"
#include "IntermediateCode.h"
#include "TargetCode.h"

FILE * GraOutStream = NULL;//����ļ�
FILE * GraInStream = NULL;

int index= 0;//�ṹ�������±�
int count=0;//�ļ�����
int CodeFlag=0;//��������(.text)�ı�־
int IsChar=-1;//��ʱ�������ַ��ı�־
int ReturnFlag=0;

int CopyIntoStruct()
{
	if((GraInStream = fopen("15182615_���_�ʷ��������.docx", "r+"))==NULL)
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
//��ȡ��һ������
void nextsym()
{
	index++;
}
//<�ӷ������>		AddOperator
void Plus()
{
	if (strcmp(Str[index].name,"'+'")==0|| strcmp(Str[index].name,"'-'") == 0)  //������ַ�
	{
		nextsym();
		printf("it is a AddOperator!\n");
	}
	else if(strcmp(Str[index].kind,"Plus")==0|| strcmp(Str[index].kind,"Minus") == 0)  //����++3-2�����������ڱ��ʽ�������������ʽ����
	{
		nextsym();
		printf("it is a AddOperator!\n");
	}
}
//<�˷������>		MultiOperator
void Mult()
{
	if(strcmp(Str[index].name,"'*'") == 0 || strcmp(Str[index].name,"'/'") == 0)//������ַ�
	{
		nextsym();
		printf("it is a MultiOperator!\n");
	}
	else if (strcmp(Str[index].kind,"Star") == 0 || strcmp(Str[index].kind,"Div") == 0) //����3*2���������������������������ʽ����
	{
		nextsym();
		printf("it is a MultiOperator!\n");
	}
}
//<��ϵ�����>		RelaOperators  < <= > >= != ==
void Rela()
{
	if (strcmp(Str[index].kind, "Less") == 0 || strcmp(Str[index].kind, "L_E") == 0 || strcmp(Str[index].kind, "Grea") == 0 || strcmp(Str[index].kind, "G_E") == 

0 || strcmp(Str[index].kind, "N_E") == 0 || strcmp(Str[index].kind, "Equ") == 0)
	{
		nextsym();
		printf("it is a RelationalOperator!\n");
	}
}
//<��ĸ>		Alpha     ֻ����һ���ַ�   �ߣ�a����������z��A����������Z  ֮һ
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
//<����>		Digital   ?????  ֻ����һ������  0|1|....|9
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
//<�ַ�>		Character
void Chara()
{
	if(strcmp(Str[index].kind, "CHSym") == 0)
	{
		if (strcmp(Str[index].name,"'+'")==0|| strcmp(Str[index].name,"'-'") == 0)
			Plus();
		else if(strcmp(Str[index].name,"'*'") == 0 || strcmp(Str[index].name,"'/'") == 0)
			Mult();
		else if(isLetter(Str[index].name[1])==1 && Str[index].name[0]=='\'' && Str[index].name[2]=='\'')
			Alpha();//��Ϊ�������洢�� 'a' �����жϵ��±���1
		else if(isDigit(Str[index].name[1])!=-1 && Str[index].name[0]=='\'' && Str[index].name[2]=='\'')
			Digital();
		printf("It is a Character!\n");
	}
}
//<�ַ���>		Strings
void Strings()
{
	int i=1;
	if(strcmp(Str[index].kind,"StrSym") == 0 && Str[index].name[0]=='\"')
	{
		for(i=1;Str[index].name[i]!='\"';i++)
		{
			if(Str[index].name[i]>=32 && Str[index].name[i]<=126 && Str[index].name[i]!=34)
				continue;
			else  //�ַ���������Ҫ��	23
				Error(23);
		}
		printf("It is a Strings!\n");
		nextsym();
	}
}
//����		Program	
void Program()
{
	//�������Ҫд���м������ļ�
	InterCodeStream = fopen("InterCode.txt", "w");
	if(strcmp(Str[index].kind,"constSym")==0)
		ConstantDescription();
	if((strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 ) && strcmp(Str[index+2].kind,"L_S_Par")!=0)
		VariableDescription();
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 || strcmp(Str[index].kind,"voidSym")==0 )
		CodeFlag=1;//�������εı�־
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
//<����˵��>		Constant Description  �ܽ����������˵���Ѿ��жϹ���һ����������const
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
		else  //ȱ�ٷֺ�		6
		{
			Error(6);
			if(IntoMain==0)
				Jump(6);	//������һ��const��int��char��void
			else
				Jump(7);
		}
	}while(strcmp(Str[index].kind,"constSym")==0);
}
//<��������>	Constant Definition   int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{,����ʶ���������ַ���}
void ConstantDefinition()
{
	char tmp[1024]={'\0'},NegTmp[1024]="-",FinInt[1024];
	if(strcmp(Str[index].kind,"intSym")==0)
	{
		nextsym();
		Identifier();
		if(ErrorFlag==20)	//ȱ�ٺϷ�������
			Jump(5);	//������һ���Ⱥ�
		strcpy(tmp,Str[index-1].name);//�ѱ�ʶ�����ִ���tmp����
		if(IntoFun==0)	//��ʾ��û���뺯������ȫ�ֳ�����
			findResult(index-1);//��Ϊȫ�ֳ����;ֲ���������
		else if(IntoFun==1)	//��ʾ���뺯����
			findTmpSame(index-1);
		if(strcmp(Str[index].kind,"Assign")==0)
		{
			nextsym();
			Integer();
			if(strcmp(Str[index-2].kind,"Minus")==0)	//����Ǹ�������ǰ����ϸ���
			{
				strcat(NegTmp,Str[index-1].name);
				strcpy(FinInt,NegTmp);
			}
			else
				strcpy(FinInt,Str[index-1].name);	//����ֱ�Ӹ��Ƶ�FinInt
			//д���м����
			if(IntoFun==0)//��ʾ��ȫ�ֳ���
			{
				TabNum = enterTab(tmp,INT,GCON,(atoi)(Str[index-1].name),adr);//������ַadr
				InsTypeName1Res(Gcon,Int,tmp,FinInt);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//��ʾ����ʱ����,���뺯����
			{
				TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),FunLen);//������ַFunLen
				InsTypeName1Res(Tcon,Int,tmp,FinInt);//��ʱ�������ɵ�ַ��һ����
				FunLen+=4;
			}
			else if(IntoFun==1 && IntoMain==1)//��ʾ����main����
			{
				TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),adr);//������ַFunLen
				InsTypeName1Res(Tcon,Int,tmp,FinInt);//��ʱ�������ɵ�ַ��һ����
				adr+=4;
			}
			strcpy(NegTmp,"-");
			memset(FinInt,'\0',1024);
			while(strcmp(Str[index].kind,"Comma")==0)
			{
				nextsym();
				Identifier();
				if(ErrorFlag==20)	//������ǺϷ���ʶ����ô������һ�����Ż�ֺ�
					Jump(5);	//������һ���Ⱥ�
				strcpy(tmp,Str[index-1].name);//�ѱ�ʶ�����ִ���tmp����
				if(IntoFun==0)	//��ʾ��û���뺯������ȫ�ֳ�����
					findResult(index-1);//��Ϊȫ�ֳ����;ֲ���������
				else if(IntoFun==1)	//��ʾ���뺯����
					findTmpSame(index-1);
				if(strcmp(Str[index].kind,"Assign")==0)
				{
					nextsym();
					Integer();
					if(strcmp(Str[index-2].kind,"Minus")==0)	//����Ǹ�������ǰ����ϸ���
					{
						strcat(NegTmp,Str[index-1].name);
						strcpy(FinInt,NegTmp);
					}
					else
						strcpy(FinInt,Str[index-1].name);	//����ֱ�Ӹ��Ƶ�FinInt
					//д���м����
					if(IntoFun==0)//��ʾ��ȫ�ֳ���
					{
						TabNum = enterTab(tmp,INT,GCON,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Gcon,Int,tmp,FinInt);
						adr+=4;
					}
					else if(IntoFun==1&& IntoMain==0)//��ʾ����ʱ����,���뺯����
					{
						TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),FunLen);
						InsTypeName1Res(Tcon,Int,tmp,FinInt);
						FunLen+=4;
					}
					else if(IntoFun==1 && IntoMain==1)//��ʾ����main����
					{
						TabNum = enterTab(tmp,INT,TCON,(atoi)(Str[index-1].name),adr);//������ַFunLen
						InsTypeName1Res(Tcon,Int,tmp,FinInt);//��ʱ�������ɵ�ַ��һ����
						adr+=4;
					}
					strcpy(NegTmp,"-");		
					memset(FinInt,'\0',1024);	//����
				}
				else //�������=		7
				{
					Error(7);
					Jump(2);
				}
			}
			printf("It is a ConstantDefinition!\n");
		}
		else //�������=		7
		{
			Error(7);
			Jump(2);	//������һ�����Ż�ֺ�
		}
	}
	else if(strcmp(Str[index].kind,"charSym")==0)
	{
		nextsym();
		Identifier();
		if(ErrorFlag==20)	//������ǺϷ���ʶ����ô������һ�����Ż�ֺ�
			Jump(5);	//������һ���Ⱥ�
		strcpy(tmp,Str[index-1].name);//�ѱ�ʶ�����ִ���tmp����
		if(IntoFun==0)	//��ʾ��û���뺯������ȫ�ֳ�����
			findResult(index-1);//��Ϊȫ�ֳ����;ֲ���������
		else if(IntoFun==1)	//��ʾ���뺯����
			findTmpSame(index-1);
		if(strcmp(Str[index].kind,"Assign")==0)
		{
			nextsym();
			Chara();//�ַ�
			//д���м����
			if(IntoFun==0)//��ʾ��ȫ�ֳ���
			{
				TabNum = enterTab(tmp,CHAR,GCON,(int)(Str[index-1].name[1]),adr);
				InsTypeName1Res(Gcon,Char,tmp,Str[index-1].name);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//��ʾ����ʱ����,���뺯����
			{
				TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),FunLen);
				InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
				FunLen+=4;
			}
			else if(IntoFun==1 && IntoMain==1)//��ʾ����main����
			{
				TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),adr);
				InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
				adr+=4;
			}
			while(strcmp(Str[index].kind,"Comma")==0)
			{
				nextsym();
				Identifier();
				strcpy(tmp,Str[index-1].name);//�ѱ�ʶ�����ִ���tmp����
				if(IntoFun==0)	//��ʾ��û���뺯������ȫ�ֳ�����
					findResult(index-1);//��Ϊȫ�ֳ����;ֲ���������
				else if(IntoFun==1)	//��ʾ���뺯����
					findTmpSame(index-1);
				if(strcmp(Str[index].kind,"Assign")==0)
				{
					nextsym();
					Chara();//�ַ�
					//д���м����
					if(IntoFun==0)//��ʾ��ȫ�ֳ���
					{
						TabNum = enterTab(tmp,CHAR,GCON,(int)(Str[index-1].name[1]),adr);
						InsTypeName1Res(Gcon,Char,tmp,Str[index-1].name);
						adr+=4;
					}
					else if(IntoFun==1 && IntoMain==0)//��ʾ����ʱ����,���뺯����
					{
						TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),FunLen);
						InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
						FunLen+=4;
					}
					else if(IntoFun==1 && IntoMain==1)//��ʾ����main����
					{
						TabNum = enterTab(tmp,CHAR,TCON,(int)(Str[index-1].name[1]),adr);
						InsTypeName1Res(Tcon,Char,tmp,Str[index-1].name);
						adr+=4;
					}
				}
				else //�������=		7
				{
					Error(7);
					Jump(2);	//������һ�����Ż�ֺ�
				}
			}
			printf("It is a ConstantDefinition!\n");
		}
		else //�������=		7
		{
			Error(7);
			Jump(2);	//������һ�����Ż�ֺ�
		}
	}
	else //ȱ��int��char		13
	{
		Error(13);
		Jump(1);	//������һ���ֺ�,��Ϊȱ�����ͣ�����¼����ű�Ҳû�����壬���Ըɴ��������������β��
	}
}
//<�޷�������>	Unsigned Integer  �����������Ĭ�������ַ���0��9
void UnsignedInteger()
{
	int i=1;
	if (strcmp(Str[index].kind,"Num") == 0)
	{
		if (isDigit(Str[index].name[0])>=1 && isDigit(Str[index].name[0])<=9)  //��λ����0����ʵ���մʷ������������λ��������0
		{
			printf("It is a NotZeroDigital!\n");
			for(i=1;Str[index].name[i]!='\0';i++)
			{
				if(isDigit(Str[index].name[i])!=-1) //0��9
				{
					printf("It is a Digital!\n");
					continue;
				}
				else //�Ƿ��޷�������	19
					Error(19);
			}
			printf("It is a UnsignedInteger!\n");
			nextsym();
		}
	}
}
//<����>		Integer  ��֪��0ǰ�治������+-��ô���
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
		if(NegFlag==1)	//�ų��Ǽ��Ŷ����Ǹ��ŵ���������ǰ���Ǳ�ʶ�����Ǽ���
		{
			makeVar(ReNeg);
			if(IntoMain==0)
			{
				enterTab(ReNeg,INT,TTVAR,0,FunLen);	//��ʱ����ʱ����һ��������
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
	else if(isDigit(Str[index].name[0])==0 && Str[index].name[1]=='\0') //����������0
	{	
		printf("It is a Integer!\n");
		nextsym();
		strcpy(ReInt,Str[index-1].name);
	}
}
//<��ʶ��>		Identifier	?????    9abc�������û����
void Identifier()
{
	int i = 1;
	if(strcmp(Str[index].kind,"IdSym")==0)  //�Ǳ�ʶ��
	{
		if(isLetter(Str[index].name[0])==1) //��һλ����ĸ��
		{
			printf("It is an Alpha!\n"); //��һλ����ĸ��
			for(i=1;Str[index].name[i]!='\0';i++)
			{
				if(isLetter(Str[index].name[i])==1)//��ĸ
				{
					printf("It is an Alpha!\n");
					continue;
				}
				else if(isDigit(Str[index].name[i])!=-1)//����
				{
					printf("It is a Digital!\n");
					continue;
				}	
				else  //�Ƿ���ʶ��   20
					Error(20);
			}
			printf("It is a Identifier!\n");
			nextsym();
		}
		else  //�Ƿ���ʶ��   20
			Error(20);
	}
	else  //�Ƿ���ʶ��   20
		Error(20);
}
//<����ͷ��>		DeclareHead   ��ַadr��û��  ��������ͷ����˵�������з���ֵ�ĺ�����������   ��������˵������main����
void DeclareHead()
{
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 )
	{
		nextsym();
		Identifier();
		Lev++;
		findResult(index-1);//������һ����ȫ�ֿ���
		if(strcmp(Str[index-2].kind,"intSym")==0)
		{
			FunLen = 0;//����һ���µ�int���������ô˺����ĳ�����0
			TabNum = enterTab(Str[index-1].name,INT,FUNC,0,FunLen);//��������ֵ��Ϊ0����ַ FunLen
			CurFun_CodeNum = CodeNum;//���µ�ǰ������codenum
			InsTypeName1(Func,Int,Str[index-1].name);//������������м����
			FunLen+=4;
		}
		else if(strcmp(Str[index-2].kind,"charSym")==0)
		{
			FunLen = 0;//����һ���µ�int���������ô˺����ĳ�����0
			TabNum = enterTab(Str[index-1].name,CHAR,FUNC,0,FunLen);//��������ֵ��Ϊ0����ַFunLen
			CurFun_CodeNum = CodeNum;//���µ�ǰ������codenum
			InsTypeName1(Func,Char,Str[index-1].name);//������������м����
			FunLen+=4;
		}
		IntoFun = 1;
		printf("It is a DeclareHead!\n");
	}
	else //ȱ��int��char   13
		Error(13);
}
//<����˵��>		Variable Description	���������壾;{���������壾;}
void VariableDescription()
{
	do{
		VariableDefinition();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a VariableDescription!\n");
		}
		else //ȱ�ֺ�		6
		{
			Error(6);
			if(IntoMain==0)
				Jump(3);	//������һ��int��char��void
			else if(IntoMain==1 && strcmp(Str[index].kind,"Assign")!=0)
				Jump(7);	//������һ��const��int��char������ͷ����,��Ȼ��ʱ�����ܳ���const�ˣ����ǻ��ǲ��ٵ�����һ��case��д��
			else if(IntoMain==1 && strcmp(Str[index].kind,"Assign")==0)
				Jump(8);	//������һ���ֺŵĺ�һ��λ��
		}
	}while((strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0 ) && strcmp(Str[index+2].kind,"L_S_Par")!=0);
}
//<��������>		VariableDefinition	?????   ��ַadr��û��
//���������壾  ::= �����ͱ�ʶ����(����ʶ����|����ʶ������[�����޷�����������]��){,(����ʶ����|����ʶ������[�����޷�����������]��) }
void VariableDefinition()
{
	char TypeTmp[100]={'\0'};
	TypeIdentifier();
	if(ErrorFlag==13)	//ȱ�����ͱ�ʶ��int��char
		Jump(1);	//������һ���ֺ�	������˵����  ::= ���������壾;{���������壾;}
	strcpy(TypeTmp,Str[index-1].kind);
	Identifier();
	if(ErrorFlag==20)	//ȱ�ٱ�����
		Jump(2);	//������һ�����Ż�ֺ�
	if(IntoFun==0)	//��ʾ��û���뺯������ȫ�ֱ�����
		findResult(index-1);//��Ϊȫ�ֱ����;ֲ���������
	else if(IntoFun==1)	//��ʾ���뺯����
		findTmpSame(index-1);
	if(strcmp(Str[index].kind,"L_M_Par")!=0) //��ʾ��<���ͱ�ʶ��><��ʶ��>����int a
	{
		if(strcmp(TypeTmp,"intSym")==0)
		{
			//д���м����
			if(IntoFun==0)//��ʾ��ȫ�ֱ���
			{
				TabNum = enterTab(Str[index-1].name,INT,GVAR,0,adr);
				InsTypeName1(Gvar,Int,Str[index-1].name);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//��ʾ�Ǻ����ڲ��ı�������
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
		else if(strcmp(TypeTmp,"charSym")==0)//��char melon
		{
			//д���м����
			if(IntoFun==0)//��ʾ��ȫ�ֱ���������main����
			{
				TabNum = enterTab(Str[index-1].name,CHAR,GVAR,0,adr);
				InsTypeName1(Gvar,Char,Str[index-1].name);
				adr+=4;
			}
			else if(IntoFun==1 && IntoMain==0)//��ʾ�Ǻ����ڲ��ı�������
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
	else if(strcmp(Str[index].kind,"L_M_Par")==0) //int a[2]  ����
	{
		nextsym();
		UnsignedInteger();
		if(strcmp(Str[index-1].kind,"Num") != 0)	//�Ƿ����鳤��
		{
			Error(19);
			Jump(2);	//������һ�����Ż�ֺ�
		}
		if(strcmp(Str[index].kind,"R_M_Par")==0)
		{
			if(strcmp(TypeTmp,"intSym")==0)
			{
				//д���м����
				if(IntoFun==0)//��ʾ��ȫ�����������main����
				{
					TabNum = enterTab(Str[index-3].name,INT,GARR,(atoi)(Str[index-1].name),adr);
					InsTypeName1Res(Garr,Int,Str[index-3].name,Str[index-1].name);
					adr+=(atoi)(Str[index-1].name)*4;
				}
				else if(IntoFun==1 && IntoMain==0)//��ʾ�Ǻ����ڲ������鶨��
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
				//д���м����
				if(IntoFun==0)//��ʾ��ȫ���������main�������������
				{
					TabNum = enterTab(Str[index-3].name,CHAR,GARR,(atoi)(Str[index-1].name),adr);
					InsTypeName1Res(Garr,Char,Str[index-3].name,Str[index-1].name);
					adr+=(atoi)(Str[index-1].name)*4;
				}
				else if(IntoFun==1 && IntoMain==0)//��ʾ�Ǻ����ڲ������鶨��
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
		else  //ȱ��������	3
		{
			Error(3);
			Jump(2);	//������һ�����Ż�ֺ�
		}
	}
	while(strcmp(Str[index].kind,"Comma")==0)
	{
		nextsym();
		Identifier();
		if(ErrorFlag==20)	//ȱ�ٱ�����
			Jump(2);	//������һ�����Ż�ֺ�
		if(IntoFun==0)	//��ʾ��û���뺯������ȫ�ֳ�����
			findResult(index-1);//��Ϊȫ�ֳ����;ֲ���������
		else if(IntoFun==1)	//��ʾ���뺯����
			findTmpSame(index-1);
		if(strcmp(Str[index].kind,"L_M_Par")!=0) //��ʾ��<���ͱ�ʶ��><��ʶ��>����int a,b
		{
			if(strcmp(TypeTmp,"intSym")==0)
			{
				//д���м����
				if(IntoFun==0)//��ʾ��ȫ�ֱ���
				{
					TabNum = enterTab(Str[index-1].name,INT,GVAR,0,adr);
					InsTypeName1(Gvar,Int,Str[index-1].name);
					adr+=4;
				}
				else if(IntoFun==1 && IntoMain == 0)//��ʾ�Ǻ����ڲ��ı�������
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
				//д���м����
				if(IntoFun==0)//��ʾ��ȫ�ֱ���
				{
					TabNum = enterTab(Str[index-1].name,CHAR,GVAR,0,adr);
					InsTypeName1(Gvar,Char,Str[index-1].name);
					adr+=4;
				}
				else if(IntoFun==1 && IntoMain == 0)//��ʾ�Ǻ����ڲ��ı�������
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
			if(strcmp(Str[index-1].kind,"Num") != 0)	//�Ƿ����鳤��
			{
				Error(19);
				Jump(2);	//������һ�����Ż�ֺ�
			}
			if(strcmp(Str[index].kind,"R_M_Par")==0) //int a,b[2]
			{
				if(strcmp(TypeTmp,"intSym")==0)
				{
					//д���м����
					if(IntoFun==0)//��ʾ��ȫ������
					{
						TabNum = enterTab(Str[index-3].name,INT,GARR,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Garr,Int,Str[index-3].name,Str[index-1].name);
						adr+=(atoi)(Str[index-1].name)*4;
					}
					else if(IntoFun==1 && IntoMain == 0)//��ʾ�Ǻ����ڲ������鶨��
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
					//д���м����
					if(IntoFun==0)//��ʾ��ȫ������
					{
						TabNum = enterTab(Str[index-3].name,CHAR,GARR,(atoi)(Str[index-1].name),adr);
						InsTypeName1Res(Garr,Char,Str[index-3].name,Str[index-1].name);
						adr+=(atoi)(Str[index-1].name)*4;
					}
					else if(IntoFun==1 && IntoMain==0)//��ʾ�Ǻ����ڲ������鶨��
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
			else  //ȱ��������	3
			{
				Error(3);
				Jump(2);	//������һ�����Ż�ֺ�
			}
		}
	}
	printf("It is a VariableDefinition!\n");
}
//<���ͱ�ʶ��>	TypeIdentifier	
void TypeIdentifier()
{
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
	{
		printf("It is a TypeIdentifier!\n");
		nextsym();
	}
	else  //ȱ��int��char		13
		Error(13);
}
//<�з���ֵ��������>		ReturnValueFunctionDefinition
void ReturnValueFunctionDefinition()
{
	char FunName[100];
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
	{
		DeclareHead();
		strcpy(FunName,Str[index-1].name);	//���溯������
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
						Error(15);	//ȱ�ٷ���ֵ
					ReturnFlag=0;//�����Ժ��ʼ��Ϊ0
					if(strcmp(Str[index].kind,"R_B_Par")==0)
					{
						printf("It is a ReturnValueFunctionDefinition!\n");
						IntoFun = 0;//��ʾС��������
						MediaTab[CurFun_CodeNum].FunLen = FunLen;//�Ѵ˺����ĳ��ȸ�����������ı�ʶ�����м����ṹ�����飬Ȼ���ڰ�funlen��Ϊ0
						FunLen=0;
						makeEndLabel();
						InsName1Res(End,EndId,FunName);//�Ѻ�����βд���м����
						enterTab(EndId,VOID,REEND,0,adr);	//һ������main����֮���,ENDд����ű�
						adr+=4;
						nextsym();
					}
					else //ȱ���Ҵ�����	5
					{
						Error(5);
						Jump(3); //������һ��int��char��void
					}
				}
				else  //ȱ���������	4
				{
					Error(4);
					Jump(3); //������һ��int��char��void
				}
			}
			else  //ȱ����С����	2
			{
				Error(2);
				Jump(3); //������һ��int��char��void
			}
		}
		else  //ȱ����С����	1
		{
			Error(1);
			Jump(3); //������һ��int��char��void
		}
	}
}
//<�޷���ֵ��������>		NoReturnValueFunctionDefinition   ��ַadr��û��
//���޷���ֵ�������壾  ::= void����ʶ������(������������)����{����������䣾��}��
void NoReturnValueFunctionDefinition()
{
	char FunName[100];
	if(strcmp(Str[index].kind,"voidSym")==0)
	{
		nextsym();
		Identifier();
		strcpy(FunName,Str[index-1].name);	//���溯������
		CurFun_CodeNum = CodeNum;
		InsTypeName1(Func,Void,Str[index-1].name);//��������д���м����
		findResult(index-1);//���������ܺͷ��ű�����Ԫ������
		FunLen = 0;//����һ���µ�void���������ô˺����ĳ�����0
		Lev++;
		TabNum = enterTab(Str[index-1].name,VOID,FUNC,0,FunLen);//��ַ��FunLen
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
						Error(27);	//ȱ�ٷ���ֵ
					if(strcmp(Str[index].kind,"R_B_Par")==0)
					{
						printf("It is a NoReturnValueFunctionDefinition!\n");
						IntoFun = 0;//��ʾС��������
						MediaTab[CurFun_CodeNum].FunLen = FunLen;//�Ѵ˺����ĳ��ȸ�����������ı�ʶ�����м����ṹ�����飬Ȼ���ڰ�funlen��Ϊ0
						FunLen=0;
						makeEndLabel();
						InsName1Res(End,EndId,FunName);//�Ѻ�����βд���м����
						enterTab(EndId,VOID,NOREEND,0,adr);	//һ������main����֮���
						adr+=4;
						nextsym();
					}
					else //ȱ���Ҵ�����	5
						Error(5);
				}
				else  //ȱ���������	4
					Error(4);
			}
			else  //ȱ����С����	2
				Error(2);
		}
		else  //ȱ����С����	1
			Error(1);
	}
}
//<�������>		CompoundStatement
void CompoundStatement()
{
	if(strcmp(Str[index].kind,"constSym")==0)
		ConstantDescription();
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
		VariableDescription();
	StatementColumn();
	printf("It is a CompoundStatement!\n");
}
//<������>	ParametersTable	 ��ַadr��û��
//��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�
void ParametersTable()
{
	char TypeTmp[100]={'\0'}; 
	int TotalPara=0;
	if(strcmp(Str[index].kind,"intSym")==0 || strcmp(Str[index].kind,"charSym")==0)
	{
		TypeIdentifier();
		strcpy(TypeTmp,Str[index-1].kind);
		Identifier();
		if(ErrorFlag==20)	//ȱ�ٲ�������������һ�����Ż�ֺ�
			Jump(2);
		if(strcmp(TypeTmp,"intSym")==0)
		{
			TabNum = enterTab(Str[index-1].name,INT,PARA,0,FunLen);//�˴�value���迼��,��ַFunLen
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
			if(ErrorFlag==20)	//ȱ�ٲ�������������һ�����Ż�ֺ�
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
	//û�м�else�������Ϊ�����<��>
	else if(strcmp(Str[index].kind,"R_S_Par")==0)
	{
		printf("It is a ParametersTable!\n");//����û��nextsym��Ϊ������ͳһ��nextsym
	}
}
//<������>		MainFunction	  ��ַadr��û��
void MainFunction()
{
	if(strcmp(Str[index].kind,"voidSym")==0)
	{
		nextsym();
		if(strcmp(Str[index].kind,"mainSym")==0)
		{
			Lev++;
			TabNum = enterTab("main",VOID,FUNC,0,adr);
			CurFun_CodeNum = CodeNum;//���µ�ǰ������codenum
			InsTypeName1(Func,Void,Str[index].name);//��ǰָ��main�����������м����
			adr+=4;
			nextsym();
			if(strcmp(Str[index].kind,"L_S_Par")==0)
			{
				nextsym();
				if(strcmp(Str[index].kind,"R_S_Par")==0)
				{
					nextsym();
					IntoFun = 1;//����������
					IntoMain = 1;
					if(strcmp(Str[index].kind,"L_B_Par")==0)
					{
						nextsym();
						CompoundStatement();
						if(strcmp(Str[index].kind,"R_B_Par")==0)
						{
							nextsym(); //����Ӧ���ǳ���Ľ�����
							IntoFun = 0;//�뿪������
							IntoMain = 0; //�������ע�͵�����Ϊ����Ҫ��mainд������ջ
							makeEndLabel();
							InsName1(End,EndId);//�Ѻ�����βд���м����
							enterTab(EndId,VOID,MAINEND,0,adr);	//һ������main����֮���
							adr+=4;
							printf("It is a MainFunction!\n");
						}
						else //ȱ���Ҵ�����	5
							Error(5);
					}
					else  //ȱ���������	4
						Error(4);
				}
				else  //ȱ����С����	2
					Error(2);
			}
			else  //ȱ����С����	1
				Error(1);
		}
		else  //ȱ��main	9
			Error(9);
	}
	else  //ȱ��void	10
		Error(10);
}
//<���ʽ>		Expression	
void Expression()
{
	char plustmp[10]={'\0'},ItemTmp1[1024]={'\0'},ItemTmp2[1024]={'\0'},FirstPlus[10]={'\0'};
	char *NoNeg;//ȥ�����ţ���Ϊ���ָ������������
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
			strcpy(ItemTmp1,ReExpr);//-tmp3(������3)copy��ItemTmp1
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
			strcpy(ReExpr,ReNeg);	//����ٰ�ȡ�����ֵ�������ʽ
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
					//��֤�ַ���ASCII����ʾ��Χ��
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
			else if(strcmp(plustmp,"Minus")==0)  //����� 3---7�������
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
					if(ItemTmp1[1]-ItemTmp2[1]>=32 && ItemTmp1[1]-ItemTmp2[1]<=126)	//��֤�ַ���ASCII����ʾ��Χ��
						IsChar=1;
					else 
						IsChar=0;
				}
				else 
					IsChar=0;
				//��ʱ����¼����ű�
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
//<��>		Item
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
		//��ʱ����¼����ű�
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
//<����>		Factor    ?????????
void Factor()
{
	char idTmp[1024]={'\0'};
	int ischartmp;
	IdentType arrType;
	if(strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0)	//�з���ֵ�ĺ�������
	{
		ReturnValueFunctionCallStatement();
		strcpy(ReFact,ReRetS);//���ַ���ֵ�ĺ����ķ���ֵ��������
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"IdSym")==0)
	{
		Identifier();	//�������������
		if(GraFindTabType(index-1) == IntGra)
		{
			IsChar=0;	//�����int���ͣ�ischar��0
			arrType = INT;
		}
		else if(GraFindTabType(index-1) == CharGra)
		{
			IsChar=1;	//�����int���ͣ�ischar��0
			arrType = CHAR;
		}
		ischartmp = IsChar;
		strcpy(idTmp,Str[index-1].name);//��ʶ�����������ִ���idTmp����
		if(strcmp(Str[index].kind,"L_M_Par")==0)	//��ʾ������
		{
			if(findIdentT_G(idTmp)!=GARR && findIdentT_G(idTmp)!=TARR)	//�����ж��Ƿ�����������
				fprintf(ErrorOut,"����δ���壺��%d��%s\n",Str[index-1].OriLine,Str[index-1].name);
			nextsym();
			Expression();	//�����������±꣬����Ӱ��ischar���жϣ�����жϲ�����
			IsChar=ischartmp;
			makeVar(ReArrG);
			//��ʱ����¼����ű�
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
			InsName12Res(ArrG,idTmp,ReExpr,ReArrG);//ReArrG[100]�������ȡ����ֵ
			strcpy(ReFact,ReArrG);//������ȡ����ֵ����ReFact
			if(strcmp(Str[index].kind,"R_M_Par")==0)
				nextsym();
			else  //ȱ����������	3
				Error(3);
		}
		else  //��ʾ�ǵ�����<��ʶ��>�����Բ���������������Ϊǰ���Ѿ��жϹ���
		{
			if(findIdentT_G(idTmp)==-1 || findIdentT_G(idTmp)==GARR || findIdentT_G(idTmp)==TARR)	//�����ж��Ƿ���������ʶ��
				fprintf(ErrorOut,"��ʶ��δ���壺��%d��%s\n",Str[index-1].OriLine,Str[index-1].name);
			strcpy(ReFact,idTmp);//��ǰ�ı�ʶ������ReFact
		}
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"Num")==0 || strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0) //����
	{
		Integer();
		strcpy(ReFact,ReInt);//��ǰ����������ReFact
		printf("It is a Factor!\n");
	}
	else if(strcmp(Str[index].kind,"CHSym") == 0) //�ַ�
	{
		Chara();
		strcpy(ReFact,Str[index-1].name);//��ǰ���ַ�����ReFact
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
		else  //ȱ��)		2
			Error(2);
	}
}
//<���>		Statement   ????????
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
		else //ȱ��}		5
			Error(5);
	}
	else if(strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0)
	{
		if( FindFuncName(Str[index].name)==INT || FindFuncName(Str[index].name)==CHAR )//���ҵ�ǰ��ʶ��������ĺ�������
			ReturnValueFunctionCallStatement();//�˴���bug���з���ֵ�������޷���ֵ�����޷�����
		else if( FindFuncName(Str[index].name)== VOID)
			NoReturnValueFunctionCallStatement();
		if(strcmp(Str[index].kind,"Semi")==0)
		{
			nextsym();
			printf("It is a Statement!\n");
		}
		else  //ȱ��;		6
		{
			Error(6);	//���е��˴��п�����ȱ�ٷֺţ�Ҳ�п����Ǻ���δ����ֱ�����ã�������������С������û���룬���е��˴����Ǻ�����������û�зֺ�
			Jump(1);	//ȱ�ٷֺţ�������һ���ֺ�
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
		else  //ȱ��;		6
		{
			Error(6);
			Jump(1);	//ȱ�ٷֺţ�������һ���ֺ�
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
		else  //ȱ��;		6
		{
			Error(6);
			Jump(1);	//ȱ�ٷֺţ�������һ���ֺ�
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
		else  //ȱ��;		6
		{
			Error(6);
			Jump(1);	//ȱ�ٷֺţ�������һ���ֺ�
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
		else  //ȱ��;		6
		{
			Error(6);
			Jump(1);	//ȱ�ٷֺţ�������һ���ֺ�
		}
	}
}
//<��ֵ���>		AssignmentStatement
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
	arrlen = findArrLen(index-1);	//���鳤�ȱ�����arrlen����
	strcpy(IdentTmp,Str[index-1].name);//���Ȱѱ�ʶ�����ִ���IdentTmp
	if(strcmp(Str[index].kind,"Assign")==0)//������ֵ
	{
		nextsym();
		Expression();
		if(strcmp(Str[index-1].kind,"Num")==0 || findInTab(ReExpr)==INT )
			Right = INT;
		else if(findInTab(ReExpr)==CHAR)
			Right = CHAR;
		else if(strcmp(Str[index-1].kind,"CHSym")==0)
				Right = Left;
		if(Left == CHAR && Right == INT)	//������ֻ��д�ڱ����ļ����棬�൱�ھ���
			fprintf(ErrorOut,"��%d��%s\t�˴���int��char���͵�ת��\n",Str[index-1].OriLine,Str[index-1].name);
		InsName1Res(Ass,IdentTmp,ReExpr);
		printf("It is a AssignmentStatement!\n");
	}
	else if(strcmp(Str[index].kind,"L_M_Par")==0)//���鸳ֵ
	{
		nextsym();
		Expression();
		if(isDigit(ReExpr[0])!=-1 && atoi(ReExpr)>=arrlen)  //����±�պ������֣������������ж��Ƿ����Խ��;�����±겻����Ϊ��
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
					fprintf(ErrorOut,"��%d��%s\t�˴���int��char���͵�ת��\n",Str[index-1].OriLine,Str[index-1].name);
				strcpy(ReExTmp2,ReExpr);
				InsName12Res(ArrA,IdentTmp,ReExTmp1,ReExTmp2);//���鸳ֵ
				printf("It is a AssignmentStatement!\n");
			}
			else  //ȱ�ٵȺ�		7
			{
				Error(7);
				Jump(1);	//������һ���ֺ�
			}
		}
		else  //ȱ����������		3
		{
			Error(3);
			Jump(1);	//������һ���ֺ�
		}
	}
	else  //�Ƿ���ֵ��Ӧ��ֻ���� = [ ֮һ		21
	{
		Error(21);
		Jump(1);	//������һ���ֺ�
	}
}
//<�������>		ConditionalStatement	�ܽ����������˵���Ѿ��жϹ�if��
void ConditionalStatement()
{
	//ʵ����ǰ�ú����label
	char TmpLabel[100]="label_";
	char LabelAdd[3];
	itoa(LabelNum,LabelAdd,10);//����ת�ַ�������LabelTmp����
	strcat(TmpLabel,LabelAdd);//�ѱ�ǩ�±�ͱ�ǩ��ƴ������
	nextsym();
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Condition();
		InsName1Res(FGoto,ReCond,TmpLabel); //�Ѿ�д��MediaTab��
		LabelNum++;
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			nextsym();
			Statement();
			//enterTab(TmpLabel,VOID,LABEL,0,adr);//�ѵ�ǰ��ǩд����ű�
			fprintf(InterCodeStream,"%s\n",TmpLabel);
			AddLabel(TmpLabel);//��ǩд��MediaTab
			printf("It is a ConditionalStatement!\n");
		}
		else  //ȱ����С����		2
		{
			Error(2);
			Jump(1);	//������һ���ֺ�
		}
	}
	else  //ȱ����С����		1
	{
		Error(1);
		Jump(1);	//������һ���ֺ�
	}
}
//<����>		Condition   ����������
void Condition()
{
	char ExTmp1[1024]={'\0'};
	char ExTmp2[1024]={'\0'};
	char rela[100]={'\0'};
	Expression();
	strcpy(ExTmp1,ReExpr);
	if(strcmp(Str[index].kind,"R_S_Par")!=0) //��ʾ���й�ϵ������ģ����򲻽���˷�֧
	{
		Rela(); //��ϵ�����
		strcpy(rela,Str[index-1].kind);//��ϵ������浽rela
		Expression();
		strcpy(ExTmp2,ReExpr);
		makeVar(ReCond);
		//��ʱ����¼����ű�
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
	else if(strcmp(Str[index].kind,"R_S_Par")==0)//��ʾ<����>ֻ��һ�����ʽ
	{
		makeVar(ReCond);
		//��ʱ����¼����ű�
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
		InsName12Res(Grea,ExTmp1,"0",ReCond);  //��ʾ<���ʽ>Ϊ��
	}
	printf("It is a Condition!\n");
}
//<ѭ�����>		LoopStatement
void LoopStatement()
{
	char TmpLabel[100]="label_";
	LableIntoMTab();//��ӱ�ǩ
	AddLabel(LabelNumber);//��ǩд��MediaTab
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
			InsName1Res(TGoto,ReCond,TmpLabel);//Ϊ����ת
			if(strcmp(Str[index].kind,"R_S_Par")==0)
			{
				printf("It is a LoopStatement!\n");
				nextsym();
			}
			else  //ȱ����С����		2
			{
				Error(2);
				ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
			}
		}
		else  //ȱ����С����		1
		{
			Error(1);
			ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
		}
	}
	else  //ȱ��while		11
	{
		Error(11);
		ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
	}
}
//<����>		Constant
void Constant()
{
	if(strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0 || isDigit(Str[index].name[0])!=-1)
	{
		Integer();
		printf("It is a Constant!\n");
	}
	else if(strcmp(Str[index].kind, "CHSym") == 0) //��������������ܻ���Chara()�б���
	{
		Chara();	
		printf("It is a Constant!\n");
	}
	else  //��������		22
		Error(22);
}
//<������>		SituationStatement
void SituationStatement()
{
	//ʵ����ǰ�ú����label
	char SwitchTmp[100],SwitchPair[100];	//SwitchPair����switch�����
	char LabelAdd[3];
	strcpy(SwitchTmp,"label_");
	itoa(LabelNum,LabelAdd,10);//����ת�ַ�������LabelTmp����
	strcat(SwitchTmp,LabelAdd);//�ѱ�ǩ�±�ͱ�ǩ��ƴ������
	MakeSwitchEndLabel();
	strcpy(SwitchPair,SwitBEid);	//��ǰ��switchend��id ������
	nextsym();
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Expression();  //��Ҫ���жϵı��ʽ
		InsName1Res(Swit,ReExpr,SwitchPair);//���������жϣ��˴��������
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
				else  //ȱ���Ҵ�����	5
				{
					Error(5);
					ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
				}
			}
			else  //ȱ���������	4
			{
				Error(4);
				ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
			}
		}
		else  //ȱ����С����	2
		{
			Error(2);
			ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
		}
	}
	else  //ȱ����С����	1
	{
		Error(1);
		ErrorFlag=0;	//������������Ϊ�Ҳ�֪����һֱ����������
	}
}
//<�����>		SituationTable	��һ��������case����
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
	else  //ȱ��case		12
	{
		Error(12);
		Jump(4);	//������һ�������ţ��뿪switch
	}
}
//<��������>		SituationSubSentence
void SituationSubSentence(char tmp[])
{
	nextsym();
	Constant();
	AddCaseLabel(index);
	if(strcmp(Str[index].kind,"Colon")==0)
	{
		nextsym();
		Statement();
		InsName1(Goto,tmp);//һ��case�����Ժ����ת��switch��β��
		printf("It is a SituationSubSentence!\n");
	}
	else  //ȱ��ð��		8
		Error(8);
}
//<�з���ֵ�����������>		ReturnValueFunctionCallStatement
void ReturnValueFunctionCallStatement()
{
	char FuncName[100]={'\0'};
	IdentType funcType;
	if(GraFindTabObj(index)!=FUNC)	//�����ж��Ƿ�����������
		fprintf(ErrorOut,"�з���ֵ�ĺ��������к���δ���壺��%d��%s\n",Str[index].OriLine,Str[index].name);
	Identifier();	//������
	if(ErrorFlag==20)
		Jump(1);	//���û�кϷ�����������������һ���ֺ�
	if(GraFindTabType(index-1)==IntGra)	//�麯��������
	{
		IsChar=0;
		funcType = INT;
	}
	else if(GraFindTabType(index-1)==CharGra)	//�麯��������
	{
		IsChar=1;
		funcType = CHAR;
	}
	else
		Error(30);
	strcpy(FuncName,Str[index-1].name);//�ȰѺ�������������
	InsName1(CallBegin,FuncName);
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		ValueParameterTable(FuncName);
		//д���м����
		InsName1(CallEnd,FuncName);
		makeVar(ReRetS);
		//��ʱ����¼����ű�
		if(IntoMain==0)
		{
			if(funcType == INT)	//�麯��������
				enterTab(ReRetS,INT,TTVAR,0,FunLen);
			else if(funcType == CHAR)	//�麯��������
				enterTab(ReRetS,CHAR,TTVAR,0,FunLen);
			FunLen+=4;
		}
		else if(IntoMain==1)
		{
			if(funcType == INT)	//�麯��������
				enterTab(ReRetS,INT,GTVAR,0,adr);
			else if(funcType == CHAR)	//�麯��������
				enterTab(ReRetS,CHAR,GTVAR,0,adr);
			adr+=4;
		}
		if(funcType == INT)	//�麯��������
			InsTypeName1(RetS,Int,ReRetS);	//���巵��ֵ����
		else if(funcType == CHAR)	//�麯��������
			InsTypeName1(RetS,Char,ReRetS);	//���巵��ֵ����
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			printf("It is a ReturnValueFunctionCallStatement!\n");
			nextsym();
		}
		else  //ȱ����С����		2
			Error(2);
	}
	else  //ȱ����С����		1
		Error(1);
}
//<�޷���ֵ�����������>		NoReturnValueFunctionCallStatement
void NoReturnValueFunctionCallStatement()
{
	char FuncName[100]={'\0'};
	Identifier();
	if(ErrorFlag==20)
		Jump(1);	//���û�кϷ�����������������һ���ֺ�
	strcpy(FuncName,Str[index-1].name);//�ȰѺ�������������
	InsName1(CallBegin,FuncName);
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		ValueParameterTable(FuncName);  //�Ѳ���Push��ȥ�ͺã������ڷ���ֵ��
		//д���м����
		InsName1(CallEnd,FuncName);
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			printf("It is a NoReturnValueFunctionCallStatement!\n");
			nextsym();
		}
		else  //ȱ����С����		2
		{
			Error(2);
			Jump(1);	//������һ���ֺ�
		}
	}
	else  //ȱ����С����		1
	{
		Error(1);
		Jump(1);	//������һ���ֺ�
	}
}
//<ֵ������>		ValueParameterTable  ����<��>�����
void ValueParameterTable(char fun[])
{
	char ParaName[100]="PARA";
	char tmp[100]={'\0'};
	int TmpParaNum;
	ParaNum=1;	//��ʼ��Ϊ1
	if(strcmp(Str[index].kind,"R_S_Par")!=0) //����ǿյ�ֵ�����������ͱ��������ķ�֧��
	{
		TmpParaNum = ParaNum;
		Expression();
		ParaNum = TmpParaNum;
		itoa(ParaNum,tmp,10);	
		strcat(ParaName,tmp);
		InsName12Res(Push,ReExpr,ParaName,fun);
		strcpy(ParaName,"PARA");	//����Ժ�PARA���±�����
		memset(tmp,'\0',100);	//tmp����
		ParaNum++;
		while(strcmp(Str[index].kind,"Comma")==0)
		{
			nextsym();
			TmpParaNum = ParaNum;
			Expression();
			ParaNum = TmpParaNum;
			itoa(ParaNum,tmp,10);	
			strcat(ParaName,tmp);
			InsName12Res(Push,ReExpr,ParaName,fun);	//Name1��Ҫpush�Ĳ���	Name2��ʾ��������в������±꣨��0��ʼ�� Result��Ҫpush���ĸ�����
			strcpy(ParaName,"PARA");	//����Ժ�PARA���±�����
			memset(tmp,'\0',100);	//tmp����
			ParaNum++;
		}
	}
	printf("It is a ValueParameterTable!\n");
}
//<�����>		StatementColumn   ?????
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
//<�����>	ReadStatement	
void ReadSentences()
{
	if(strcmp(Str[index].kind,"scanfSym")==0)
	{
		nextsym();
		if(strcmp(Str[index].kind,"L_S_Par")==0)
		{
			nextsym();
			Identifier();
			InsName1(Scan,Str[index-1].name);//�Ѷ���ʶ��д���м����
			while(strcmp(Str[index].kind,"Comma")==0)
			{
				nextsym();
				Identifier();
				InsName1(Scan,Str[index-1].name);//�Ѷ���ʶ��д���м����
			}
			if(strcmp(Str[index].kind,"R_S_Par")==0)
			{
				printf("It is a ReadSentences!\n");
				nextsym();
			}
			else  //ȱ����С����		2
				Error(2);
		}
		else  //ȱ����С����		1
			Error(1);
	}
	else  //ȱ��scanf		17
		Error(17);
}
//<д���>		WriteStatement   ?????
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
				strcpy(tmpStr,Str[index-1].name);	//��ʱ���浱ǰ�ַ���
				if(strcmp(Str[index].kind,"Comma")==0)
				{
					nextsym();
					Expression();
					if(ReExpr[0]=='\'')
						IsChar=1;
					InsName1(Prin,tmpStr);	//������Ԫʽ˳�򣬱�֤��������ʽ������ַ���
					InsName1(Prin,ReExpr);
					IsChar=0;
					if(strcmp(Str[index].kind,"R_S_Par")==0)
					{
						printf("It is a WriteStatement!\n");
						nextsym();
					}
					else  //ȱ����С����		2
						Error(2);
				}
				else if(strcmp(Str[index].kind,"R_S_Par")==0)
				{
					InsName1(Prin,tmpStr);
					printf("It is a WriteStatement!\n");
					nextsym();
				}
				else  //ȱ����С����		2
					Error(2);
			}
			else if(strcmp(Str[index].kind,"Plus")==0 || strcmp(Str[index].kind,"Minus")==0||strcmp(Str[index].kind,"IdSym")==0
			||strcmp(Str[index].kind,"Num")==0||strcmp(Str[index].kind,"CHSym")==0||strcmp(Str[index].kind,"L_S_Par")==0)  //���ʽfirst����
			{
				Expression();
				if(ReExpr[0]=='\'')
					IsChar=1;
				InsName1(Prin,ReExpr);
				IsChar=0;	//�����Ժ��ʼ��Ϊ-1
				if(strcmp(Str[index].kind,"R_S_Par")==0)
				{
					printf("It is a WriteStatement!\n");
					nextsym();
				}
				else  //ȱ����С����		2
					Error(2);
			}
		}
		else  //ȱ����С����		1
			Error(1);
	}
	else  //ȱ��printf		18
		Error(18);
}
//<�������>		ReturnStatement	�ܽ��������˵��return�Ѿ��жϹ���
void ReturnStatement()
{
	nextsym();
	if(strcmp(Str[index].kind,"L_S_Par")==0)
	{
		nextsym();
		Expression();
		if(strcmp(Str[index-1].kind,"Num")!=0 && strcmp(Str[index-1].kind,"CHSym")!=0 && findDef(ReExpr)==-1)
			Error(27);	//�ڷ���ֵ�������ֻ��ַ���ǰ����ȥ����ű�û�ҵ��򱨴��Ƿ�����ֵ
		InsName1(Ret,ReExpr);	//Ret����д����ű�
		if(strcmp(Str[index].kind,"R_S_Par")==0)
		{
			printf("It is a ReturnStatement!\n");
			nextsym();
		}
		else  //ȱ����С����		2
			Error(2);
	}
	else  //return ���治��()Ҳ�����ķ�
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
int findIdentT_G(char name[])	//�ȴӵ�ǰ��ο�ʼ�ң�����Ҳ�������ȫ��
{
	int i = 0,n=0,u=0;
	while(Tab[n].lev != Lev)	//�ȴӵ�ǰ��ο�ʼ��
		n++;
	if(Tab[n].lev == Lev)	//�ȴӵ�ǰ��ο�ʼ��
	{
		for(i = n;Tab[i].lev == Lev && i<TabNum; i++)	
			if(strcmp(name,Tab[i].name)==0)//��ʾ�ڸò���ҵ���
				return Tab[i].obj;
	}
	//�������е���˵��û�ҵ�
	while(Tab[u].lev==0)
	{
		if(strcmp(name,Tab[u].name)==0)//��ʾ��ȫ���ҵ���
			return Tab[u].obj;
		else
			u++;
	}
	return -1;	//���е���˵��û�ҵ�
}
int findInTab(char name[])
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(stricmp(name,Tab[i].name)==0)
			return Tab[i].type;
	}
	return -1; //û�ҵ�
}
int main(){
	FILE *TabToTxt;
	int pu=0;
	WordAnalysis();
	printf("�﷨�������ֿ�ʼ\n");
	count = CopyIntoStruct()-1;
	index = 0;
	ErrorOut = fopen("������Ϣ.txt", "w+");	//�����ӡ��txt����
	Program();
	printf("�﷨�������ֽ���\n");
	AddParaNumToFun();
	TabToTxt = fopen("Tab.txt","w+");
	fprintf(TabToTxt,"�±�\t����\t\t��ַ\t����\t����\t����\tֵ\t�θ���\n");
	fprintf(TabToTxt,"----------------------------------------------------------------------\n");
	for(pu=0;pu<TabNum;pu++)
		fprintf(TabToTxt,"%d\t%s\t\t%d\t%d\t%s\t%s\t%d\t%d\n",pu,Tab[pu].name,Tab[pu].adrFirst,Tab[pu].lev,PrinObj[Tab[pu].obj],PrinType[Tab[pu].type],Tab[pu].value,Tab[pu].AllParaNum);
	fclose(ErrorOut);
	fclose(TabToTxt);
	//if(ErrorNum==0)
	//{
	//ȫ�ֱ�����ȫ���������ȫ�ֽṹ������
	IntoRunStack();
	CalInStack();
	printf("Ŀ�����������\n");
	//}
	return 0;
}