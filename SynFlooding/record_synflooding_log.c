#include <stdio.h>

int main(int argc,char** argv)
{
	FILE* fp=NULL;
	char strtmp[3];
	int syn_recv=0,time=0;
	
	system("echo >.tmp.dat");
	while(1)
	{
		fp=popen("netstat -na|grep SYN_RECV|wc -l","r");
		memset(strtmp,0x00,3);
		fgets(strtmp,3,fp);
		pclose(fp);
	
		syn_recv=atoi(strtmp);
		printf("%d %d\n",time,syn_recv);

		fp=fopen(".tmp.dat","a");
		fseek(fp,0,SEEK_END);
		fprintf(fp,"%d %d\n",time++,syn_recv);
		fclose(fp);
		sleep(1);
	}
}


