#include <windows.h>             /*Proudly to be compiled with MingW!*/
#include <scrnsave.h>
#include <string.h>
#include <stdio.h>
#include <zlib.h>
#include <dumb.h>
unsigned char *create_wav_header(unsigned long datasize, unsigned long frequency, int channels, int bitspersample, unsigned char *dt);
#define ITSIZE 95557
#define TIMER 1
#define WIN32_LEAN_AND_MEAN
#define SCREENSAVER_NAME  "EBScreenSaver"
#define SCREENSAVER_COMPANY "EvilPuffBall"
#define SIZE_OF_BKG 342646
#define DOWN 0    /*Test Phase 1: Make Paula walk up and down left side of screen*/
#define LEFT 1
#define UP 2
#define RIGHT 3
#define SECONDSTEP 4
#define DOWNLEFT 8
#define UPLEFT 9
#define UPRIGHT 10             
#define DOWNRIGHT 11
#define PAULA 1      /*Not neccessarily in order of Coolniss*/
#define PJNESS 2     
#define NESS 4
#define POO 3
#define JEFF 5 
#define CONFIG  ("Software\\"EvilPuffBall"\\" EbScreenSaver)  /*Memo to self: I don't use this anymore*/


/*Size: 32 x 48*/
unsigned char *buffer = NULL;

//globals used by the function below to hold the screen size (and other stuff)
struct structsettings{
unsigned lastdir : 4;   /*I only need 4 bits to store this variable, so I only use 4 bits*/
unsigned changedir : 1;   /*Not sure what it might be used for...eh, whatever*/
unsigned speed : 3;     
} directiondata[5]; /*Only one byte for each character*/

int soundcount=0;      /*Memo to self: Clean some of these now unused variables up*/
BYTE ys[8];            /*Why did I choose to use global variables almost exclusively?  The world may never know*/
BYTE high[8];
BYTE toobig = 0;
BYTE Icow;
int Width;
int Height;
unsigned char Delaystring[7];
unsigned int delay;
int tWidth, tHeight;
BYTE status = 0; BYTE ImageCount = 0;
BYTE ystart, xstart;
BYTE key;
HINSTANCE hInstance;
HANDLE Blue;
HBITMAP BattleBkg, Photo;
int xstart2, ystart2;
HBITMAP Flash;
HBITMAP FX;
BYTE BackgroundChangeCount = 0;
BYTE Background = 0;
unsigned long SizeOfBkgResource[4];
POINT DeletePoint[5]; /*  <- I'm lazy so I'm gonna store the old Coordinates*/
POINT DrawPoint[5];
BYTE direction[5]; /*This will store the directions they are going.*/
BYTE ws[8];
BYTE pguy;
BYTE Regbinarydata[4];
BYTE framemerge = 1;
BYTE fmframecount = 0;
long returns;
BYTE count = 0;
BYTE config1 = 0;
int px, py, pinit;
int hdat = 0;
BYTE pstat = 0;
signed char DirectionalOrder = 1;
HRSRC relisthrsrc, scripthrsrc;
HGLOBAL relist, script;

HBITMAP ExtractBkgBitmapResource(int val);
unsigned char *Rec_1(unsigned char *buffer);
unsigned char *ptfile;

int random(int max, int seeder)           /*A random number generator...very basic*/
{
   SYSTEMTIME gen;
   static seed = 1;
   float temp;
   int number;

   if(max == -1)
   {seed = seeder; return 0;}

   GetLocalTime(&gen);
   /*
   temp = (gen.wMilliseconds * 1.34765350008) + (gen.wSecond * 1.00000865) + (seed * 0.985) + (seed * 2.341188106 * max);
   temp = temp * 2;
      -Most Random so far*/

   temp = (gen.wMilliseconds * 1.34765350008) + (gen.wSecond * 1.00000865) + (seed * 0.985) + (seed * 2.341188106 * max * (seed*0.15));
   temp = temp * 2.1;

   if(max == 0)
   {return (int)temp;}

   seed++;
   if(seed > 1900)
   {seed = 1;}
   number = (long)temp % max;
   return number;
}


