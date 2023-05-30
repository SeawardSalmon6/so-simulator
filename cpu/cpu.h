#ifndef OS_SIMULATOR_CPU_H
#define OS_SIMULATOR_CPU_H

#define ONE_SECOND_IN_NS (1000000000L)

void cpu_init(void);
void *cpu(void *);

#endif // OS_SIMULATOR_CPU_H
