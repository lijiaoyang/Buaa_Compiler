#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>
#include"WordAnalysis.h"

FILE * fp = NULL;
FILE * stream = NULL;

int i = 0;//i��alpha�����±�
int flag = 0;//flag�ж��Ƿ�Ϊ������ ������Ϊ1
int line = 1;//line���б�ʶ�������������֣��ĸ�����������
int lineflag = 0;//lineflagΪ0ʱ��ʾ���Ի��У�����\n����lineflag=0
int TextLine = 1;//ԭ���кţ���1��ʼ
char alpha[1024]={'\0'};//��ŵ�ǰ�е�����
char token[1024][30];//��ű�ʶ��������
char id[][10] = {"const","int","char","void","main","if","do","while","switch","case","scanf","printf","return"};
void lineCheck(){
	if(lineflag==0){
		fprintf(stream,"%d\t%d\t",line,TextLine);
		line++;
		lineflag++;
	}
}
int isLetter(char a) {
	if (a >= 'a'&&a <= 'z' || a >= 'A'&&a <= 'Z'|| a=='_')
		return 1;
	else
		return 0;
}
int isDigit(char a) {
	if (a >= '0'&&a <= '9')
		return (int)(a - '0');
	else
		return -1;
}
void ungetcha() {
	i--;
}
char getcha() {
	char c, c_new;
	c = alpha[i];
	if (c=='\t'|| c=='\n')
	{
		i++;
		if(c=='\n')
			TextLine++;
		return -1;
	}
	else {
		c_new = c;
		i++;//���´�ʱ�ķ��Ų�������ָ����һ��Ԫ��
		return c_new;
	}
}
//����ַ������ַ�
void Str_CH(char c){
	if(c=='\"'){
		lineCheck();
		fprintf(stream ,"StrSym%50c",c);
		while ((c = getcha()) != -1 && c!='\"'&& c>=32 && c<=126 ){
			fprintf(stream ,"%c",c);
		}
		if(c=='\"'){
			fprintf(stream ,"%c\n",c);
			lineflag=0;//ǰ����\n����flagline=0
		}
	}
	else if(c=='\''){
		lineCheck();
		fprintf(stream ,"CHSym%50c",c);
		while ((c = getcha()) != -1 && c!='\''){
			fprintf(stream ,"%c",c);
		}
		if(c=='\''){
			fprintf(stream ,"%c\n",c);
			lineflag=0;//ǰ����\n����flagline=0
		}
	}
}
int isSym(char c) {
	/*
	+	1	Plus
	-	2	Minus
	*	3	Star
	/	4	Div
	<	5	Less
	<=	6	L_E
	>	7	Grea
	>=	8	G_E
	!=	9	N_E
	==	10	Equ
	_	11	Under
	'	12	Apos
	"	13	Doub
	;	14	Semi
	=	15	Assign
	,	16	Comma
	[	17	L_M_Par
	]	18	R_M_Par
	(	19	L_S_Par
	)	20	R_S_Par
	{	21	L_B_Par
	}	22	R_B_Par
	:	23	Colon
	*/
	char a;
	switch (c)
	{
	case '+':	return 1;
	case '-':	return 2;
	case '*':	return 3;
	case '/':	return 4;
	case '<': {
		a = getcha();
		if (a == '=')
		{
			return 6; //  <=
		}
		else
		{
			ungetcha();
			return 5;  // <
		}
	}
	case '>': {
		a = getcha();
		if (a == '=')
		{
			return 8;// >=
		}
		else
		{
			ungetcha();
			return 7; // >
		}
	}
	case '!': {
		a = getcha();
		if (a == '=')
		{
			return 9;// !=
		}
		else
		{
			ungetcha();//ֻ��̾�ŵĻ�ʲôҲ������
		}
	}
	case '=': {
		a = getcha();
		if (a == '=')
		{
			return 10;// ==
		}
		else
		{
			ungetcha();
			return 15; // =
		}
	}
	case '_':	return 11; // _
	case '\'':	return 12; // '
	case '\"':	return 13; // "
	case ';':	return 14; // ;
	case ',':	return 16; // ,
	case '[':	return 17; // [
	case ']':	return 18; // ]
	case '(':	return 19; // (
	case ')':	return 20; // )
	case '{':	return 21; // {
	case '}':	return 22; // }
	case ':':	return 23; // :
	default:	return -1;
	}
}
int compare(int h) {
	int v=0;	
	for (v = 0; v < 15; v++)
		{
			if (stricmp(id[v], token[h]) == 0) {//�Ǳ�����
				flag = 1;
				return v;
				break;
			}
			else
			{
				flag = 0;//���Ǳ�����
			}
		}
}
void print(int k) {
	int v=0;
	v=compare(k);
	if (flag == 1){//������
		lineCheck();
		fprintf(stream ,"%sSym%50s\n", id[v], strlwr(token[k]));
		lineflag=0;//ǰ����\n����flagline=0
		}
	else if (flag == 0){ //��ʶ��
		lineCheck();
		fprintf(stream ,"IdSym%50s\n", strlwr(token[k]));
		lineflag=0;//ǰ����\n����flagline=0
		}
	flag = 0;
}
void printSym(int d) {
	/*
	+	1	Plus
	-	2	Minus
	*	3	Star
	/	4	Div
	<	5	Less
	<=	6	L_E
	>	7	Grea
	>=	8	G_E
	!=	9	N_E
	==	10	Equ
	_	11	Under
	'	12	Apos
	"	13	Doub
	;	14	Semi
	=	15	Assign
	,	16	Comma
	[	17	L_M_Par
	]	18	R_M_Par
	(	19	L_S_Par
	)	20	R_S_Par
	{	21	L_B_Par
	}	22	R_B_Par
	:	23	Colon
	*/
	switch (d)
{
	case 1:	{
		lineCheck();
		fprintf(stream,"Plus");//+
		break;
	}
	case 2: {
		lineCheck();
		fprintf(stream,"Minus");//-
		break;
	}
	case 3: {
		lineCheck();
		fprintf(stream,"Star");//*
		break;
	}
	case 4: {
		lineCheck();
		fprintf(stream,"Div");// /
		break;
	}
	case 5: {
		lineCheck();
		fprintf(stream,"Less");// <
		break;
	}
	case 6: {
		lineCheck();
		fprintf(stream,"L_E");// <=
		break;
	}
	case 7: {
		lineCheck();
		fprintf(stream,"Grea"); // >
		break;
	}
	case 8: {
		lineCheck();
		fprintf(stream,"G_E"); // >=
		break;
	}
	case 9: {
		lineCheck();
		fprintf(stream,"N_E"); // !=
		break;
	}
	case 10: {
		lineCheck();
		fprintf(stream,"Equ"); // ==
		break;
	}
	case 11: {
		lineCheck();
		fprintf(stream,"Under"); // _
		break;
	}
	case 12: {
		lineCheck();
		fprintf(stream,"Apos"); // '
		break;
	}
	case 13: {
		lineCheck();
		fprintf(stream,"Doub"); // "
		break;
	}
	case 14: {
		lineCheck();
		fprintf(stream,"Semi"); // ;
		break;
	}
	case 15: {
		lineCheck();
		fprintf(stream,"Assign"); // =
		break;
	}
	case 16: {
		lineCheck();
		fprintf(stream,"Comma"); // ,
		break;
	}
	case 17: {
		lineCheck();
		fprintf(stream,"L_M_Par"); // [
		break;
	}
	case 18: {
		lineCheck();
		fprintf(stream,"R_M_Par"); // ]
		break;
	}
	case 19: {
		lineCheck();
		fprintf(stream,"L_S_Par"); // (
		break;
	}
	case 20: {
		lineCheck();
		fprintf(stream,"R_S_Par"); // )
		break;
	}
	case 21: {
		lineCheck();
		fprintf(stream,"L_B_Par"); //{
		break;
	}
	case 22: {
		lineCheck();
		fprintf(stream,"R_B_Par"); // }
		break;
	}
	case 23: {
		lineCheck();
		fprintf(stream,"Colon"); //:
		break;
	}
	}
}
int WordAnalysis() {
	char c,filename[1024];
	int k = 0, h = 0, num = 0, d,ErrorFlag=0;
	printf("�������ļ���\n");
	gets(filename);
	if((fp=fopen(filename,"r+"))==NULL){
          printf("Open File Failed!\n"); 
          return -1; 
    } 
	stream = fopen("15182615_���_�ʷ��������.docx", "w");
	while(!feof(fp))
	{
		fgets(alpha,1024,fp);
		for (i = 0; alpha[i] != '\0'; )
		{
			c = getcha();
			if(c == -1 || c==' ')
				continue;
			else if (isDigit(c) != -1) {
				while (isDigit(c) != -1) //���ִ���ʽ������ǰ0������023����23��05��5��00��0��0��0
				{
					num = 10 * num + isDigit(c);
					c = getcha();
				}
				lineCheck();
				fprintf(stream,"Num%50d\n", num);
				lineflag=0;//ǰ����\n����flagline=0
				ungetcha();
				num = 0;
			}
			else if (isLetter(c))
			{
				while (isLetter(c))
				{
					token[k][h++] = c;
					while ((c = getcha()) != -1 && (isLetter(c)!=0||isDigit(c)!=-1)) //=-1ʱ��ʾ�ո�֮���
					{
						token[k][h++] = c;
					}
					ungetcha();
					print(k);
					h = 0;
					k++;
				}
			}
			else if(c=='\"'||c=='\'')
			{
				Str_CH(c);
			}
			else if ((d = isSym(c)) != -1)
			{
				printSym(d);
				if (d==6||d==8||d==9||d==10)
				{
					fprintf(stream,"%50c=\n",c);
					lineflag=0;//ǰ����\n����flagline=0
				}
				else 
				{
					fprintf(stream,"%50c\n",c);
					lineflag=0;//ǰ����\n����flagline=0

				}
			}
			else if(c=='\n'){
				memset(alpha,'\0',1024);
				fgets(alpha,1024,fp);
			}
		}
		memset(alpha,'\0',1024);
	}
	fclose(fp);
	fclose(stream);
	printf("�ʷ��������ֽ���\n");
}