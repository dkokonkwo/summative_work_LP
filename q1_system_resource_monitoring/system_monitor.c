#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include "monitor.h"

#define USAGE_FILE "usage.txt"
#define PROCESSES_FILE "processes.txt"

// Function to get CPU usage percentage
double get_cpu_usage(void) 
{
    static long prev_idle = 0, prev_total = 0;
    long idle, total, user, nice, system, idle_cpu, iowait, irq, softirq, steal;

    FILE *file = fopen("/proc/stat", "r");
    if (!file)
    {
        perror("Failed to open /proc/stat");
        return -1;
    }

    fscanf(file, "cpu  %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle_cpu, &iowait, &irq, &softirq, &steal);
    fclose(file);

    idle = idle_cpu + iowait;
    total = user + nice + system + idle + irq + softirq + steal;

    double usage = (double)(total - prev_total - (idle - prev_idle)) / (total - prev_total) * 100.0;

    prev_idle = idle;
    prev_total = total;

    return usage;
}

// Function to get memory usage percentage
double get_memory_usage(void)
{
    long total_memory, free_memory, available_memory;
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file)
    {
        perror("Failed to open /proc/meminfo");
        return -1;
    }

    fscanf(file, "MemTotal: %ld kB\nMemFree: %ld kB\nMemAvailable: %ld kB\n",
           &total_memory, &free_memory, &available_memory);
    fclose(file);

    double usage = (double)(total_memory - available_memory) / total_memory * 100.0;
    return usage;
}

// Function to get network usage in KB
long get_network_usage(void)
{
    long rx_bytes = 0, tx_bytes = 0;
    char line[256];
    FILE *file = fopen("/proc/net/dev", "r");
    if (!file)
    {
        perror("Failed to open /proc/net/dev");
        return -1;
    }

    // Skip the first two lines
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file))
    {
        char iface[32];
        long rx, tx;
        sscanf(line, "%s %ld %*d %*d %*d %*d %*d %*d %ld", iface, &rx, &tx);
        rx_bytes += rx;
        tx_bytes += tx;
    }

    fclose(file);
    return (rx_bytes + tx_bytes) / 1024; // Convert to KB
}

// Function to list all active processes
void list_processes(void)
{
    FILE *file = fopen(PROCESSES_FILE, "w");
    if (!file) {
        perror("Failed to open processes.txt");
        return;
    }

    DIR *proc_dir = opendir("/proc");
    if (!proc_dir)
    {
        perror("Failed to open /proc");
        fclose(file);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)))
    {
        if (entry->d_type == DT_DIR) {
            // Check if the directory name is a number (process ID)
            if (atoi(entry->d_name) > 0) {
                fprintf(file, "PID: %s\n", entry->d_name);
            }
        }
    }

    closedir(proc_dir);
    fclose(file);
}

int system_monitor(int runtime)
{
    if (runtime  <= 5)
    {
        printf("Proccess should run longer than 5 times");
        return 0;
    }

    while (runtime != 0) 
    {
        double cpu_usage = get_cpu_usage();
        double mem_usage = get_memory_usage();
        long net_usage = get_network_usage();

        // Write metrics to usage.txt
        FILE *usage_file = fopen(USAGE_FILE, "a");
        if (!usage_file)
        {
            perror("Failed to open usage.txt");
            return 1;
        }

        time_t now = time(NULL);
        fprintf(usage_file, "Timestamp: %s", ctime(&now));
        printf("Timestamp: %s\n", ctime(&now));
        fprintf(usage_file, "CPU Usage: %.2f%%\n", cpu_usage);
        printf("CPU Usage: %.2f%%\n", cpu_usage);
        fprintf(usage_file, "Memory Usage: %.2f%%\n", mem_usage);
        printf("Memory Usage: %.2f%%\n", mem_usage);
        fprintf(usage_file, "Network Usage: %ld KB\n\n", net_usage);
        printf("Network Usage: %ld KB\n\n", net_usage);
        fclose(usage_file);

        // Update processes list
        list_processes();

        sleep(2);
        runtime--;
    }

    return 1;
}
