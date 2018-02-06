#ifndef __KindsOfTabs_H__
#define __KindsOfTabs_H__

#define MaxName 100
/*
	��ʶ������type
	VOID 0
	INT  1
	CHAR 2
*/
typedef enum
{
	VOID=0,INT,CHAR
}IdentType;
/*
	��ʶ������obj
	PARA  2    ����
	FUNC  3    ������
	STR   4    �ַ���
	GVAR  5    ȫ�ֱ���
	GCON  6    ȫ�ֳ���
	GARR  7    ȫ������
	TVAR  8    ��ʱ����
	TCON  9   ��ʱ����
	TARR  10   ��ʱ����
	LABEL 11   ��ǩ(������ת)
	GTVAR 12   ȫ����ʱ��������tmp1��
	TTVAR 13   �ֲ���ʱ��������tmp1��
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
/*������ű�����*/
struct{
	char name[MaxName];//��ʶ������
	IdentType type;//��ʶ������  INT CHAR
	IdentObj obj;//��ʶ������   VAR PARA FUNC ARR�ȵ�
	int lev;//��ʶ�����ڷֳ���ľ�̬��Σ��涨��������Ϊ1��Ƕ�׷ֳ�������һ����
	int value;//��ֵ
	/*���ڳ�����������Ӧ������ֵ
	*�������飬 ��������洢��Ԫ����Ŀ������С��
	*/
	int adrFirst;
	/*���ڱ������ñ���������ջS�з���洢��Ԫ�� ��Ե�ַ
	*���ں���������Ŀ�������ڵ�ַ
	*�������飬�ʼ��ַ
	*/
	int AllParaNum;
}Tab[4096000];	

//¼����ű�
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