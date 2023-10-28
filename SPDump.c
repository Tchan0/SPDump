//How to run:
//  sudo /opt/toolchains/dc/bin/dc-tool-ser -t /dev/ttyUSB0 -b 781250 -c . -x /mnt/SSD1TB/src/SPDump/SPDump.elf 

#include <kos.h>

#define RAM_DEST 0x8C400000

void getROMData (uint16_t* pdst, int len){
 uint16_t* psrc = (uint16_t*)0xA1000000;

 while (len--){
    *pdst++ = *(uint16_t*)psrc;
    psrc++;
 }
}


void setBank (uint32_t bankNr){
 volatile uint32_t* spbankreg = (volatile uint32_t*)0xA1010000;

 *spbankreg = bankNr;
}


int curBank = 0;

void dumpOneChip (char* filename, uint32_t offset, uint32_t bytesInOneChip){
 FILE *fout;
 int numBanks = bytesInOneChip / 0x10000;
 uint8_t* pdst = (uint8_t*)RAM_DEST;

 printf("file %s:\n", filename);
 fout = fopen (filename, "wb");
 if (!fout){
    printf("unable to open file %s...\n", filename);
    return;
 } else {
    printf("writing output file...\n");
 }

 printf("fetching ROM data...\n");
 for (int i=0; i < numBanks; i++){
    getROMData ((uint16_t*)pdst, 0x10000 /2);
    curBank++;
    setBank(curBank);
    pdst += 0x10000;
 }
 
 fwrite ((void*)RAM_DEST, bytesInOneChip, 1, fout);

 fclose (fout);
 printf("file %s closed.\n", filename);
}

void dumpFlashRomData (int mbitsize, int numChips){
 int bytesInOneChip = (mbitsize / 8) * 1024 * 1024;
 char* filename;
 uint32_t offset = 0;

 for (int i = 0; i < numChips; i++){
    switch (i){
        case 0:
            filename = "/pc/ic62";
            break;
        case 1:
            filename = "/pc/ic63";
            break;
        case 2:
            filename = "/pc/ic64";
            break;
        case 3:
            filename = "/pc/ic65";
            break;
        case 4:
            filename = "/pc/ic66s";
            break;
        case 5:
            filename = "/pc/ic67s";
            break;
        case 6:
            filename = "/pc/ic68s";
            break;
        case 7:
            filename = "/pc/ic69s";
            break;
        default:
            return;
	}
    
    dumpOneChip (filename, offset, bytesInOneChip);
    offset += bytesInOneChip;
 }

}

int main(int argc, char *argv[]) {
 printf("starting SPDump...\n");

 //WARNING: this program copies the flash data into RAM, so it doesn't work yet with 512Mbits chips (easy fix needs to be implemented still)
 dumpFlashRomData (128, 3);//bingogals
 //dumpFlashRomData (128, 8);//bingogal

 printf("ending SPDump...\n");
 
 return (0);
}
