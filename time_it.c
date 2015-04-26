#include <time_it.h>
#include <stdio.h>


void time_it_start(time_it *t)
{
  t->start_time = times(&(t->start_cpu));
}

void time_it_stop(time_it *t)
{
  t->end_time = times(&(t->end_cpu));
}

void time_it_report(time_it *t,char *tag)
{
printf("Real Time %s: %lf\nUser CPU Time %s: %lf\nSystem CPU Time %s: %lf\n",
	 tag,
	 ((double)t->end_time -t-> start_time)/sysconf(_SC_CLK_TCK),
	 tag,
	 ((double)t->end_cpu.tms_utime - t->start_cpu.tms_utime)/
	 sysconf(_SC_CLK_TCK),
	 tag,
	 ((double)t->end_cpu.tms_stime - t->start_cpu.tms_stime)/
	 sysconf(_SC_CLK_TCK));
}
