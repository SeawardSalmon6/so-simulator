#ifndef OS_SIMULATOR_DISK_H
#define OS_SIMULATOR_DISK_H

#include "../process/process.h"

#define DISK_REQUEST_DURATION (5000);

void disk_read_request(process_t *, int);
void disk_write_request(process_t *, int);

#endif // OS_SIMULATOR_DISK_H
