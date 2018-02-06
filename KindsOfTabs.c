#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KindsOfTabs.h"
#include "WordAnalysis.h"
#include "grammar.h"
#include "IntermediateCode.h"
#include "TargetCode.h"
#include "ErrorHandling.h"

int TabNum=0;//����±�
int Lev = 0;
int adr = 0;
int IntoFun = 0;//��ʾ���뺯���ı��
int IntoMain = 0;//��ʾ����main����
int LabelNum = 0;//��ǩ����
int VarNum = 0;//��ʱ�����±�
int EndNum = 0;//End���±�
int SwitLabNum = 0;//Switch��case����label
int SwitEndNum = 0;
char *PrinType[10] = {"VOID","INT","CHAR"};
char *PrinObj[30] = {"PARA","FUNC","GVAR","GCON","GARR","TVAR","TCON","TARR","LABEL","GTVAR","TTVAR","NOREEND","REEND","MAINEND"};

int enterTab(char enT_name[],IdentType enT_type,IdentObj enT_obj,int enT_value,int enT_adr)
{
	strcpy(Tab[TabNum].name,enT_name);//�ֱ������½ڵ��������  ��ʶ������
	Tab[TabNum].type = enT_type;//��ʶ������ INT CHAR VOID TMPVAR
	Tab[TabNum].obj = enT_obj;//��ʶ������
	Tab[TabNum].lev = Lev;
	Tab[TabNum].value = enT_value;
	Tab[TabNum].adrFirst = enT_adr;
	//printf("-.-.-.-.-.-%d\n����:%s�Ѿ�¼����ű�\n����:%d\n����:%d\n���:%d\n����ֵ�������С:%d\n��ַ: %d\n",TabNum,enT_name,enT_type,enT_obj,Tab[TabNum].lev,enT_value,enT_adr);
	TabNum++;
	return TabNum;//���ص�ַ����ַ��1��ʼ��
}
//����ű��غ������� VOID 1  INT 2  CHAR 3
IdentType FindFuncName(char name[]) 
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(strcmp(strlwr(name),Tab[i].name)==0 && Tab[i].type == VOID)//��ʾ���޷���ֵ�ĺ���
			return VOID;
		else if(strcmp(strlwr(name),Tab[i].name)==0 && Tab[i].type == INT)//��ʾ���޷���ֵ�ĺ���
			return INT;
		else if(strcmp(strlwr(name),Tab[i].name)==0 && Tab[i].type == CHAR)//��ʾ���޷���ֵ�ĺ���
			return CHAR;
	}
}
/*
1.�ֲ���ȫ�ֿ�������
2.���������ڲ��������������ظ�
*/
int findDef(char name[])
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(stricmp(name,Tab[i].name)==0)//��ʾ�ҵ���������
			return i;
	}
	return -1; //û�ҵ�ͬ����
}
void findResult(int i_findResult)	//��ȫ�ֳ�����������������
{
	int i;
	i = findDef(Str[i_findResult].name);
	if(i>=0)//��ʾ��ȫ�ֳ�����������������,���ǲ�������һԪ��ͬ��
	{
		fprintf(ErrorOut,"��%d����%s������\n",Str[i_findResult].OriLine,Str[i_findResult].name);
	}
}
void findTmpSame(int i_findTmpSame)
{
	int i = 0,n=0,g=0;
	for(g=0;g<TabNum;g++)
	{
		if(Tab[g].obj == FUNC && strcmp(Tab[g].name,Str[i_findTmpSame].name)==0)
			fprintf(ErrorOut,"��%d���뺯����%s����\n",Str[i_findTmpSame].OriLine,Str[i_findTmpSame].name);
	}
	while(Tab[n].lev != Lev)
		n++;
	if(Tab[n].lev == Lev)	//���ڵ����˵�ǰ���
	{
		for(i = n+1;Tab[i].lev == Lev && i<TabNum; i++)	//�ӵ�ǰ��εĺ�������ʼ�ҿ�����û���ظ�����
		{
			if(strcmp(Str[i_findTmpSame].name,Tab[i].name)==0)//��ʾ�ڸò���ҵ���������
			{
				fprintf(ErrorOut,"��%d����%s������\n",Str[i_findTmpSame].OriLine,Str[i_findTmpSame].name);
				break;
			}
		}
	}
}
//�ҵ����鳤�ȣ��жϷ���Խ������
int findArrLen(int id)
{
	int i = 0;
	for(i = 0;i < TabNum; i++)
	{
		if(strcmp(Str[id].name,Tab[i].name)==0 && (Tab[i].obj == TARR|| Tab[i].obj == GARR))//��ʾ�ҵ���������
			return Tab[i].value;
	}
	return -1; //û�ҵ��������
}
//Ϊ��ת��ӱ�ǩ������label_1  label_2  label_3
void LableIntoMTab()  
{
	char name[100]="label_";
	char LabelTmp[6];
	itoa(LabelNum,LabelTmp,10);//����ת�ַ�������LabelTmp����
	strcat(name,LabelTmp);//�ѱ�ǩ�±�ͱ�ǩ��ƴ������
	strcpy(LabelNumber,name);//��ǩ������LabelNumber�ַ�������
	fprintf(InterCodeStream,"%s\n",LabelNumber);
	LabelNum++;
}
//Ϊswitch��case�����ӱ�ǩ
void AddCaseLabel(int i)
{
	char name[100]="labelcon_";//��ÿһ��case����label
	char LabelTmp[6],NegTmp[1024]="-",FinInt[1024]={'\0'};
	if(strcmp(Str[index-2].kind,"Minus")==0)	//����Ǹ�������ǰ����ϸ���
	{
		strcat(NegTmp,Str[index-1].name);
		strcpy(FinInt,NegTmp);
	}
	else
		strcpy(FinInt,Str[index-1].name);	//����ֱ�Ӹ��Ƶ�FinInt
	itoa(SwitLabNum,LabelTmp,10);
	strcat(name,LabelTmp);
	strcpy(CaseNumber,name);
	fprintf(InterCodeStream,"%s\n",CaseNumber);
	InsTypeName1Res(ThisLabel,Label,CaseNumber,FinInt);
	SwitLabNum++;
}
//������ʱ�ı���������tmp1  tmp2  tmp3
void makeVar(char name[])
{
	char tmp[15]="tmp";
	char VarTmp[6];
	itoa(VarNum,VarTmp,10);//����ת�ַ���
	strcat(tmp,VarTmp);
	strcpy(name,tmp);
	VarNum++;
}
//����End1518_1  end1518_2
void makeEndLabel()
{
	char EndTmp[6];
	strcpy(EndId,"end1518_");
	itoa(EndNum,EndTmp,10);//����ת�ַ���
	strcat(EndId,EndTmp);
	EndNum++;
}
//����switch��id,����Ƕ��switchʱ�����
void MakeSwitchEndLabel()
{
	char EndTmp[6];
	strcpy(SwitBEid,"switend_");
	itoa(SwitEndNum,EndTmp,10);
	strcat(SwitBEid,EndTmp);
	SwitEndNum++;
}