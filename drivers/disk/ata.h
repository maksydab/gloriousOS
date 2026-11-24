#ifndef GLORIOUS_ATA_H
#define GLORIOUS_ATA_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ATA_SECTOR_SIZE 512

/* let any person who says disk handlign is easy you should hit them in the face */
bool ata_init(void);

/* total of LBA28 sectors on the drive (0 if init failed and thats bad). */
uint32_t ata_sector_count(void);

/* single-sector I/O. lba must be < ata_sector_count(). */
bool ata_read_sector(uint32_t lba, uint8_t *buf);
bool ata_write_sector(uint32_t lba, const uint8_t *buf);

/* flush drive's internal write cache. */
bool ata_flush(void);

#endif