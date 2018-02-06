#ifndef __ErrorHandling_H__
#define __ErrorHandling_H__

/*
	1:	缺少(	
	2:	缺少）
	4:	缺少]	3
	5:	缺少{	4
	6:	缺少}	5
	7:	缺少;	6
	8:	缺少=	7
	18:  缺少:	8

	13: 缺少main		9
	14: 缺少void		10
	16:缺少while		11
	19:缺少case		12
	21:缺少int或char		13
	22:缺少if		14
	23:缺少return		15
	24:缺少do		16
	25:缺少scanf		17				
	26:缺少printf		18

	10:	非法无符号整数	19
	12:	非法标识符		20
	15: 非法赋值			21
	17:常量错误			22
	20:字符串不符合要求	23
	符号表满了	24
	重复定义		25
	未定义		26
	非法返回值	27
	数组访问越界 28
	左右操作数类型不一致 29
*/
extern FILE * ErrorOut;//引用输出文件
extern int ErrorNum;
extern 	int ErrorFlag;

void Error(int e);
void Jump(int r);

#endif	/*ErrorHandling*/