LONG WINAPI ScreenSaverProc(HWND hWnd, UINT message, 
                               WPARAM wParam, LPARAM lParam)
{
  static HDC hDC, Mem, BkgMem, tempfield, Batt;
  static RECT rect;
  PAINTSTRUCT ps;
  HPEN GreenPen;
  static unsigned char dat[ITSIZE];
 static DUMBFILE *memfile;
 static DUH *input_it;
 static DUH_SIGRENDERER *d_rend;
 static signed char *data;
 static signed short xtapi222[265171];
 static int increment = 1;
 static signed char *dat7;
 static BYTE numframes[8];
  switch ( message ) {

  case WM_CREATE: 
    // get window dimensions
    GetClientRect( hWnd, &rect );
    Width = rect.right;		
    Height = rect.bottom;
    rect.bottom = Width;
    m2 = FindResourceEx(NULL, "WAVE", MAKEINTRESOURCE(1000), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    ptfile = LoadResource(hMainInstance, m2);  /* Loads the music ImpulseTracker file... even though it's not really a waveform */
    
    
    if(Height > 768 && Width > 1024)
    {toobig = 1;
    tHeight = Height;
    tWidth = Width;
    Height = 768;
    Width = 1024;}

    /*RegCreateKeyEx( HKEY_CURRENT_USER, "EvilPuffBall - EBScreenSaver",0, "DWORD", 0x0000,KEY_ALL_ACCESS,NULL,RegConfig,&returns);
    returns = 4;
    RegQueryValueEx(HKEY_CURRENT_USER, "EvilPuffBall - EBScreenSaver", 0, NULL, Regbinarydata, &returns);
    RegCloseKey(HKEY_CURRENT_USER);*/
    GetProfileString("EvilPuffBall - EBScreenSaver", "config1", "00", Regbinarydata, 4);
    GetProfileString("EvilPuffBall - EBScreenSaver", "delay", "200", Delaystring, 6);
    sscanf(Delaystring, "%d", &delay);
    config1 = (Regbinarydata[0]-48)+((Regbinarydata[1]-48)*16); /*Loadage the configuration*/
    if( (config1&128) == 128)
    {toobig = 1;
    tHeight = Height;
    tWidth = Width;
    Height = 480;
    Width = 640;}

    /*if( (config1&64) == 64)
    {hdat = 1;}*/

    ys[0] = 67;    high[0] = 88;   /*117 pixels*/  ws[0] = 255;
    ys[1] = 47;    high[1] = 128;  /*171 pixels*/  ws[1] = 255;
    ys[2] = 57;    high[2] = 108;  /*144 pixels*/  ws[2] = 255;
    ys[3] = 57;    high[3] = 108;  /*144 pixels*/  ws[3] = 255;
    ys[6] = 67;    high[6] = 88;   /*155 pixels*/  ws[4] = 255;
    ys[4] = 67;    high[4] = 88;                   ws[5] = 255;
    ys[7] = 67;    high[7] = 88;                   ws[6] = 255;
    ys[5] = 67;    high[5] = 88;                   ws[7] = 255;
    numframes[0] = 60;
    numframes[1] = 69;
    numframes[2] = 52;
    numframes[3] = 59;
    numframes[4] = 48;
    numframes[5] = 58;
    numframes[6] = 63;
    numframes[7] = 53; /*Sizes and junk*/

    if( (config1&8) == 0)
    {
    ws[0] = 116;
    ws[1] = 170;
    ws[2] = 143;
    ws[3] = 143;
    ws[4] = ws[5] = ws[6] = ws[7] = ws[0];
    }
    Background = random(7,0);
    if( (config1&4) == 0)
    {/*PlaySound(MAKEINTRESOURCE(90), hMainInstance, SND_RESOURCE|SND_NODEFAULT|SND_ASYNC|SND_LOOP);*/
 
 
 if( (data=malloc( (sizeof(short) * 2121368) + 69 )) == NULL)
 {
    
    goto skipmusak;
 }
 
 memfile = dumbfile_open_memory(ptfile,ITSIZE);
 input_it = dumb_read_it(memfile);
 d_rend = duh_start_sigrenderer(input_it, 0,1,0);
 for(rect.right = 0; rect.right < 8; rect.right++)
 {
 duh_render(d_rend, 16, 0,1.0f,65536.0f/44100,265171,&xtapi222);
 memcpy(&data[44+(rect.right*530342)],&xtapi222,530342);
 }
 dat7 = &data[0];
 create_wav_header(4242736, 44100, 1, 16, dat7);
 sndPlaySound(dat7, SND_MEMORY|SND_ASYNC|SND_LOOP);        /*Render the ImpulseTracker module into a great big waveform and then play it on repeat*/
 skipmusak: ;
    }
    /*Okay, I think I'll initialize the POINT variables now, at least the "new" POINTs*/
    for(rect.right = 0; rect.right < 5; rect.right++)   /*Aren't I effiecient? ;)*/
    {
       DrawPoint[rect.right].x = rect.right * ((Width-32) / 5); /*Make the sprites start equidistant apart*/
       DrawPoint[rect.right].y = random(29,0); /*Top-o-Screen*/
       direction[rect.right] = DOWN;
       /*Init each character's speed too.*/
       directiondata[rect.right].speed = 5;
    }
    hDC = GetDC(hWnd);
    FX = CreateCompatibleBitmap(hDC, Width, Height);
    ReleaseDC(hWnd, hDC);

/*SizeOfBkgResource*/
   SizeOfBkgResource[0] = 60160;
   SizeOfBkgResource[1] = 74533;
   SizeOfBkgResource[2] = 28150;
   SizeOfBkgResource[3] = 33947;/*Don't use this anymore*/
/*LOAD THE SPRITES!*/
 Blue = LoadBitmap(hMainInstance,MAKEINTRESOURCE(7));
 Photo = LoadBitmap(hMainInstance,MAKEINTRESOURCE(80));
 Flash = LoadBitmap(hMainInstance,MAKEINTRESOURCE(81));
    /*LOAD BATTLE BACKGROUND!*/
  if( (config1&64) == 64)
  {hdat = 1; BattleBkg = LoadBitmap(hMainInstance, MAKEINTRESOURCE(556));goto stopload;}

  BattleBkg = LoadBitmap(hMainInstance,MAKEINTRESOURCE(900+Background));
  stopload: ;
    /* Start clicky-timer thingy*/
    SetTimer( hWnd, TIMER, delay, NULL );
    
    return 0;
  case WM_DESTROY:
    KillTimer( hWnd, TIMER );
    DeleteObject(Blue);
    DeleteObject(BattleBkg);
    DeleteObject(Photo);
    DeleteObject(Flash);
    DeleteObject(FX);
    DeleteObject(m3);
    dumb_exit();
 sndPlaySound(NULL, NULL);
    /*PlaySound(NULL, 0, SND_NODEFAULT);*/
    /* Clean up after myself... */
    return 0;
  case WM_TIMER:
    /* Change position and such.*/
    ImageCount += increment;

    if( (config1 & 16) != 0)
    {
    if(pinit == 10)
    {py += 15;pstat = (pstat+1)&3;}
    if(pinit == 11)
    {py -= 15;pstat = (pstat+1)&3;}
    if(pinit == 10 && py > Height - 56)
    {pstat = 5;}
    if(pinit == 11 && py < -30)
    {pinit = 0;}
    random(-1,random(900,0));
    if(pinit < 10)
    {
      px = random(Width,0) - 19;
      if(px < 0)
      {px = px + 20 + random(50,0);}
      py = -30 - random(9,0);
      pinit = 10;
    }
    }

   if(ImageCount > numframes[Background]-1 && (Background == 6 || Background == 7))
   {
      increment = -1;
      ImageCount = numframes[Background]-1;
   }

   if(ImageCount == 0 && increment == -1)
   {
      increment = 1;
   }

   if(ImageCount > numframes[Background]-1 && (Background != 6 || Background != 7))
    {ImageCount = 0;}
    BackgroundChangeCount++;

  if( (config1&64) == 64)
  {hdat = 1; goto stopload2;}

    if(BackgroundChangeCount == 205)
    {BackgroundChangeCount == 1; increment = 1;
    Background = (Background+1)&7;
    DeleteObject(BattleBkg);
    BattleBkg = LoadBitmap(hMainInstance, MAKEINTRESOURCE(900+Background));
    }
  stopload2: ;
    for(rect.right = 0; rect.right < 5; rect.right++)
    {
       DeletePoint[rect.right].x = DrawPoint[rect.right].x;
       DeletePoint[rect.right].y = DrawPoint[rect.right].y;

       directiondata[rect.right].lastdir = direction[rect.right];
       directiondata[rect.right].changedir = 0;
       
       /*status = (status + 1)&1;*/ /*Makes a nifty binary switch, with no comparison to slow it down!*/
       /*Moved to increase effiency*/
       if(direction[rect.right] == DOWN)
       {DrawPoint[rect.right].y = DrawPoint[rect.right].y + directiondata[rect.right].speed;}
       if(direction[rect.right] == LEFT)
       {DrawPoint[rect.right].x = DrawPoint[rect.right].x - directiondata[rect.right].speed;}
       if(direction[rect.right] == RIGHT)
       {DrawPoint[rect.right].x = DrawPoint[rect.right].x + directiondata[rect.right].speed;}
       if(direction[rect.right] == UP)
       {DrawPoint[rect.right].y = DrawPoint[rect.right].y - directiondata[rect.right].speed;}

       if(direction[rect.right] == UPLEFT)
       {DrawPoint[rect.right].y -= directiondata[rect.right].speed; DrawPoint[rect.right].x -= directiondata[rect.right].speed;}
       if(direction[rect.right] == DOWNLEFT)
       {DrawPoint[rect.right].x -= directiondata[rect.right].speed; DrawPoint[rect.right].y += directiondata[rect.right].speed;}
       if(direction[rect.right] == UPRIGHT)
       {DrawPoint[rect.right].x += directiondata[rect.right].speed; DrawPoint[rect.right].y -= directiondata[rect.right].speed;}
       if(direction[rect.right] == DOWNRIGHT)
       {DrawPoint[rect.right].y += directiondata[rect.right].speed; DrawPoint[rect.right].x += directiondata[rect.right].speed;}


       if(DrawPoint[rect.right].x < 0 && (DrawPoint[rect.right].y+48) > Height) /*If you hit part of the edge of the screen, you turn around.*/
       {direction[rect.right] = UPRIGHT; directiondata[rect.right].changedir = 1;goto nextcharacter;}
       if(DrawPoint[rect.right].x < 0 && DrawPoint[rect.right].y < 0)
       {direction[rect.right] = DOWNRIGHT; directiondata[rect.right].changedir = 1;goto nextcharacter;}
       if(DrawPoint[rect.right].x < 0)
       {direction[rect.right] = RIGHT;directiondata[rect.right].changedir = 1;goto nextcharacter;}
       if(DrawPoint[rect.right].y < 0 && (DrawPoint[rect.right].x+32) > Width)
       {direction[rect.right] = DOWNLEFT;directiondata[rect.right].changedir = 1; goto nextcharacter;}
       if((DrawPoint[rect.right].y+48) > Height && (DrawPoint[rect.right].x+32) > Width)
       {direction[rect.right] = UPLEFT;directiondata[rect.right].changedir = 1; goto nextcharacter;}
       if(DrawPoint[rect.right].y < 0)
       {direction[rect.right] = DOWN;goto nextcharacter;}
       if((DrawPoint[rect.right].y+46) > Height)
       {direction[rect.right] = UP;directiondata[rect.right].changedir = 1;}
       if((DrawPoint[rect.right].x+32) > Width)
       {direction[rect.right] = LEFT;directiondata[rect.right].changedir = 1;}
       nextcharacter: ; /*Changes Direction if an edge of the screen is hit, I put it here so that they will not leave a trail when they change direction.*/


    }
    count++;

    if( count == 21)
    {
      for(rect.right = 0; rect.right < 5; rect.right++)
      {
      direction[rect.right] = random(12,0);
      directiondata[rect.right].speed = 2 + random(6,0);
      }
      count = 0;
    }

    InvalidateRect(hWnd, NULL, FALSE);
    /*directional changes will be applied here later.*/

    
    return 0;				
  case WM_PAINT:
    {hDC = BeginPaint(hWnd, &ps);
    Mem = CreateCompatibleDC(hDC);
    BkgMem = CreateCompatibleDC(hDC);
    tempfield = CreateCompatibleDC(hDC);
    Batt = CreateCompatibleDC(hDC);

  SelectObject(tempfield, FX);
  SelectObject(Mem, Blue);
  SelectObject(BkgMem, BattleBkg);
    /*Obsolete code removed*/


    status = (status+1)&1;              
    xstart = ImageCount & 1;
    ystart = (ImageCount - xstart)/2;

    if( hdat == 1)
    {StretchBlt(tempfield, 0, 0,Width,Height,BkgMem,0,0,256,223,SRCCOPY); goto area2;}

    if(Background < 3)
    {

    if( (config1&2) == 0)
    {StretchBlt(tempfield, 0,0,Width,Height,BkgMem,xstart*256,ystart*224,256,223,SRCCOPY); goto nextstep;}
    else
    {StretchBlt(tempfield, 0,0,Width,Height,BkgMem,xstart*256,(ystart*224)+ys[Background],ws[Background]+1,high[Background],SRCCOPY);}

    }

    if(Background >= 3)
    {
    if( (config1&2) == 0)
    {StretchBlt(tempfield, 0,0,Width,Height,BkgMem,xstart*256,ystart*223,256,223,SRCCOPY); goto nextstep;}
    else
    {StretchBlt(tempfield, 0,0,Width,Height,BkgMem,xstart*256,(ystart*223)+ys[Background],ws[Background]+1,high[Background],SRCCOPY);}
    
    }
    area2: ;
    nextstep: ;
    nextareabkg: ;
    if( (config1&1) == 0)
    {
    for(rect.right = 0; rect.right < 5; rect.right++)
    {  /*Erase Mask Area*/
       BitBlt(tempfield, DrawPoint[rect.right].x, DrawPoint[rect.right].y, 32, 48, Mem, (((status * 4) + direction[rect.right]) * 33) + 1, (rect.right * 100)+50, SRCAND);
       BitBlt(tempfield, DrawPoint[rect.right].x, DrawPoint[rect.right].y, 32, 48, Mem, (((status * 4) + direction[rect.right]) * 33) + 1, (rect.right * 100)+1, SRCPAINT);
       /*Draw New Character*/
    }
    }


    SelectObject(Mem, Photo);

    if( (config1 & 16) != 0)
    {
    if(pinit >= 10)
    {
    if(pstat == 5)
    {
      pguy = (pguy+1)&3;
      pinit = 11;
      BitBlt(tempfield, px, py, 36, 56, Mem, 72, 56, SRCAND);
      BitBlt(tempfield, px, py, 36, 56, Mem, 72, 0, SRCPAINT);
      SelectObject(Mem, Flash);

      if(pguy > 0)
      {py -= 15; pinit = 10;}
      if(pguy == 3)
      {StretchBlt(tempfield, 0,0,Width,Height,Mem,0,0,420,268,SRCPAINT);}
      goto endpaint;
      
    }
    BitBlt(tempfield, px, py, 36, 56, Mem, 36*pstat, 56, SRCAND);
    BitBlt(tempfield, px, py, 36, 56, Mem, 36*pstat, 0, SRCPAINT);
    }
    }

    endpaint: ;

    if(toobig == 0)
    {BitBlt(hDC,0,0,Width,Height, tempfield,0,0,SRCCOPY);}
    else
    {StretchBlt(hDC,0,0,tWidth,tHeight, tempfield,0,0,Width,Height,SRCCOPY);}

    DeleteDC(tempfield);
    DeleteDC(Batt);
    EndPaint(hWnd, &ps);
    DeleteDC(BkgMem);
    DeleteDC(Mem);
    return 0;}
  }
     
  /*Any other messages...It's the screensaver-library's problem...*/

  return DefScreenSaverProc(hWnd, message, wParam, lParam );
}

/*BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, 
                           WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}*/

BOOL WINAPI RegisterDialogClasses(HANDLE dlg)
{
/*LPTSTR *cmdline;
char cmdlinex[1024];
int length;
int loop;
char test[50];
cmdline = GetCommandLine();
strcpy(cmdlinex, cmdline);
length = strlen(cmdlinex);
if(cmdlinex[length-1] != '/' && cmdlinex[length] != 'S' || cmdlinex[length] != 's')
{MessageBox(hMainWindow,cmdlinex,"cmdline",MB_OK);}*/
return TRUE;
}


BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, 
                           WPARAM wParam, LPARAM lParam)
{

  switch ( message ) 
  {

	case WM_INITDIALOG:
    {
    GetProfileString("EvilPuffBall - EBScreenSaver", "config1", "40", Regbinarydata, 4);   
    GetProfileString("EvilPuffBall - EBScreenSaver", "delay", "200", Delaystring, 7);   
    config1 = (Regbinarydata[0]-48)+((Regbinarydata[1]-48)*16);
       if( (config1&1) == 0)
       {SendDlgItemMessage(hDlg, 501, BM_SETCHECK, 1, 0);}
       if( (config1&2) != 0)
       {SendDlgItemMessage(hDlg, 502, BM_SETCHECK, 1, 0);}
       if( (config1&4) == 0)
       {SendDlgItemMessage(hDlg, 503, BM_SETCHECK, 1, 0);}
       if( (config1&8) == 0)
       {SendDlgItemMessage(hDlg, 504, BM_SETCHECK, 1, 0);}
       if( (config1&16) != 0)
       {SendDlgItemMessage(hDlg, 505, BM_SETCHECK, 1, 0);}
       if( (config1&128) == 128)
       {SendDlgItemMessage(hDlg, 506, BM_SETCHECK, 1, 0);}
       if( (config1&64) == 64)
       {SendDlgItemMessage(hDlg, 507, BM_SETCHECK, 1, 0);}

       SetDlgItemText(hDlg,600,Delaystring);

       return TRUE;}

       case WM_COMMAND:
           switch( LOWORD( wParam ) ) 
		{ 

                //handle various cases sent when controls
                //are checked, unchecked, etc

                //when OK is pressed, write the configuration
                //to the registry and end the dialog box

            case IDOK:
            config1 = 1;
            if( SendDlgItemMessage(hDlg, 501, BM_GETCHECK, 0, 0) == 1)
            {config1 = 0;}
            if( SendDlgItemMessage(hDlg, 502, BM_GETCHECK, 0, 0) == 1)
            {config1 += 2;}
            if( SendDlgItemMessage(hDlg, 503, BM_GETCHECK, 0, 0) == 0)
            {config1 += 4;}
            if( SendDlgItemMessage(hDlg, 504, BM_GETCHECK, 0, 0) == 0)
            {config1 += 8;}
            if( SendDlgItemMessage(hDlg, 505, BM_GETCHECK, 0, 0) == 1)
            {config1 += 16;}
            if( SendDlgItemMessage(hDlg, 506, BM_GETCHECK, 0, 0) == 1)
            {config1 += 128;}  
            if( SendDlgItemMessage(hDlg, 507, BM_GETCHECK, 0, 0) == 1)
            {config1 += 64;}  

                               /*Mem (In low-op): 5115900 Bytes*/       /*5 -> 7MB*/

       Regbinarydata[0] = (config1&15)+48; Regbinarydata[1] = ((config1&240)/16)+48; Regbinarydata[2] = 0; Regbinarydata[3] = 0;

       WriteProfileString("EvilPuffBall - EBScreenSaver", "config1",Regbinarydata);
       GetDlgItemText(hDlg, 600, Delaystring, 6);
       if(strlen(Delaystring) != 0)
       {WriteProfileString("EvilPuffBall - EBScreenSaver", "delay", Delaystring);}
       else
       {MessageBox(hDlg, "Invalid Entry for delay (blank), no change made to delay", "Error", MB_OK);}
			return TRUE; 
        case 900:
            MessageBox(hDlg, "Additional Credits:\nPotato\t\tChosen Four Sprites\nShadowX\t\tMaking me get the backgrounds right\nKitties\t\tThey're so cute!\nBen S.\t\tTesting\nWhat evar\tTesting\n\t\tBackground Number 2\nB. G.-Edmond\tPorted XMatrix Screensaver (I learned from it)\nYou\t\tDownloaded it\n\
Version: 1.44\nEBScreenSaver          Memory Required: 7.5 to 12.5MB\nExecutable compressed by UPX\nInternal Compression: zlib\nMemory Checker: MemCheck (2.69)\n\t --I Hope you enjoy the screensaver!\n\nEvilPuffBall\tpokekirby@mail.com", "About/Credits", MB_OK);
            return TRUE;
		case IDCANCEL: 
			EndDialog( hDlg, LOWORD( wParam ) == IDOK ); 
			return TRUE;   
		}

  }	//end command switch

  return FALSE; 
} 

