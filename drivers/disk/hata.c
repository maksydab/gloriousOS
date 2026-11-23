#include "ata.h"
#include "../../libs/util/io.h"

#define ATA_DATA       0x1F0
#define ATA_ERROR      0x1F1
#define ATA_SECCOUNT   0x1F2
#define ATA_LBA_LOW    0x1F3
#define ATA_LBA_MID    0x1F4
#define ATA_LBA_HIGH   0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_STATUS     0x1F7
#define ATA_COMMAND    0x1F7

#define ATA_CMD_READ   0x20
#define ATA_CMD_WRITE  0x30

#define ATA_SR_BSY     0x80
#define ATA_SR_DRQ     0x08
#define ATA_SR_ERR     0x01
#define ATA_SR_DF      0x20

static bool ata_wait(bool require_drq) {
    for (uint32_t i = 0; i < 1000000; i++) {
        uint8_t status = inb(ATA_STATUS);

        if (status == 0 || status == 0xFF)
            return false;

        if (status & (ATA_SR_ERR | ATA_SR_DF))
            return false;

        if (!(status & ATA_SR_BSY)) {
            if (!require_drq || (status & ATA_SR_DRQ))
                return true;
        }
    }

    return false;
}

static void ata_select(uint32_t lba) {
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));

    // 400 ns delay
    inb(ATA_STATUS);
    inb(ATA_STATUS);
    inb(ATA_STATUS);
    inb(ATA_STATUS);
}

bool ata_write_sector(uint32_t lba, const uint8_t *buffer) {
    if (!buffer || lba >= 0x10000000U)
        return false;

    ata_select(lba);

    if (!ata_wait(false))
        return false;

    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_CMD_WRITE);

    if (!ata_wait(true))
        return false;

    for (uint32_t i = 0; i < 256; i++) {
        uint16_t word =
            (uint16_t)buffer[i * 2] |
            ((uint16_t)buffer[i * 2 + 1] << 8);

        __asm__ volatile (
            "outw %0, %1"
            :
            : "a"(word), "Nd"((uint16_t)ATA_DATA)
        );
    }

    if (!ata_wait(false))
        return false;

    return true;
}

bool ata_read_sector(uint32_t lba, uint8_t *buffer) {
    if (!buffer || lba >= 0x10000000U)
        return false;

    ata_select(lba);

    if (!ata_wait(false))
        return false;

    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_CMD_READ);

    if (!ata_wait(true))
        return false;

    for (uint32_t i = 0; i < 256; i++) {
        uint16_t word;

        __asm__ volatile (
            "inw %1, %0"
            : "=a"(word)
            : "Nd"((uint16_t)ATA_DATA)
        );

        buffer[i * 2] = (uint8_t)word;
        buffer[i * 2 + 1] = (uint8_t)(word >> 8);
    }

    return ata_wait(false);
}