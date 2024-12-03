#ifndef MONITOR_H
#define MONITOR_H

int system_monitor(int runtime);
void list_processes(void);
long get_network_usage(void);
double get_memory_usage(void);
double get_cpu_usage(void);

#endif // MONITOR_H