BOOL VerifyPassword(HWND hwnd)
{   
  OSVERSIONINFO osv;
  osv.dwOSVersionInfoSize=sizeof(osv);
  GetVersionEx(&osv);
  if (osv.dwPlatformId==VER_PLATFORM_WIN32_NT)
  {return TRUE;}
  
  HINSTANCE hpwdcpl=LoadLibrary("PASSWORD.CPL");
  if (hpwdcpl==NULL) {return TRUE;}
  typedef BOOL (WINAPI *VERIFYSCREENSAVEPWD)(HWND hwnd);
  VERIFYSCREENSAVEPWD VerifyScreenSavePwd;
  VerifyScreenSavePwd=(VERIFYSCREENSAVEPWD)GetProcAddress(hpwdcpl,"VerifyScreenSavePwd");
  if (VerifyScreenSavePwd==NULL) {FreeLibrary(hpwdcpl);return TRUE;}
  BOOL bres=VerifyScreenSavePwd(hwnd); FreeLibrary(hpwdcpl);
  return bres;
}

 void ChangePassword(HWND hwnd)
 { // This only ever gets called under '95, when started with the /a option.
     HINSTANCE hmpr=LoadLibrary("MPR.DLL");
     if (hmpr==NULL) {return;}
     typedef VOID (WINAPI *PWDCHANGEPASSWORD) (LPCSTR lpcRegkeyname,HWND hwnd,UINT uiReserved1,UINT uiReserved2);
     PWDCHANGEPASSWORD PwdChangePassword=(PWDCHANGEPASSWORD)GetProcAddress(hmpr,"PwdChangePasswordA");
     if (PwdChangePassword==NULL) {FreeLibrary(hmpr); return;}
     PwdChangePassword("SCRSAVE",hwnd,0,0); FreeLibrary(hmpr);
 }

