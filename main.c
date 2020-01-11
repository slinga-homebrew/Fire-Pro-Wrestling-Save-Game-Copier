#include "sgl.h" // Required for basic sgl functions
#include "sgl_cd.h" // Required for cd access
#include "sega_bup.h" // Required for backup memory functions
#include "stdlib.h" // Don't know why I need this...don't feel like burning another cd to find out

#define BUP_START 0x6070000 // In the sgl doc's they say to put 0x06????? but that doesn't compile...
#define MAX_FILE 128 // Needed for CD functions
#define READSECT 50 // Needed for CD functions

Uint32 workmemory[2048]; // Needed for BackUpFunctions
Sint32 dirwork[SLCD_WORK_SIZE(MAX_FILE)/sizeof(Sint32)]; // Needed for CD function
Uint8 readbuf[(READSECT*CDBUF_FORM1/sizeof(Uint8))]; // Needed for CD function
 
void ss_main(void)
{
 
	Sint32 ndir;
	CDHN cdhn;
	CDKEY key[2];
	CDBUF buf[2];
	Sint32 stat;
	Sint32 len[2];
	Sint32 ypos=1;

	BupConfig conf[3]; // stores backup configuration
	BupStat sttb; // stores backup status
	BupDir writetb; // writeb holds file data
	

	slInitSystem(TV_320x224, NULL, 1); // Initializes screen
	
	ndir=slCdInit(MAX_FILE, dirwork); // initializes cd
	slPrint("SlCdInit:", slLocate(1, ypos));
	slPrintFX(toFIXED(ndir), slLocate(11, ypos));
	ypos++;

	key[0].cn=key[0].sm=key[0].ci=CDKEY_NONE;
	key[1].cn=CDKEY_TERM;
	cdhn=slCdOpen("FPS_6MEN.DAT", key); // opens cd
	slPrint("slCdOpen:", slLocate(1,ypos));
	slDispHex((Uint32)cdhn, slLocate(11, ypos));

	buf[0].type=CDBUF_COPY;
	buf[0].trans.copy.addr=readbuf;
	buf[0].trans.copy.unit=CDBUF_FORM1;
	buf[0].trans.copy.size=READSECT;
	buf[1].type=CDBUF_TERM;
	slCdLoadFile(cdhn,buf); // loads file from cd
	ypos++;

	
	while(stat=CDSTAT_COMPLETED)
	{
		slSynch();
		stat=slCdGetStatus(cdhn, len);
		slPrint("stat:", slLocate(1, ypos));
		slDispHex((Uint32)stat, slLocate(7, ypos));
		ypos++;
		if(ypos>=27) ypos=1;
		if(stat==CDSTAT_COMPLETED) break;
					
	}

	slPrint("Save loaded...", slLocate(1,ypos++));
	BUP_Init(BUP_START, workmemory, conf); // Initializes back up cart
	slPrint("Back up initialized...", slLocate(1,ypos++));   
			
	strcpy((char *)writetb.filename, "FPS_6MEN_01"); // Specifies filename for the save
	strcpy((char *)writetb.comment, "Test6"); // Specifies comment for the save
	writetb.language = BUP_JAPANESE; // not sure what this does?? language probably
	writetb.datasize = 20308; // Specifies file size, this is not working. 

	slPrint("Writing to internal ram....", slLocate(1, ypos++));
	BUP_Write(0, &writetb, readbuf, 0); // This should copy the file to internal memory but doesn't...
	slPrint("Done.", slLocate(1,ypos++)); // Prints done to the screen
	slPrint("Special thanks to:", slLocate(1,ypos++));
	slPrint("Anitime, Charles Doty, CyberWarriorX,", slLocate(1,ypos++));
	slPrint("ExCyber, Reinhart, and TakaIsSilly", slLocate(1,ypos++));
	
	while(1)
	{
		slSynch();
	}


}
