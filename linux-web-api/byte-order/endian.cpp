#include<stdio.h>


int main()
{
	union{
		short a;
		char b[sizeof(short)];
	}data;
	
	data.a = 0x0102;
	if(data.b[0]==1 && data.b[1]==2) printf("big endian\n");
	else if(data.b[0]==2 && data.b[1]==1) printf("little endian\n");
	else printf("unknown\n");
	
	return 0;
}
