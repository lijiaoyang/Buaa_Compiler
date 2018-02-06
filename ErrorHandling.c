#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>
#include"WordAnalysis.h"
#include "grammar.h"
#include"ErrorHandling.h"

int ErrorNum = 0;
int ErrorFlag=0;
FILE * ErrorOut;//��������ļ�
void Error(int e)
{
	fprintf(ErrorOut,"Error%d\t",e);
	switch(e)
	{
		case 1:fprintf(ErrorOut,"lack (	L_S_Par\n");break;
		case 2:fprintf(ErrorOut,"lack )	R_S_Par\n");break;
		case 3:fprintf(ErrorOut,"lack ] R_M_Par\n");break;
		case 4:fprintf(ErrorOut,"lack { L_B_Par\n");break;
		case 5:fprintf(ErrorOut,"lack } R_B_Par\n");break;
		case 6:fprintf(ErrorOut,"lack ; Semi\n");break;
		case 7:fprintf(ErrorOut,"lack = Assign\n");break;
		case 8:fprintf(ErrorOut,"lack : Colon\n");break;
		case 9:fprintf(ErrorOut,"lack main \n");break;
		case 10:fprintf(ErrorOut,"lack void \n");break;
		case 11:fprintf(ErrorOut,"lack while \n");break;
		case 12:fprintf(ErrorOut,"lack case \n");break;
		case 13:fprintf(ErrorOut,"lack int OR char \n");break;
		case 15:fprintf(ErrorOut,"lack return \n");break;
		case 17:fprintf(ErrorOut,"lack scanf \n");break;
		case 18:fprintf(ErrorOut,"lack printf \n");break;
		case 19:fprintf(ErrorOut,"Illegal unsigned integer \n");break;
		case 20:fprintf(ErrorOut,"Illegal identifier \n");break;
		case 21:fprintf(ErrorOut,"Illegal assignment \n");break;
		case 22:fprintf(ErrorOut,"Constant error \n");break;
		case 23:fprintf(ErrorOut,"String does not meet the requirements \n");break;  //??????
		case 27:fprintf(ErrorOut,"Illegal return value \n");break; //ok
		case 28:fprintf(ErrorOut,"Array access cross-border \n");break;
		case 30:fprintf(ErrorOut,"Wrong Func Call \n");break;

		//case 29:printf("lack ( \n");break;
		//case 30:printf("lack ( \n");break;
	}
	fprintf(ErrorOut,"��������ڵ� %d �� ������%s ������%s\n",Str[index-1].OriLine,Str[index-1].kind,Str[index-1].name);
	ErrorNum++;//ÿ������һ�����󣬴���������1
	//exit(0);
	ErrorFlag = e;
}

void Jump(int r)
{
	switch(r)
	{
		case 1:{	//ȱ�ٷֺ�		6    ������һ���ֺ�
					while(strcmp(Str[index].kind,"Semi")!=0)
						nextsym();
					fprintf(ErrorOut,"������һ���ֺ�\n");
					break;
			   }
		case 2:{	//������һ�����Ż�ֺ�
					while(strcmp(Str[index].kind,"Semi")!=0 && strcmp(Str[index].kind,"Comma")!=0)
						nextsym();
					fprintf(ErrorOut,"������һ�����Ż�ֺ�\n");
					break;
			   }
		case 3:{	//������һ��int��char��void
					while( strcmp(Str[index].kind,"intSym")!=0 && strcmp(Str[index].kind,"charSym")!=0
						  && strcmp(Str[index].kind,"voidSym")!=0 )
						nextsym();
					fprintf(ErrorOut,"������һ��int��char��void\n");
					break;
			   }
		case 4:{	//������һ�� }
					while( strcmp(Str[index].kind,"R_B_Par")!=0)
						nextsym();
					fprintf(ErrorOut,"������һ�� }\n");
					break;
			   }
		case 5:{	
					while( strcmp(Str[index].kind,"Assign")!=0)
						nextsym();
					fprintf(ErrorOut,"������һ���Ⱥ�\n");
					break;
			   }
		case 6:{	//������һ��const��int��char��void
					while( strcmp(Str[index].kind,"constSym")!=0 && strcmp(Str[index].kind,"intSym")!=0 && 
						   strcmp(Str[index].kind,"charSym")!=0 && strcmp(Str[index].kind,"voidSym")!=0 )
						nextsym();
					fprintf(ErrorOut,"������һ��const��int��char��void\n");
					break;
			   }
		case 7:{	//������һ��const��int��char������ͷ����
					while( !(strcmp(Str[index].kind,"constSym")==0 || strcmp(Str[index].kind,"intSym")==0 || 
						strcmp(Str[index].kind,"charSym")==0 || strcmp(Str[index].kind,"ifSym")==0 || 
						strcmp(Str[index].kind,"doSym")==0 || strcmp(Str[index].kind,"switchSym")==0 ||
						strcmp(Str[index].kind,"L_B_Par")==0 || (strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0) ||
						(strcmp(Str[index].kind,"IdSym")==0 && (strcmp(Str[index+1].kind,"Assign")==0 || strcmp(Str[index+1].kind,"L_M_Par")==0)) ||
						strcmp(Str[index].kind,"scanfSym")==0 || strcmp(Str[index].kind,"printfSym")==0 || strcmp(Str[index].kind,"Semi")==0 ||
						strcmp(Str[index].kind,"returnSym")==0 || strcmp(Str[index].kind,"R_B_Par")==0) )	//"R_B_Par"��ʾ���main�Ĵ����Ž�β
							nextsym();
					fprintf(ErrorOut,"������һ��const��int��char������ͷ����\n");
					break;
			   }
		case 8:{	//������һ���ֺŵĺ�һ��λ��
					while( strcmp(Str[index].kind,"Semi")!=0)
						nextsym();
					nextsym();	//������һ���ֺŵĺ�һ��λ��
					fprintf(ErrorOut,"������һ���ֺŵĺ�һ��λ��\n");
					break;
			   }
	}
	fprintf(ErrorOut,"-*-*-*-*-*-*-*-*-*-\n");
	ErrorFlag=0;	//ÿ�����궼��ErrorFlag��Ϊ0
}