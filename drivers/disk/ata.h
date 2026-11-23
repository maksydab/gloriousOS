#ifndef GLORIOUS_ATA_H
#define GLORIOUS_ATA_H

#include <stdint.h>
#include <stdbool.h>

#define ATA_SECTOR_SIZE 512

bool ata_write_sector(uint32_t lba, const uint8_t *buffer);
bool ata_read_sector(uint32_t lba, uint8_t *buffer);

#endif