HBITMAP ExtractBkgBitmapResource(int val)
{
   HRSRC d1;
   HGLOBAL d2;
   long Ressize;
   int stat;
   unsigned char *compdata;
   unsigned char *black;
   unsigned char *ldat;
   unsigned long destlength;
   unsigned char dat[80000];
   Ressize = SizeOfBkgResource[val];

   d1 = FindResourceEx(hMainInstance, "WAVE", MAKEINTRESOURCE(900+val), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
   d2 = LoadResource(hMainInstance, d1);
   black=LockResource(d2);
   memcpy(&dat[0],&black[44],Ressize);
   destlength = (dat[0] * 0x1000000) + (dat[1] * 0x10000) + (dat[2] * 0x100) + dat[3];
   printf("%ld", destlength);
   if( (buffer = realloc(buffer, sizeof(char) * (destlength + 40))) == NULL)
   {return NULL;}
   compdata = &dat[4];
   stat = uncompress(buffer, &destlength,compdata, Ressize-4);
   if(stat == Z_BUF_ERROR)
   {return NULL;}
   if(stat == Z_MEM_ERROR)
   {return NULL;}
   if(stat == Z_DATA_ERROR)
   {return NULL;}
      BITMAPFILEHEADER *bfh=(BITMAPFILEHEADER*)buffer;
      BITMAPINFOHEADER *bih=(BITMAPINFOHEADER*)(buffer+sizeof(BITMAPFILEHEADER));
/*    int ncols=bih->biClrUsed; if (ncols==0) ncols = 1<<bih->biBitCount;
      unsigned char *srcbits = (char*)(buffer+bfh->bfOffBits);
      unsigned char *dstbits;
      BattleBkg=CreateDIBSection(NULL,(BITMAPINFO*)bih,DIB_RGB_COLORS,(void**)&dstbits,NULL,0);
      if(NULL == BattleBkg)
      {
      printf("%d", GetLastError());
      }
      unsigned int numbytes = bih->biSizeImage;
      if (numbytes==0) numbytes = ((bih->biWidth*bih->biBitCount/8+3)&0xFFFFFFFC)*bih->biHeight;*/
      BITMAP lpbd;
      BITMAP FAR *lpccc;
      lpbd.bmType = bfh->bfType;
      lpbd.bmWidth = bih->biWidth;
      lpbd.bmHeight = bih->biHeight;
      lpbd.bmPlanes = 1;
      lpbd.bmWidthBytes = ((bih->biWidth * bih->biBitCount/8)+3)&0xFFFFFFFC;
      lpbd.bmBitsPixel = bih->biBitCount;
      lpbd.bmBits = &buffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
      lpccc = &lpbd;
      BattleBkg = CreateBitmapIndirect( lpccc);
      if(NULL == BattleBkg)
      {
      printf("%d", GetLastError());
      }
      /*CopyMemory(dstbits,srcbits,numbytes);*/
}


unsigned char *create_wav_header(unsigned long datasize, unsigned long frequency, int channels, int bitspersample, unsigned char *dt)
{
   int num;
   dt[0] = 'R'; dt[1] = 'I'; dt[2] = 'F'; dt[3] = 'F';
   dt[4] = (datasize+36)&0xFF;
   dt[5] = ((datasize+36)&0xFF00)/0x100;
   dt[6] = ((datasize+36)&0xFF0000)/0x10000;
   dt[7] = ((datasize+36)&0xFF000000)/0x1000000;
   dt[8] = 'W'; dt[9] = 'A'; dt[10] = 'V'; dt[11] = 'E';
   dt[12] = 'f'; dt[13] = 'm'; dt[14] = 't'; dt[15] = ' ';
   dt[16] = 16; dt[17] = 0; dt[18] = 0; dt[19] = 0;
   /*16,17,18,19*/
   dt[20] = 1; dt[21] = 0;
   dt[22] = 1; dt[23] = 0;num = 1;
   if(channels == 1)
   {dt[22] = 1; dt[23] = 0; num=1;}
   if(channels == 2)
   {dt[22] = 2; dt[23] = 0; num = 2;}
   dt[24] = (frequency)&0xFF;
   dt[25] = ((frequency)&0xFF00)/0x100;
   dt[26] = ((frequency)&0xFF0000)/0x10000;
   dt[27] = ((frequency)&0xFF000000)/0x1000000;
   dt[28] = (num*frequency*(bitspersample/8))&0xFF;
   dt[29] = ((num*frequency*(bitspersample/8))&0xFF00)/0x100;
   dt[30] = ((num*frequency*(bitspersample/8))&0xFF0000)/0x10000;
   dt[31] = ((num*frequency*(bitspersample/8))&0xFF000000)/0x1000000;
   dt[32] = (num*(bitspersample/8))&0xFF;
   dt[33] = ((num*(bitspersample/8))&0xFF00)/0x100;
   dt[34] = bitspersample&0xFF;
   dt[35] = (bitspersample&0xFF00)/0x100;
   dt[36] = 'd'; dt[37] = 'a'; dt[38] = 't'; dt[39] = 'a';
   dt[40] = (datasize)&0xFF;
   dt[41] = ((datasize)&0xFF00)/0x100;
   dt[42] = ((datasize)&0xFF0000)/0x10000;
   dt[43] = ((datasize)&0xFF000000)/0x1000000;
   return dt;

}
