#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#define MATCH_SIZE 1
#define MAC_SIZE 18
#define SIZE 128
#define BUFFER_SIZE 4096

char MACaddr[SIZE][MAC_SIZE];

void error(char* msg)
{
	printf("%s error!",msg);
	exit(1);
}

int _regex(char* pattern,char* str) 
{
       	int ret;
       	int cflags = REG_EXTENDED;
      	regex_t reg;
	
	//printf("regexp(%s),string(%s)\n", pattern, str);
      	ret = regcomp(&reg, pattern, cflags); //reg compile
      	if (ret != 0) {
        	char errStr[SIZE];
         	regerror(ret, &reg, errStr, sizeof(errStr));
         	printf("regcomp error(%s)\n", errStr);
         	exit(1);
       	}

      	int eflags = 0, len;
       	int offset = 0, cnt = 1;

       	regmatch_t pmatch;
       	while((ret = regexec(&reg, str+offset, MATCH_SIZE, &pmatch, eflags)) == 0) 
       	{
       	      	len = pmatch.rm_eo - pmatch.rm_so;
		strncpy(MACaddr[cnt],str+offset+pmatch.rm_so,len);
       	      	//printf("matched string(%d:%.*s)\n", cnt, len, str+offset+pmatch.rm_so);
       	      	offset += pmatch.rm_eo;
       	      	cnt++;
       	}
       	regfree(&reg);
       	return cnt;	//list length
}

int check(int len)
{
	int i,j;
	for(i=0;i<len;i++)
		for(j=i+1;j<len;j++)
			if(strcmp(MACaddr[i],MACaddr[j])==0) return 1;
	return 0;
}

main()
{
	int  i=0,len=0;
	char* pattern="[0-9,a-f]\{2\}:[0-9,a-f]\{2\}:[0-9,a-f]\{2\}:[0-9,a-f]\{2\}:[0-9,a-f]\{2\}:[0-9,a-f]\{2\}";
	int readSize=0;	
	FILE* fp=NULL;
	char buf[BUFFER_SIZE];
	
		memset(buf,0x00,BUFFER_SIZE);
	if(!(fp=popen("arp -a","r"))) error("open");
	if((readSize=fread((void*)buf,sizeof(char),BUFFER_SIZE,fp))==0) error("read");
	pclose(fp);
	len=_regex(pattern,buf);
		for(i=0;i<len;i++)
		if(strlen(MACaddr[i])) puts(MACaddr[i]);
	if(check(len)) puts("attack detected!");
	else puts("safe!");
	
}
