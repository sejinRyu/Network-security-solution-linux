#include <stdio.h>

void gnuplot(const char *gnucommand)
{
	char syscommand[1024];
 	sprintf(syscommand, "echo \"%s\" | gnuplot -persist", gnucommand);
    system(syscommand); 
}   

main()
{
	gnuplot("plot '.tmp.dat' with linespoints");
	gnuplot("exit");
}
