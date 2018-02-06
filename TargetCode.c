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

FILE * TargetCodeStream = NULL;//����ļ�
FILE * TargetInStream = NULL;//���м�����ļ�

int strFlag=0;
int FindStrId=0;
int LineAdd=0;
int RunPosi;
int RunIndex=0;
int switId=0;	//switch���ж������ĸ���
int switCaseId=0;
char StrIdRes[100]="Str";//Prin�������Str�±�Ľ�����������
char *PushReg[100] = {"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7"};
int para_index=0;
int PushId=0;
/*����ǰ�ķ��ű�����������һ�м������Ĳ�������*/
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
/*ȫ�ֲ����Լ�main�����ڵĳ�����������ṹ������RunStack*/
void IntoRunStack()
{
	int GvarId=0;
	for(GvarId=0;GvarId<TabNum;GvarId++)
	{
		if(Tab[GvarId].obj==GVAR || Tab[GvarId].obj==GARR || Tab[GvarId].obj==GCON)//ȫ�ֲ��ִ�������ջ
		{
			strcpy(RunStack[RunIndex].name,Tab[GvarId].name);
			RunStack[RunIndex].RunPosi=Tab[GvarId].adrFirst;//��ʼ��ַ���ȥ
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
			RunStack[RunIndex].RunPosi=Tab[GvarId].adrFirst;//��ʼ��ַ���ȥ
			RunStack[RunIndex].RunType = Tab[GvarId].type;
			RunStack[RunIndex].RunObj = Tab[GvarId].obj;
			RunIndex++;
		}
	}
	RunPosi = RunStack[RunIndex-1].RunPosi;	
}
/*���м���룬����Ŀ�����*/
void CalInStack()
{
	int Code_i=1;
	TargetCodeStream = fopen("TargetCode.txt","w");
	StrDef();
	fprintf(TargetCodeStream,"LineBreaks:\t.asciiz\t\"\\n\" \n");//����
	fprintf(TargetCodeStream,".text\n");//����
	for(Code_i=1;Code_i<CodeNum;Code_i++)
	{
		if(MediaTab[Code_i].ins == Gcon || MediaTab[Code_i].ins == Gvar || MediaTab[Code_i].ins == Garr)
			DataDef(Code_i);	
		/*����main����*/
		else if(MediaTab[Code_i].ins == Func && strcmp(MediaTab[Code_i].name1,"main")!=0)
		{
			fprintf(TargetCodeStream,".text\nj main\n%s:\n",MediaTab[Code_i].name1);//��mars������������ȫ��תСд
			fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",-1*MediaTab[Code_i].FunLen);	//���뺯��ʱ�ȼ�ȥ���Ŀռ�
			fprintf(TargetCodeStream,"sw\t$ra\t%d($sp)\n",MediaTab[Code_i].FunLen);
			while(MediaTab[Code_i].ins != End)
			{
				Code_i++;
				if(MediaTab[Code_i].ins == Tcon)//��ʱ����
					TmpDataDef(Code_i,NotMain);
				else if(MediaTab[Code_i].ins == Tvar || MediaTab[Code_i].ins == Tarr ||MediaTab[Code_i].ins == Para)
					continue;
				/*�Ƚ����㶨��*/
				else if(MediaTab[Code_i].ins == Grea || MediaTab[Code_i].ins == GaE ||MediaTab[Code_i].ins == Less ||
					    MediaTab[Code_i].ins == LaE || MediaTab[Code_i].ins == NaE ||MediaTab[Code_i].ins == Equ)  
					CompareOp(Code_i);
				/*Ϊ����ת*/
				else if(MediaTab[Code_i].ins == TGoto)	//��ת����ǩ����ǩ����תСд
					fprintf(TargetCodeStream,"beq\t$t2\t1\t%s\n",MediaTab[Code_i].result);//��name1����name2��$t2��Ϊ1,$t2ͳһ��űȽϽ��
				/*��������������ת����if*/
				else if(MediaTab[Code_i].ins == FGoto)	//��ת����ǩ����ǩ����תСд
					fprintf(TargetCodeStream,"bne\t$t2\t1\t%s\n",MediaTab[Code_i].result);//��name1����name2��$t2��Ϊ1,$t2ͳһ��űȽϽ��
				/*ֱ����ת*/
				else if(MediaTab[Code_i].ins == Goto)
					fprintf(TargetCodeStream,"j\t%s\n",MediaTab[Code_i].name1);//��name1����name2��$t2��Ϊ1,$t2ͳһ��űȽϽ��
				/*��ǩ����*/
				else if(MediaTab[Code_i].ins == ThisLabel && MediaTab[Code_i].type == Label)
					fprintf(TargetCodeStream,"%s:\n",MediaTab[Code_i].name1);
				/*�������ÿ�ͷ*/
				else if(MediaTab[Code_i].ins == CallBegin)
					ParaIntoStack(Code_i);
				/*�����в�����������*/
				else if(MediaTab[Code_i].ins == Push)
					Code_i = CallParaFun(Code_i);
				/*�����޲�����������*/
				else if(MediaTab[Code_i].ins == CallEnd)
					CallNoParaFun(Code_i);
				/*д��䶨��*/
				else if(MediaTab[Code_i].ins == Prin)
					PrinStrs(Code_i);
				/*����䶨��*/
				else if(MediaTab[Code_i].ins == Scan)
					ScanIdent(Code_i);
				/*�Ӽ��˳���ȡ������*/
				else if(MediaTab[Code_i].ins == Add ||MediaTab[Code_i].ins == Min ||MediaTab[Code_i].ins == Mul ||
						MediaTab[Code_i].ins == Div ||MediaTab[Code_i].ins == Neg)
					Arithmetic(Code_i);
				/*��ֵ��䶨��*/
				else if(MediaTab[Code_i].ins == Ass)
					AssDef(Code_i);//��ʾ����main����
				/*������䶨��*/
				else if(MediaTab[Code_i].ins == Ret)
					RetDef(Code_i);
				/*�޷���ֵ�ķ������*/
				else if(MediaTab[Code_i].ins == NRet)
					NRetDef(Code_i);
				/*������䶨��*/
				else if(MediaTab[Code_i].ins == End)
					ReEndDef(Code_i);
				/*������ֵ��䶨��*/
				else if(MediaTab[Code_i].ins == RetS)
					RetsDef(Code_i);
				/*�����鸳ֵ����*/
				else if(MediaTab[Code_i].ins == ArrA)
					ArrADef(Code_i);
				/*������ȡֵ����*/
				else if(MediaTab[Code_i].ins == ArrG)
					ArrGDef(Code_i);
				/*�����䶨��*/
				else if(MediaTab[Code_i].ins == Swit)
					SwitDef(Code_i);
			}
		}
		else if(MediaTab[Code_i].ins == Func && strcmp(MediaTab[Code_i].name1,"main")==0)//��main����
		{
			fprintf(TargetCodeStream,".text\nmain:\n");
			while(MediaTab[Code_i].ins != End)
			{
				Code_i++;
				if(MediaTab[Code_i].ins == Tcon)
					TmpDataDef(Code_i,JustInMain);
				else if(MediaTab[Code_i].ins == Tvar || MediaTab[Code_i].ins == Tarr)
					continue;
				/*�Ƚ����㶨��*/
				else if(MediaTab[Code_i].ins == Grea || MediaTab[Code_i].ins == GaE ||MediaTab[Code_i].ins == Less ||
					    MediaTab[Code_i].ins == LaE || MediaTab[Code_i].ins == NaE ||MediaTab[Code_i].ins == Equ)
					CompareOp(Code_i);
				/*Ϊ����ת*/
				else if(MediaTab[Code_i].ins == TGoto)
					fprintf(TargetCodeStream,"beq\t$t2\t1\t%s\n",MediaTab[Code_i].result);//��name1����name2��$t2��Ϊ1,$t2ͳһ��űȽϽ��
				/*��������������ת����if*/
				else if(MediaTab[Code_i].ins == FGoto)
					fprintf(TargetCodeStream,"bne\t$t2\t1\t%s\n",MediaTab[Code_i].result);//��name1����name2��$t2��Ϊ1,$t2ͳһ��űȽϽ��
				/*ֱ����ת*/
				else if(MediaTab[Code_i].ins == Goto)
					fprintf(TargetCodeStream,"j\t%s\n",MediaTab[Code_i].name1);//��name1����name2��$t2��Ϊ1,$t2ͳһ��űȽϽ��
				/*��ǩ����*/
				else if(MediaTab[Code_i].ins == ThisLabel && MediaTab[Code_i].type == Label)
					fprintf(TargetCodeStream,"%s:\n",MediaTab[Code_i].name1);
				/*�������ÿ�ͷ*/
				else if(MediaTab[Code_i].ins == CallBegin)
					ParaIntoStack(Code_i);
				/*�����в�����������*/
				else if(MediaTab[Code_i].ins == Push)
					Code_i = CallParaFun(Code_i);
				/*�����޲�����������*/
				else if(MediaTab[Code_i].ins == CallEnd)
					CallNoParaFun(Code_i);
				/*д��䶨��*/
				else if(MediaTab[Code_i].ins == Prin)
					PrinStrs(Code_i);
				/*����䶨��*/
				else if(MediaTab[Code_i].ins == Scan)
					ScanIdent(Code_i);
				/*�Ӽ��˳���ȡ������*/
				else if(MediaTab[Code_i].ins == Add ||MediaTab[Code_i].ins == Min ||MediaTab[Code_i].ins == Mul ||
						MediaTab[Code_i].ins == Div ||MediaTab[Code_i].ins == Neg)
					Arithmetic(Code_i);
				/*��ֵ��䶨��*/
				else if(MediaTab[Code_i].ins == Ass)
					AssDef(Code_i);//��ʾ��main����
				/*�޷���ֵ�ķ������*/
				else if(MediaTab[Code_i].ins == NRet)	//main������return;
					NRetDef(Code_i);
				/*������䶨��*/
				else if(MediaTab[Code_i].ins == End)	//��main���end��ǩ����������return;��ʱ����ת
					MainEndDef(Code_i);
				/*������ֵ��䶨��*/
				else if(MediaTab[Code_i].ins == RetS)
					RetsDef(Code_i);
				/*�����鸳ֵ����*/
				else if(MediaTab[Code_i].ins == ArrA)
					ArrADef(Code_i);
				/*������ȡֵ����*/
				else if(MediaTab[Code_i].ins == ArrG)
					ArrGDef(Code_i);
				/*�����䶨��*/
				else if(MediaTab[Code_i].ins == Swit)
					SwitDef(Code_i);
			}
			fprintf(TargetCodeStream,"li\t$v0\t10\nsyscall\n");	//�������
		}
	}
}
/*store data*/
void StoreData(int i_StoreData,char Reg[5],LoadWhat S_StoreData)
{
	JudgeMain c;
	char CurIdent[100];
	//���浱ǰ���жϵı�ʶ����CurIdent
	if(S_StoreData == Name1)
		strcpy(CurIdent,MediaTab[i_StoreData].name1);	
	else if(S_StoreData == Name2)
		strcpy(CurIdent,MediaTab[i_StoreData].name2);	
	else if(S_StoreData == Result)
		strcpy(CurIdent,MediaTab[i_StoreData].result);	
	c = FindIdentLoc(i_StoreData,S_StoreData);	//notmain�����ڷ�main������
	if(c == NotMain)	//��main������
		fprintf(TargetCodeStream,"sw\t%s\t%d($sp)\n",Reg,FindTmpIdentAdr(i_StoreData,S_StoreData,Radfirst));
	else if(c == IsMain)	//ȫ��
		fprintf(TargetCodeStream,"sw\t%s\t%d($gp)\n",Reg,FindAdr(i_StoreData,S_StoreData,Radfirst));//name1	ȫ��
	else if(c == JustInMain)	//main������
		fprintf(TargetCodeStream,"sw\t%s\t%d($gp)\n",Reg,FindTmpIdentAdr(i_StoreData,S_StoreData,Radfirst));//name1	main�����ڵĸ�ֵ���
}
/*load data*/	//�˴���name[]Ҫע����name1����name2����result
void LoadData(int i_LoadData,char Reg[5],LoadWhat l_LoadData,char name[])
{
	JudgeMain c;
	if(isDigit(name[0])!=-1||name[0]=='+'||name[0]=='-')//������
			fprintf(TargetCodeStream,"li\t%s\t%d\n",Reg,(atoi)(name));//name1
	else if(name[0]=='\'')//���ַ�
			fprintf(TargetCodeStream,"li\t%s\t%d\n",Reg,name[1]);//name1
	else
	{
		c = FindIdentLoc(i_LoadData,l_LoadData);	/*�жϱ�ʶ����ȫ�ֻ��Ǿֲ�*/
		if(c == IsMain)	//ȫ��
			fprintf(TargetCodeStream,"lw\t%s\t%d($gp)\n",Reg,FindAdr(i_LoadData,l_LoadData,Radfirst));
		else if(c == JustInMain) //main�����Ĳ���
			fprintf(TargetCodeStream,"lw\t%s\t%d($gp)\n",Reg,FindTmpIdentAdr(i_LoadData,l_LoadData,Radfirst));
		else if(c == NotMain)	//��main������
			fprintf(TargetCodeStream,"lw\t%s\t%d($sp)\n",Reg,FindTmpIdentAdr(i_LoadData,l_LoadData,Radfirst));
	}
}
/*ȫ�����ݶ�.data�Ķ���*/	//�Ѿ����  ȫ����Сд
void DataDef(int i_DataDef)
{
	if(MediaTab[i_DataDef].ins == Gcon)	//ȫ�ֳ���
	{
		if(MediaTab[i_DataDef].type == Int)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",atoi(MediaTab[i_DataDef].result));
		else if(MediaTab[i_DataDef].type == Char)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",MediaTab[i_DataDef].result[1]);
		fprintf(TargetCodeStream,"sw\t$t0\t%d($gp)\n",FindAdr(i_DataDef,Name1,Radfirst));
	}
}
///*��ʱ��������*/	//�Ѿ����
void TmpDataDef(int i_TmpDataDef,JudgeMain ifMain)
{
	fprintf(TargetCodeStream,"#��ʱ��������\n"); //���ݶ�
	if(MediaTab[i_TmpDataDef].ins == Tcon)//��ʱ��������
	{
		if(MediaTab[i_TmpDataDef].type == Int)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",atoi(MediaTab[i_TmpDataDef].result));
		else if(MediaTab[i_TmpDataDef].type == Char)	
			fprintf(TargetCodeStream,"li\t$t0\t%d\n",MediaTab[i_TmpDataDef].result[1]);
	}
	if(ifMain == NotMain)	//��main������
		fprintf(TargetCodeStream,"sw\t$t0\t%d($sp)\n",FindTmpIdentAdr(i_TmpDataDef,Name1,Radfirst));
	else if(ifMain == JustInMain)	//main������
		fprintf(TargetCodeStream,"sw\t$t0\t%d($gp)\n",FindAdr(i_TmpDataDef,Name1,Radfirst));
}
/*�ַ���.data�ζ���*/
void StrDef()
{
	int strIndex=0,StrId=0;
	char Strname[100]="Str";
	char StrTmp[3];
	//�ַ�������
	if(strFlag==0)
	{
		fprintf(TargetCodeStream,"#���ݶ�\n"); //���ݶ�
		fprintf(TargetCodeStream,".data\n"); //���ݶ�
		for(strIndex=0;strIndex<count;strIndex++)
		{
			if(strcmp(Str[strIndex].kind,"StrSym")==0)  //���ַ���������
			{
				itoa(StrId,StrTmp,10);//����ת�ַ�������StrTmp����
				strcat(Strname,StrTmp);//�ѱ�ǩ�±�ͱ�ǩ��ƴ������
				fprintf(TargetCodeStream,"%s:\t.asciiz\t%s\n",Strname,Str[strIndex].name);
				strcpy(Strname,"Str");
				StrId++;
			}
		}
		strFlag=1;
	}
}
/*��ֵ���*/
void AssDef(int i_AssDef)	//Ass j tmp5
{
	fprintf(TargetCodeStream,"#��ֵ���\n");
	LoadData(i_AssDef,"$t0",Result,MediaTab[i_AssDef].result);	//��loaddata������תСд
	StoreData(i_AssDef,"$t0",Name1);	//��storedata������תСд
}
/*�з���ֵ�ķ������*/
void RetDef(int i_RetDef)
{
	int m=i_RetDef;
	while(MediaTab[m].ins!=End)
		m++;
	fprintf(TargetCodeStream,"#�������\n");
	LoadData(i_RetDef,"$t0",Name1,MediaTab[i_RetDef].name1);
	fprintf(TargetCodeStream,"addi\t$v0\t$t0\t0\n");//����ֵ����$v0
	if(MediaTab[m].ins==End)
		fprintf(TargetCodeStream,"j\t%s\n",MediaTab[m].name1);//endid��name1
}
/*�޷���ֵ�ķ������*/
void NRetDef(int i_NRetDef)
{
	int m=i_NRetDef;
	while(MediaTab[m].ins!=End)
		m++;
	fprintf(TargetCodeStream,"#�������\n");
	if(MediaTab[m].ins==End)
		fprintf(TargetCodeStream,"j\t%s\n",MediaTab[m].name1);//endid��name1
}
/*�������*/	//End End1518_0 fun
void ReEndDef(int i_EndDef)
{
	int i=0,funlength=0;
	fprintf(TargetCodeStream,"#�������\n");
	fprintf(TargetCodeStream,"%s:\n",MediaTab[i_EndDef].name1);//���End1518_0
	//fprintf(TargetCodeStream,"addi\t$v0\t$v0\t0\n");//���$v0
	for(i=i_EndDef;i>=0;i--)	//���ص���ȥ�Ҵ˺��������������������ĳ���
	{
		if(MediaTab[i].ins == Func && strcmp(MediaTab[i].name1,MediaTab[i_EndDef].result)==0)
		{
			funlength = MediaTab[i].FunLen;
			break;
		}
	}
	fprintf(TargetCodeStream,"#ra����\nlw\t$ra\t%d($sp)\n",funlength);//��ȡ��ra��ȡsp
	fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",funlength);//ȡ��sp
	fprintf(TargetCodeStream,"jr\t$ra\n");
}
/*main����������䣬���ü��ط���ֵ��ֱ�Ӽ�end��ǩ�ͺ�*/
void MainEndDef(int i_MainEndDef)
{
	fprintf(TargetCodeStream,"#main�������\n");
	fprintf(TargetCodeStream,"%s:\n",MediaTab[i_MainEndDef].name1);//���End1518_0
}
/*������ֵ���*/
void RetsDef(int i_RetsDef)	//RetS tmp6	ȡ������ֵv0
{
	fprintf(TargetCodeStream,"#������ֵ���\n");
	StoreData(i_RetsDef,"$v0",Name1);
}
/*�����鸳ֵ����*/
void ArrADef(int i_ArrADef)
{
	JudgeMain c;
	fprintf(TargetCodeStream,"#�����鸳ֵ\n");
	c = FindIdentLoc(i_ArrADef,Name1);	/*�ж�������ȫ�ֻ��Ǿֲ�*/
	if(c == NotMain && FindTmpIdentAdr(i_ArrADef,Name1,Robj) == TARR)	//��main������
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindTmpIdentAdr(i_ArrADef,Name1,Radfirst));	//t0����������ַ
		LoadData(i_ArrADef,"$t1",Name2,MediaTab[i_ArrADef].name2);	//�����±����$t1
		LoadData(i_ArrADef,"$t2",Result,MediaTab[i_ArrADef].result);	//Ҫ���������ֵ����$t2
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//�����±����4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//�������sp��ƫ�ƴ���t0
		fprintf(TargetCodeStream,"addu\t$sp\t$sp\t$t0\n");	//�ٰ�ƫ�Ƶ�sp�ӻ���
		fprintf(TargetCodeStream,"sw\t$t2\t0($sp)\n");	
		fprintf(TargetCodeStream,"subu\t$sp\t$sp\t$t0\n");	
	}
	else if( (c == IsMain && FindAdr(i_ArrADef,Name1,Robj)==GARR) || 
		     (c==JustInMain && FindTmpIdentAdr(i_ArrADef,Name1,Robj)==TARR) )	//ȫ�ֻ�main������
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindAdr(i_ArrADef,Name1,Radfirst));//name1	
		LoadData(i_ArrADef,"$t1",Name2,MediaTab[i_ArrADef].name2);	//�����±����$t1
		LoadData(i_ArrADef,"$t2",Result,MediaTab[i_ArrADef].result);	//Ҫ���������ֵ����$t2
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//�����±����4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//�������sp��ƫ�ƴ���t0
		fprintf(TargetCodeStream,"addu\t$gp\t$gp\t$t0\n");	//�ٰ�ƫ�Ƶ�sp�ӻ���
		fprintf(TargetCodeStream,"sw\t$t2\t0($gp)\n");	
		fprintf(TargetCodeStream,"subu\t$gp\t$gp\t$t0\n");	
	}
}
/*������ȡֵ����*/
void ArrGDef(int i_ArrGDef)
{
	JudgeMain c;
	fprintf(TargetCodeStream,"#������ȡֵ\n");
	c = FindIdentLoc(i_ArrGDef,Name1);	/*�ж�������ȫ�ֻ��Ǿֲ�*/
	if(c == NotMain && FindTmpIdentAdr(i_ArrGDef,Name1,Robj) == TARR)	//��main�����ڵľֲ�����
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindTmpIdentAdr(i_ArrGDef,Name1,Radfirst));
		LoadData(i_ArrGDef,"$t1",Name2,MediaTab[i_ArrGDef].name2);	//�����±����$t1
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//�����±����4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//�������fp��ƫ�ƴ���t0
		fprintf(TargetCodeStream,"addu\t$sp\t$sp\t$t0\n");	//�ٰ�ƫ�Ƶ�sp�ӻ���
		fprintf(TargetCodeStream,"lw\t$t2\t0($sp)\n");	//������ȡֵ����$t2
		fprintf(TargetCodeStream,"subu\t$sp\t$sp\t$t0\n");	//�����ַ
		StoreData(i_ArrGDef,"$t2",Result);
	}
	else if((c == IsMain && FindAdr(i_ArrGDef,Name1,Robj)==GARR) ||		//ȫ������
			(c==JustInMain && FindTmpIdentAdr(i_ArrGDef,Name1,Robj)==TARR) )	//main�ڵľֲ�����
	{
		fprintf(TargetCodeStream,"la\t$t0\t%d\n",FindAdr(i_ArrGDef,Name1,Radfirst));//name1	
		LoadData(i_ArrGDef,"$t1",Name2,MediaTab[i_ArrGDef].name2);	//�����±����$t1
		fprintf(TargetCodeStream,"sll\t$t1\t$t1\t2\n");	//�����±����4
		fprintf(TargetCodeStream,"add\t$t0\t$t1\t$t0\n");	//�������sp��ƫ�ƴ���t0
		fprintf(TargetCodeStream,"addu\t$gp\t$gp\t$t0\n");	//�ٰ�ƫ�Ƶ�sp�ӻ���
		fprintf(TargetCodeStream,"lw\t$t2\t0($gp)\n");	
		fprintf(TargetCodeStream,"subu\t$gp\t$gp\t$t0\n");	
		StoreData(i_ArrGDef,"$t2",Result);
	}
}
/*�����䶨��*/
int SwitDef(int i_SwitDef)
{
	int tmp;
	LoadData(i_SwitDef,"$t0",Name1,MediaTab[i_SwitDef].name1);	//ȡ�����ʽ��ֵ�浽$t0
	tmp = WhichCase(i_SwitDef);	//��case����ĳ����Լ����Եı�ǩ¼��SwitCase�ṹ������
	return tmp;
}
/*д���*/
void PrinStrs(int i_PrinStr)
{
	JudgeMain c;
	if(MediaTab[i_PrinStr].name1[0]=='\"')	
	{
		fprintf(TargetCodeStream,"#����ַ���\n");
		SpliceStrId(MediaTab[i_PrinStr].name1);
		fprintf(TargetCodeStream,"li\t$v0\t4\n");	//�ַ�����4
		fprintf(TargetCodeStream,"la\t$a0\t%s\n",StrIdRes);
		fprintf(TargetCodeStream,"syscall\n");
		strcpy(StrIdRes,"Str");//����StrIdRes����Ϊ"Str"
	}
	else if(isDigit(MediaTab[i_PrinStr].name1[0])!=-1||MediaTab[i_PrinStr].name1[0]=='+'||MediaTab[i_PrinStr].name1[0]=='-')	//������
	{
		fprintf(TargetCodeStream,"#�������\n");
		fprintf(TargetCodeStream,"li\t$t0\t%d\n",(atoi)(MediaTab[i_PrinStr].name1));
		fprintf(TargetCodeStream,"li\t$v0\t1\n");	//������1
		fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
		fprintf(TargetCodeStream,"syscall\n");
	}
	else if(MediaTab[i_PrinStr].name1[0]=='\'')		//���ַ�
	{
		fprintf(TargetCodeStream,"#����ַ�\n");
		fprintf(TargetCodeStream,"li\t$t0\t%d\n",MediaTab[i_PrinStr].name1[1]);
		fprintf(TargetCodeStream,"li\t$v0\t11\n");	//�ַ���11
		fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
		fprintf(TargetCodeStream,"syscall\n");
	}
	else	//��ʶ��
	{
		fprintf(TargetCodeStream,"#�����ʶ��\n");
		LoadData(i_PrinStr,"$t0",Name1,MediaTab[i_PrinStr].name1);
		c = FindIdentLoc(i_PrinStr,Name1);	/*�жϱ�ʶ����ȫ�ֻ��Ǿֲ�*/
		if( ((c == NotMain || c ==JustInMain) && FindTmpIdentAdr(i_PrinStr,Name1,Rtype) == CHAR)||	//main���main������
			( c == IsMain  && FindAdr(i_PrinStr,Name1,Rtype)==CHAR) )	//ȫ��
		{
			fprintf(TargetCodeStream,"li\t$v0\t11\n");	//�ַ���11
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
		else if( ((c == NotMain || c ==JustInMain) && FindTmpIdentAdr(i_PrinStr,Name1,Rtype) == INT)||	//main���main������
			(c == IsMain && FindAdr(i_PrinStr,Name1,Rtype)==INT))	//ȫ��
		{
			fprintf(TargetCodeStream,"li\t$v0\t1\n");	//������1
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
		else if(MediaTab[i_PrinStr].IsCharFlag == 1 )
		{
			fprintf(TargetCodeStream,"li\t$v0\t11\n");	//�ַ���11
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
		else
		{
			fprintf(TargetCodeStream,"li\t$v0\t1\n");	//������1
			fprintf(TargetCodeStream,"move\t$a0\t$t0\n");
			fprintf(TargetCodeStream,"syscall\n");
		}
	}
	//�����
	fprintf(TargetCodeStream,"#����\n");
	fprintf(TargetCodeStream,"li\t$v0\t4\n");
	fprintf(TargetCodeStream,"la\t$a0\tLineBreaks\n");
	fprintf(TargetCodeStream,"syscall\n");
}
/*�����*/
void ScanIdent(int i_ScanIdent)
{
	JudgeMain c;
	c = FindIdentLoc(i_ScanIdent,Name1);	/*�жϱ�ʶ����ȫ�ֻ��Ǿֲ�*/
	if( ( (c == NotMain || c == JustInMain) && FindTmpIdentAdr(i_ScanIdent,Name1,Rtype) == INT)||
		  (c == IsMain && FindAdr(i_ScanIdent,Name1,Rtype)==INT) )
		fprintf(TargetCodeStream,"#��������\nli\t$v0\t5\n");
	else if( ((c == NotMain || c == JustInMain) && FindTmpIdentAdr(i_ScanIdent,Name1,Rtype) == CHAR)||
		(c == IsMain && FindAdr(i_ScanIdent,Name1,Rtype)==CHAR) )
		fprintf(TargetCodeStream,"#�����ַ�\nli\t$v0\t12\n");
	fprintf(TargetCodeStream,"syscall\n");
	StoreData(i_ScanIdent,"$v0",Name1);
}
/*�Ƚ�����*/
void CompareOp(int i_CompareOp)
{
	fprintf(TargetCodeStream,"#�Ƚ�\n");
	LoadData(i_CompareOp,"$t0",Name1,MediaTab[i_CompareOp].name1);
	LoadData(i_CompareOp,"$t1",Name2,MediaTab[i_CompareOp].name2);
	if(MediaTab[i_CompareOp].ins == Grea)
		fprintf(TargetCodeStream,"slt\t$t2\t$t1\t$t0\n");//��name1����name2��$t2��Ϊ1
	else if(MediaTab[i_CompareOp].ins == GaE)
		fprintf(TargetCodeStream,"sge\t$t2\t$t0\t$t1\n");//��name1���ڵ���name2��$t2��Ϊ1
	else if(MediaTab[i_CompareOp].ins == Less)
		fprintf(TargetCodeStream,"slt\t$t2\t$t0\t$t1\n");//��name1С��name2��$t2��Ϊ1
	else if(MediaTab[i_CompareOp].ins == LaE)
		fprintf(TargetCodeStream,"sle\t$t2\t$t0\t$t1\n");//��name1С�ڵ���name2��$t2��Ϊ1
	else if(MediaTab[i_CompareOp].ins == NaE)
		fprintf(TargetCodeStream,"sne\t$t2\t$t0\t$t1\n");//��name1������name2��$t2��Ϊ1
	else if(MediaTab[i_CompareOp].ins == Equ)
		fprintf(TargetCodeStream,"seq\t$t2\t$t0\t$t1\n");//��name1����name2��$t2��Ϊ1
}
/*�Ӽ��˳�*/
void Arithmetic(int i_Arithmetic)
{
	fprintf(TargetCodeStream,"#�Ӽ��˳�\n");
	LoadData(i_Arithmetic,"$t0",Name1,MediaTab[i_Arithmetic].name1);
	if(MediaTab[i_Arithmetic].ins != Neg)	//��Ϊnegû��name2������
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
		fprintf(TargetCodeStream,"neg\t$t2\t$t0\n");	//$t0��ŵ�name1
	StoreData(i_Arithmetic,"$t2",Result);
}
/*���ú�������,push*/	//Push m PARA1 gcd 
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
			LoadData(PushStack[PushId-1].FourId,"$t0",Name1,PushStack[PushId-1].con);	//Name1  is para	ȡ��������ֵ����$t0
			fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",-1*FindFunLen(PushStack[PushId-1].FourId));
			fprintf(TargetCodeStream,"sw\t$t0\t%d($sp)\n",FindParaAdr(PushStack[PushId-1].FourId,FindParaIndex(PushStack[PushId-1].FourId) ));//Ѱ��name1�ĵ�ַ
			fprintf(TargetCodeStream,"addu\t$sp\t$sp\t%d\n",FindFunLen(PushStack[PushId-1].FourId));
			ParaOutStack();
		}
		ParaOutStack();
	}
	return (Cur-1);
}
/*������ջ*/
void ParaIntoStack(int i_ParaIntoStack)
{
	if(MediaTab[i_ParaIntoStack].ins!=CallBegin)
	{
		strcpy(PushStack[PushId].con,MediaTab[i_ParaIntoStack].name1);	//��������ջ
		PushStack[PushId].ParaPosi=FindParaIndex(i_ParaIntoStack);	//�����±���ջ
		strcpy(PushStack[PushId].funName,MediaTab[i_ParaIntoStack].result);//�����õĺ���������ջ
		PushStack[PushId].FourId = i_ParaIntoStack;	//������Ԫʽ��λ�ü�����
		//printf("��ջ������\t%s\t�����±�\t%d���ú���\t%s����Ԫʽ��λ��%d\n",PushStack[PushId].con,PushStack[PushId].ParaPosi,PushStack[PushId].funName,PushStack[PushId].FourId);
	}
	else
		strcpy(PushStack[PushId].con,"CallBegin");
	PushId++;
}
/*������ջ*/
void ParaOutStack()
{
	//printf("��ջ������\t%s\t�����±�\t%d���ú���\t%s����Ԫʽ��λ��%d\n",PushStack[PushId-1].con,PushStack[PushId-1].ParaPosi,PushStack[PushId-1].funName,PushStack[PushId-1].FourId);
	strcpy(PushStack[PushId-1].con,"\0");	//������ջ
	PushStack[PushId-1].ParaPosi=-1;	//�����±��ջ
	strcpy(PushStack[PushId-1].funName,"\0");//�����õĺ������ֳ�ջ
	PushStack[PushId-1].FourId = -1;	//������Ԫ��ʽ��λ����Ϊ-1
	PushId--;
}
/*�����޲�����������*/	//callend
void CallNoParaFun(int i_CallNoParaFun)
{
	fprintf(TargetCodeStream,"jal\t%s\n",MediaTab[i_CallNoParaFun].name1);
}
/*����ַ���ʱѰ����Ӧ���ݶε�Str�±�*/
void FindStrLabel(char CurStr[])
{
	int m=0;
	for(m=0;m<count;m++)
	{
		if(strcmp(Str[m].kind,"StrSym")==0)   //���ַ���������
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
/*����Str���±�ƴ�ӳ�Str�ı�ǩ*/
void SpliceStrId(char CurStr[])
{
	char FindStrIdTmp[3];
	FindStrLabel(CurStr);
	itoa(FindStrId,FindStrIdTmp,10);//����ת�ַ�������LabelTmp����
	FindStrId = 0;
	strcat(StrIdRes,FindStrIdTmp);//�ѱ�ǩ�±�ͱ�ǩ��ƴ������
}
/*�ڽṹ������RunStack��Ѱ��ȫ�ֵĵ�ַ������*/
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
/*�ڷ��ű���Ѱ����ʱ����������������� ��ַ������������Ϊ��������*/
int FindTmpIdentAdr(int i_FindTmpIdentAdr,LoadWhat which,ReturnWhat rewhich)
{
	int id=i_FindTmpIdentAdr;
	int m=0;
	char itsFunName[100],CurIdent[100];
	//���浱ǰ���жϵı�ʶ����CurIdent
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
		strcpy(itsFunName,MediaTab[id].name1);	//�ҵ���������ĺ�����Ȼ���ٴ����������ʼ��������ʱ����������������ĵ�ַ
	while(strcmp(Tab[m].name,itsFunName)!=0)	//�ӷ��ű��ͷ����ʼ�������������ڵĺ�������ֹͣ
		m++;
	do{
		m++;
	}while(strcmp(CurIdent,Tab[m].name)!=0);	//�����ں������������¶���ֱ����������ı�ʶ����ֹͣ
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
/*Ѱ�Ҳ�����Ӧ�ĵ�ַ*/	//�Ѿ����
int FindParaAdr(int i_FindParaAdr,int id_Para)
{
	int ParaId=0;
	for(ParaId=0;ParaId<TabNum;ParaId++)
	{
		if(Tab[ParaId].obj == FUNC && strcmp(MediaTab[i_FindParaAdr].result,Tab[ParaId].name)==0)
			return Tab[ParaId + id_Para].adrFirst;	//id_Para��1��ʼ����ε���
		else
			continue;
	}
}
/*Ѱ�Һ����ĳ���*/
int FindFunLen(int i_FindFunLen)
{
	int m=0;
	for(m=0;m<CodeNum;m++)
	{
		if(MediaTab[m].ins == Func && strcmp(MediaTab[m].name1,MediaTab[i_FindFunLen].result)==0)	//�ҵ������ú����ĳ���
			//printf("�����õĺ�����%s\t����Ϊ%d\n",MediaTab[m].name1,MediaTab[m].FunLen);
			return MediaTab[m].FunLen;
	}
}
/*ת�������±�*/
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
//�ж����ڲ��
JudgeMain FindIdentLoc(int i_FindIdentLoc,LoadWhat l_FindIdentLoc)
{
	int Id1518=0,id=i_FindIdentLoc;
	int m=0,tmplev;
	char itsFunName[100]={'\0'},CurIdent[100]={'\0'};
	//���浱ǰ���жϵı�ʶ����CurIdent
	if(l_FindIdentLoc == Name1)
		strcpy(CurIdent,MediaTab[i_FindIdentLoc].name1);	
	else if(l_FindIdentLoc == Name2)
		strcpy(CurIdent,MediaTab[i_FindIdentLoc].name2);	
	else if(l_FindIdentLoc == Result)
		strcpy(CurIdent,MediaTab[i_FindIdentLoc].result);	
	while(MediaTab[id].ins!=Func)		//��ǰ�ҵ���������ĺ���
		id--;
	if(MediaTab[id].ins == Func)
		strcpy(itsFunName,MediaTab[id].name1);	//�ҵ���������ĺ�����Ȼ���ٴ����������ʼ��������ʱ����������������ĵ�ַ		
	while(strcmp(Tab[m].name,itsFunName)!=0)	//�ӷ��ű��ͷ����ʼ�������������ڵĺ�������ֹͣ,��Ϊ�Ǵ�ǰ�����ң���ʱ�ҵ���һ���Ǻ������岿��
		m++;
	tmplev = Tab[m].lev;	//���浱ǰ���
	while(Tab[m].lev==tmplev)	//��֤����main����
	{
		if(strcmp(CurIdent,Tab[m].name)==0  && strcmp(itsFunName,"main")!=0)	//�������main����
			return NotMain;	
		else if(strcmp(CurIdent,Tab[m].name)==0  && strcmp(itsFunName,"main")==0)	//�����main��������
			return JustInMain;
		else
			m++;
	}
	for(Id1518=0;Id1518<RunIndex;Id1518++)
	{
		if(l_FindIdentLoc == Name1 && strcmp(RunStack[Id1518].name,CurIdent)==0
			|| l_FindIdentLoc == Name2 && strcmp(RunStack[Id1518].name,CurIdent)==0
			|| l_FindIdentLoc == Result && strcmp(RunStack[Id1518].name,CurIdent)==0)
			return IsMain;//��ȫ�ֵ�
	}
}
/*Ѱ��Ӧ����ת���ĸ�case*/
int WhichCase(int i_WhichCase)		//ThisLabel Label labelCon_2 2
{
	int i = i_WhichCase+1;
	char switB[100],switB1[100];
	strcpy(switB,MediaTab[i_WhichCase].result);	//switB ������SwitEnd_0  
	while(strcmp(switB,MediaTab[i].result)!=0)
	{
		if(strncmp(MediaTab[i].name1,"labelcon_",9)==0)		//ֻ�ж�ǰ��λ�Ƿ�һ��, �Ƚ��ַ���str1��str2��ǰn���ַ����ֵ���Ĵ�С�����ǲ�������ĸ��Сд
		{
			if(MediaTab[i].result[0]=='\'')		//�������������ַ�	ThisLabel Label labelCon_9 'E'  ��������result������
				Many[switId].SwitchCase[switCaseId].caseCon = MediaTab[i].result[1];
			else if(isDigit(MediaTab[i].result[0])!=-1||MediaTab[i].result[0]=='+'||MediaTab[i].result[0]=='-')	//������	ThisLabel Label labelCon_7 1
				Many[switId].SwitchCase[switCaseId].caseCon = (atoi)(MediaTab[i].result);
			strcpy(Many[switId].SwitchCase[switCaseId].SwitchGoto,MediaTab[i].name1);
			fprintf(TargetCodeStream,"beq\t$t0\t%d\t%s\n",Many[switId].SwitchCase[switCaseId].caseCon,
			Many[switId].SwitchCase[switCaseId].SwitchGoto);//name1	
			switCaseId++;
		}
		else if(MediaTab[i].ins == Swit)	//����switch��Ƕ��switch
		{
			strcpy(switB1,MediaTab[i].result);
			do{i++;}while(strcmp(switB1,MediaTab[i].result)!=0);
		}
		i++;
	}
	if(strcmp(switB,MediaTab[i].result)==0)		//������ж�����������ȣ�����ת�����switch�Ľ�����
		fprintf(TargetCodeStream,"j\t%s\n",MediaTab[i].name1);	//ThisLabel Label label_0 SwitEnd_0	   name1����label_0
	return i;
}