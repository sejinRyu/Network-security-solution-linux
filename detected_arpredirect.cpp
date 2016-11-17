#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cstdio>

using namespace std;

int main(int argc,char* argv[])
{
	FILE* fp=popen("arp -a | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'","r");
	list<string> macAddr;
	list<string>::iterator iter;
	char tmpMac[18];
	int before,after;

	while(!feof(fp))
	{
		fgets(tmpMac,18,fp);
		macAddr.push_back(tmpMac);
	}
		pclose(fp);
	
	macAddr.remove("\n");
	for(iter=macAddr.begin(); iter != macAddr.end(); iter++)
		cout<<*iter<<endl;
	before=macAddr.size();
	macAddr.sort();
	macAddr.unique();
	after=macAddr.size();
	
	if(before==after)
		cout<<"safe!"<<endl;
	else
		cout<<"attack detected!"<<endl;

	return 0;
}
