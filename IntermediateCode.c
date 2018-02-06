#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WordAnalysis.h"
#include "grammar.h"
#include "ErrorHandling.h"
#include  "KindsOfTabs.h"
#include "IntermediateCode.h"

FILE * InterCodeStream = NULL;//����ļ�

int CodeNum = 1;
int FunLen = 0;
int CurFun_CodeNum=-1;
int ParaNum=1;//�����±��1��ʼ
char *PrinInstr[50]={"Gcon","Gvar","Garr","Func","Para","Tcon","Tvar","Tarr","Grea","GaE","Less","LaE",
	"NaE","Equ","Goto","TGoto","FGoto","Ret","NRet","RetS","Add","Min","Mul","Div","Neg","Ass","CallBegin","CallEnd","Push",
	"End","ArrA","ArrG","Scan","Prin","Swit","SwitEnd","ThisLabel"}; 
char *PrinCodeType[10]={"Void","Int","Char","Label"};
char ReArrG[100];
char ReFact[100];
char ReTerm[100];
char ReExpr[100];
char ReCond[100];
char LabelNumber[100];
char CaseNumber[100];
char ReRetS[100];
char ReNeg[100];
char ReInt[100];
char EndId[100];
char SwitBEid[100];
char emmEndSwit[100]="TheEndOfSwitch";

void AddLabel(char Intername1[])
{
	MediaTab[CodeNum].ins = ThisLabel;
	MediaTab[CodeNum].type = Label;
	strcpy(MediaTab[CodeNum].name1,Intername1);	//����label_1 
	printf("-*-*-*-*-*-*-*-\nָ�%s����ǩ����:%s\n",PrinInstr[ThisLabel],Intername1);
	CodeNum++;
}
//��
void InsName1(Instr Interins,char Intername1[])
{
	MediaTab[CodeNum].ins = Interins;
	strcpy(MediaTab[CodeNum].name1,Intername1);
	if(IsChar!=0)
		MediaTab[CodeNum].IsCharFlag=1;
	printf("-*-*-*-*-*-*-*-\nָ��:%s ����һ:%s\n",PrinInstr[Interins],Intername1);
	CodeNum++;
	fprintf(InterCodeStream,"%s %s\n",PrinInstr[Interins],Intername1);
}
//��
void InsTypeName1(Instr Interins,CodeType Intertype,char Intername1[])
{
	MediaTab[CodeNum].ins = Interins;
	MediaTab[CodeNum].type = Intertype;
	strcpy(MediaTab[CodeNum].name1,Intername1);
	//if(IsChar!=0)
	//	MediaTab[CodeNum].IsCharFlag=1;
	printf("-*-*-*-*-*-*-*-\nָ��:%s ����:%s ����һ:%s\n",PrinInstr[Interins],PrinCodeType[Intertype],Intername1);
	CodeNum++;
	fprintf(InterCodeStream,"%s %s %s\n",PrinInstr[Interins],PrinCodeType[Intertype],Intername1);
}
//��
void InsName1Res(Instr Interins,char Intername1[],char Interresult[])
{
	MediaTab[CodeNum].ins = Interins;
	strcpy(MediaTab[CodeNum].name1,Intername1);
	strcpy(MediaTab[CodeNum].result,Interresult);
	//if(IsChar!=0)
	//	MediaTab[CodeNum].IsCharFlag=1;
	printf("-*-*-*-*-*-*-*-\nָ��:%s ����һ:%s ���:%s\n",PrinInstr[Interins],Intername1,Interresult);
	CodeNum++;
	fprintf(InterCodeStream,"%s %s %s\n",PrinInstr[Interins],Intername1,Interresult);
}
//��
void InsTypeName1Res(Instr Interins,CodeType Intertype,char Intername1[],char Interresult[])
{
	MediaTab[CodeNum].ins = Interins;
	MediaTab[CodeNum].type = Intertype;
	strcpy(MediaTab[CodeNum].name1,Intername1);
	strcpy(MediaTab[CodeNum].result,Interresult);
	//if(IsChar!=0)
	//	MediaTab[CodeNum].IsCharFlag=1;
	printf("-*-*-*-*-*-*-*-\nָ��:%s ����:%s ����һ:%s ���:%s\n",PrinInstr[Interins],PrinCodeType[Intertype],Intername1,Interresult);
	CodeNum++;
	fprintf(InterCodeStream,"%s %s %s %s\n",PrinInstr[Interins],PrinCodeType[Intertype],Intername1,Interresult);
}
//��
void InsName12Res(Instr Interins,char Intername1[],char Intername2[],char Interresult[])
{
	MediaTab[CodeNum].ins = Interins;
	strcpy(MediaTab[CodeNum].name1,Intername1);
	strcpy(MediaTab[CodeNum].name2,Intername2);
	strcpy(MediaTab[CodeNum].result,Interresult);
	//if(IsChar!=0)
	//	MediaTab[CodeNum].IsCharFlag=1;
	printf("-*-*-*-*-*-*-*-\nָ��:%s ����һ:%s ���ֶ�:%s ���:%s\n",PrinInstr[Interins],Intername1,Intername2,Interresult);
	CodeNum++;
	fprintf(InterCodeStream,"%s %s %s %s\n",PrinInstr[Interins],Intername1,Intername2,Interresult);
}