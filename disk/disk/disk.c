/**
 * Name: Jonathan Ginsburg
 * Lab/Task: Project 2
 * Date: 04/23/19
 **/

#include "disk.h"

disk_t disk;

int log2phys(int logaddr, physaddr_t *phaddr)
{
    // TODO: implement DONE
    // check to make sure the logical address is valid
    if  (logaddr < 0 && logaddr > MAX_LOGICAL_SECTOR)
        {
            printf("ERROR: Invalid logical address");
            return DISK_ERROR;
        }
    phaddr->cyl = (logaddr / SECT_SIZE) + 1;
    phaddr->head = ((logaddr % SECT_SIZE) / NUM_OF_SECTS) + 1;
    phaddr->sect = (logaddr % NUM_OF_SECTS) + 1;
    return DISK_SUCCESS;
}

int phys2log(physaddr_t *phaddr)
{
    // TODO: implement DONE

    // reverse the implementation of logical address
    return (((phaddr->head - 1) * NUM_OF_SECTS) + (phaddr->sect - 1) + ((phaddr->cyl - 1) * SECT_SIZE));
}

void printTransl(int logaddr)
{
    physaddr_t phaddr;

    printf("Logical sector number: %d\n", logaddr);
    if (log2phys(logaddr, &phaddr) == 0)
    {
        printf("Physical address: (%d, %d, %d)\n", phaddr.cyl, phaddr.head, phaddr.sect);

        if (phys2log(&phaddr) != logaddr)
            printf("ERROR: no match!\n");
    } else
        printf("ERROR: invalid logical address!\n");
}

// copy the memory
void copySector(char * dest, const char * src, int size)
{
    for (int i =0; i<size; ++i)
        dest[i] = src[i];
}

void printSectorContents(char * sector)
{
    for (int i =0; i<SECT_SIZE; ++i)
    {
        if (i %64 == 0)
            printf(" \n");
        printf("%d ", (int)sector[i]);
    }
}

void printSector(physaddr_t phyaddr)
{
    printf("disk[%d][%d][%d]: ", phyaddr.cyl, phyaddr.head, phyaddr.sect);
    printSectorContents(disk[phyaddr.cyl][phyaddr.head][phyaddr.sect]);
}

int readDisk(int logical_block_num, int numOfBlocks, void **buffer)
{
    // TODO: implement DONE
    // find number of blocks, create an array to hold that and then add to that array
    // numOfBlocks is the number of sectors we want to copy
    // logical_block_num is the starting address we wish to use
    // buffer is what we are using to store the information from the disk

    // create the array
    // as long as the starting address is within our bounds run through the disk and
    // copy the information at each sector into the buffer
    if (logical_block_num < 0)
        return DISK_ERROR;
    if (logical_block_num + numOfBlocks < MAX_LOGICAL_SECTOR)
        return DISK_ERROR;

    physaddr_t phyaddr;
    for (int i = 0; i < numOfBlocks; ++i)
    {
        log2phys(logical_block_num + i, &phyaddr);
        char* blockBuffer = (*buffer);
        copySector(&blockBuffer[i*SECT_SIZE], disk[phyaddr.cyl][phyaddr.head][phyaddr.sect], SECT_SIZE);
    }
    return DISK_SUCCESS;
}

int writeDisk(int logicalBlockNum, int numOfSectors, void *buffer)
{
    // TODO: implement DONE
        if (logicalBlockNum < 0)
            return DISK_ERROR;
        if (logicalBlockNum + numOfSectors >= MAX_LOGICAL_SECTOR)
            return DISK_ERROR;
    // create physical address object
    physaddr_t phyaddr;
    char* blockBuffer = buffer;

    for(int i = 0; i < numOfSectors; ++i)
        {
            log2phys(logicalBlockNum, &phyaddr);
            copySector(disk[phyaddr.cyl][phyaddr.head][phyaddr.sect], blockBuffer, SECT_SIZE);
        }
    return DISK_SUCCESS;
}

int main(int argc, char *argv[])
{
    // TODO: extend to also test for reading and writing
    physaddr_t phaddr;

    char buf[8 * SECT_SIZE];
    memset(buf, 69, 8 * SECT_SIZE);

    int logaddr;

    if (argc < 2)
        while (1)
        {
            logaddr = rand() % MAX_LOGICAL_SECTOR;
            printTransl(logaddr);
            switch (rand() % 2)
            {
                case 0:
                    readDisk(logaddr, rand() % 8, (void *) &buf);
                    break;
                case 1:
                    writeDisk(logaddr, rand() % 8, buf);
                    break;
            }
            usleep(10);
        }

    switch (argv[1][0]) // "l" for logical "p" for physical
    {
        case 'l': // logical
            if (argv[2] == NULL)
                break;
            logaddr = strtol(argv[2], NULL, 10) % MAX_LOGICAL_SECTOR;
            printTransl(logaddr);
            break;

        case 'p': // physical
            if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
                break;
            phaddr.cyl = strtol(argv[2], NULL, 10);
            phaddr.head = strtol(argv[3], NULL, 10);
            phaddr.sect = strtol(argv[4], NULL, 10);
            logaddr = phys2log(&phaddr);
            printTransl(logaddr);
            break;

        case 'r': // read
            if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
                break;
            phaddr.cyl = strtol(argv[2], NULL, 10);
            phaddr.head = strtol(argv[3], NULL, 10);
            phaddr.sect = strtol(argv[4], NULL, 10);
            logaddr = phys2log(&phaddr);
            writeDisk(logaddr, rand() % 8, buf);
            printSectorContents(buf);
            memset(buf, 0, 8*SECT_SIZE);
            printSectorContents(buf);
            readDisk(logaddr, rand() % 8, (void *) &buf);
            break;

        case 'w': // write
            if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
                break;
            phaddr.cyl = strtol(argv[2], NULL, 10);
            phaddr.head = strtol(argv[3], NULL, 10);
            phaddr.sect = strtol(argv[4], NULL, 10);
            logaddr = phys2log(&phaddr);
            writeDisk(logaddr, rand() % 8, buf);
            printSector(phaddr);
            break;
    }
}