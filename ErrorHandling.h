#ifndef __ErrorHandling_H__
#define __ErrorHandling_H__

/*
	1:	ȱ��(	
	2:	ȱ�٣�
	4:	ȱ��]	3
	5:	ȱ��{	4
	6:	ȱ��}	5
	7:	ȱ��;	6
	8:	ȱ��=	7
	18:  ȱ��:	8

	13: ȱ��main		9
	14: ȱ��void		10
	16:ȱ��while		11
	19:ȱ��case		12
	21:ȱ��int��char		13
	22:ȱ��if		14
	23:ȱ��return		15
	24:ȱ��do		16
	25:ȱ��scanf		17				
	26:ȱ��printf		18

	10:	�Ƿ��޷�������	19
	12:	�Ƿ���ʶ��		20
	15: �Ƿ���ֵ			21
	17:��������			22
	20:�ַ���������Ҫ��	23
	���ű�����	24
	�ظ�����		25
	δ����		26
	�Ƿ�����ֵ	27
	�������Խ�� 28
	���Ҳ��������Ͳ�һ�� 29
*/
extern FILE * ErrorOut;//��������ļ�
extern int ErrorNum;
extern 	int ErrorFlag;

void Error(int e);
void Jump(int r);

#endif	/*ErrorHandling*/