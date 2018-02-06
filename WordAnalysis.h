#ifndef __WordAnalysis_H__
#define __WordAnalysis_H__

extern FILE* fp;//引用输入文件
extern FILE* stream;//引用输出文件

extern int i;
extern int flag;
extern int line;
extern int lineflag;
extern int TextLine;
extern char alpha[1024];
extern char token[1024][30];
extern char id[13][10];

void lineCheck();
int isLetter(char a);
int isDigit(char a);
void ungetcha();
char getcha();
void Str_CH(char c);
int isSym(char c);
int compare(int h);
void print(int k);
void printSym(int d);
int WordAnalysis();

#endif  /*__WordAnalysis_H__*/