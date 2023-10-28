//sudo time /opt/toolchains/dc/bin/dc-tool-ser -t /dev/ttyUSB0 -b 781250 -c . -x /mnt/SSD1TB/src/SPDump/SPDump.elf 

#include <kos.h>
//#include <time.h>

#define RAM_DEST 0x8C400000


// taken from kos examples/dreamcast/kgl/demos/specular
//static uint32 s, ms;
//static uint64 msec;
/* Get current hardware timing using arch/timer.h */
/*uint64 GetTime(void) {
    timer_ms_gettime(&s, &ms);
    msec = (((uint64)s) * ((uint64)1000)) + ((uint64)ms);
    return msec;
}*/

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
 //int readlen = 0x10000 * 2;//16777216;
 int numBanks = bytesInOneChip / 0x10000;
 uint8_t* pdst = (uint8_t*)RAM_DEST;

 printf("file %s:\n", filename);

 printf("fetching ROM data...\n");
 for (int i=0; i < numBanks; i++){
    getROMData ((uint16_t*)pdst, 0x10000 /2);
	curBank++;
	setBank(curBank);
	pdst += 0x10000;
 }

 //write the data to PC file
 fout = fopen (filename, "wb");
 if (!fout){
	printf("unable to open file %s...\n", filename);
	return;
 } else {
	printf("writing output file...\n");
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
 //uint64 start, end;
 //time_t start, end;
 //unsigned int seconds;

 printf("starting SPDump...\n");

 //start = GetTime();
 //start = time(NULL);

 dumpFlashRomData (128, 3);//bingogals
 //dumpFlashRomData (128, 8);//bingogal

 //display stats
 //end = GetTime();
 /*end = time(NULL);
 seconds = (unsigned int) ((end - start));/// (uint64)1000);
 printf ("Time taken: %i seconds\n", seconds);
 if (seconds) printf ("Speed: %i kbytes/s\n", 16384 / seconds);*/

 printf("ending SPDump...\n");
 
 return (0);
}