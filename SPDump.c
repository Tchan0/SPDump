//How to run:
//  sudo /opt/toolchains/dc/bin/dc-tool-ser -t /dev/ttyUSB0 -b 781250 -c . -x /mnt/SSD1TB/src/SPDump/SPDump.elf 

#include <kos.h>
#include <stdlib.h>

// Modify the following lines to match the flash configuration
// used on your SystemSP board:

#define FLASH_SIZE_MBIT     512  // 512 MBit flash chip
// #define FLASH_SIZE_MBIT     128  // 128 MBit flash chip

#define FLASH_CHIP_COUNT    2    // Only 62 & 63 are populated
// #define FLASH_CHIP_COUNT    8    // All 8 flash chips are populated

// End of configuration


#define BANK_SIZE           0x10000
#define FLASH_SIZE_BYTE     (FLASH_SIZE_MBIT * 1024 * 1024 / 8)
#define BANKS_PER_FLASH     (FLASH_SIZE_BYTE / BANK_SIZE)
#define BANKS_PER_XFER      32

static const char * CHIP_NAMES[8] = { "62", "63", "64", "65", "66s", "67s", "68s", "69s" };
static volatile uint32_t* FLASH_BASE = (volatile uint32_t*)0xA1000000;
static volatile uint32_t* FLASH_BANK_REG = (volatile uint32_t*)0xA1010000;
__attribute__((aligned(4)))
static uint8_t DATA_BUFFER[(BANK_SIZE * BANKS_PER_XFER)] = {};

const char * getChipName (int chipIndex){
 if (chipIndex > 8){
    printf("invalid chip index %d\n", chipIndex);
    exit(-1);
 }
 return CHIP_NAMES[chipIndex];
}

void setBank (uint32_t bankNr){
 *FLASH_BANK_REG = bankNr;
}

void copySingleBank (uint8_t* pdst, int bankNum, int len){    
 setBank(bankNum);
 memcpy(pdst, (void*) FLASH_BASE, len);
}

void copyMultiBanks(uint8_t* pdst, int startBank, int bankCount){
 for (int i = 0; i < bankCount; i++){
    copySingleBank(pdst, startBank + i, BANK_SIZE);
    pdst += (BANK_SIZE);
 }
}

void dumpOneChip (const char* filename, int chipIndex){
 const int bankOffset = chipIndex * BANKS_PER_FLASH;
 FILE *fout;
 
 printf("file %s:\n", filename);
 fout = fopen (filename, "wb");
 if (!fout){
    printf("unable to open file %s...\n", filename);
    return;
 }

 printf("fetching ROM data for ic%s...\n", getChipName(chipIndex));
 for (int curBank=0; curBank < BANKS_PER_FLASH; curBank += BANKS_PER_XFER) {
    printf("reading bank %d of %d...", curBank, BANKS_PER_FLASH);
    fflush(stdout);
    copyMultiBanks(DATA_BUFFER, bankOffset + curBank, BANKS_PER_XFER);
    fwrite (DATA_BUFFER, BANK_SIZE * BANKS_PER_XFER, 1, fout);
    printf("done.\n");
 }
 
 fclose (fout);
 printf("file %s closed.\n", filename);
}

void dumpFlashRomData (){
 for (int i = 0; i < FLASH_CHIP_COUNT; i++){
    char filename[32];
    sprintf(filename, "/pc/ic%s", getChipName(i));
    dumpOneChip (filename, i);
 }
}

int main(int argc, char *argv[]){
 printf("\nstarting SPDump...\n");
 dumpFlashRomData();
 printf("ending SPDump...\n");
 
 return (0);
}
