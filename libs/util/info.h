
#ifndef INFO_H
#define INFO_H

void cpu_get_vendor(char *vendor);
void cpu_get_brand(char *brand);
unsigned int cpu_get_cores(void);
unsigned int ram_get_total_kb(void);
unsigned int ram_get_total_mb(void);
unsigned int info_get_boot_magic(void);

#endif
