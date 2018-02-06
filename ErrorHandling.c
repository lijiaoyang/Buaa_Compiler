#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>
#include"WordAnalysis.h"
#include "grammar.h"
#include"ErrorHandling.h"

int ErrorNum = 0;
int ErrorFlag=0;
FILE * ErrorOut;//引用输出文件
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
	fprintf(ErrorOut,"错误可能在第 %d 行 类型是%s 名字是%s\n",Str[index-1].OriLine,Str[index-1].kind,Str[index-1].name);
	ErrorNum++;//每处理完一个错误，错误数量加1
	//exit(0);
	ErrorFlag = e;
}

void Jump(int r)
{
	switch(r)
	{
		case 1:{	//缺少分号		6    跳到下一个分号
					while(strcmp(Str[index].kind,"Semi")!=0)
						nextsym();
					fprintf(ErrorOut,"跳到下一个分号\n");
					break;
			   }
		case 2:{	//跳到下一个逗号或分号
					while(strcmp(Str[index].kind,"Semi")!=0 && strcmp(Str[index].kind,"Comma")!=0)
						nextsym();
					fprintf(ErrorOut,"跳到下一个逗号或分号\n");
					break;
			   }
		case 3:{	//跳到下一个int或char或void
					while( strcmp(Str[index].kind,"intSym")!=0 && strcmp(Str[index].kind,"charSym")!=0
						  && strcmp(Str[index].kind,"voidSym")!=0 )
						nextsym();
					fprintf(ErrorOut,"跳到下一个int或char或void\n");
					break;
			   }
		case 4:{	//跳到下一个 }
					while( strcmp(Str[index].kind,"R_B_Par")!=0)
						nextsym();
					fprintf(ErrorOut,"跳到下一个 }\n");
					break;
			   }
		case 5:{	
					while( strcmp(Str[index].kind,"Assign")!=0)
						nextsym();
					fprintf(ErrorOut,"跳到下一个等号\n");
					break;
			   }
		case 6:{	//跳到下一个const或int或char或void
					while( strcmp(Str[index].kind,"constSym")!=0 && strcmp(Str[index].kind,"intSym")!=0 && 
						   strcmp(Str[index].kind,"charSym")!=0 && strcmp(Str[index].kind,"voidSym")!=0 )
						nextsym();
					fprintf(ErrorOut,"跳到下一个const或int或char或void\n");
					break;
			   }
		case 7:{	//跳到下一个const或int或char或语句的头符号
					while( !(strcmp(Str[index].kind,"constSym")==0 || strcmp(Str[index].kind,"intSym")==0 || 
						strcmp(Str[index].kind,"charSym")==0 || strcmp(Str[index].kind,"ifSym")==0 || 
						strcmp(Str[index].kind,"doSym")==0 || strcmp(Str[index].kind,"switchSym")==0 ||
						strcmp(Str[index].kind,"L_B_Par")==0 || (strcmp(Str[index].kind,"IdSym")==0 && strcmp(Str[index+1].kind,"L_S_Par")==0) ||
						(strcmp(Str[index].kind,"IdSym")==0 && (strcmp(Str[index+1].kind,"Assign")==0 || strcmp(Str[index+1].kind,"L_M_Par")==0)) ||
						strcmp(Str[index].kind,"scanfSym")==0 || strcmp(Str[index].kind,"printfSym")==0 || strcmp(Str[index].kind,"Semi")==0 ||
						strcmp(Str[index].kind,"returnSym")==0 || strcmp(Str[index].kind,"R_B_Par")==0) )	//"R_B_Par"表示后跟main的大括号结尾
							nextsym();
					fprintf(ErrorOut,"跳到下一个const或int或char或语句的头符号\n");
					break;
			   }
		case 8:{	//跳到下一个分号的后一个位置
					while( strcmp(Str[index].kind,"Semi")!=0)
						nextsym();
					nextsym();	//跳到下一个分号的后一个位置
					fprintf(ErrorOut,"跳到下一个分号的后一个位置\n");
					break;
			   }
	}
	fprintf(ErrorOut,"-*-*-*-*-*-*-*-*-*-\n");
	ErrorFlag=0;	//每次跳完都把ErrorFlag设为0
}