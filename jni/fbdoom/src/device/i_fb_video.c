#include "i_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
//#include <linux/fb.h>
#include <directfb.h>

#include <sys/mman.h>
#include <stdint.h>
#include "v_video.h"
#include "m_argv.h"
#include "d_event.h"


//static FILE* fbfd = 0;
//static struct fb_var_screeninfo vinfo;
//static struct fb_fix_screeninfo finfo;
static long int screensize = 0;
//static char *fbp = 0;
static uint8_t* gameScreen;


/* the super interface */
static IDirectFB            *dfb;

/* the primary surface (surface of primary layer) */
static IDirectFBSurface     *screen;
static IDirectFBSurface     *primary;
static IDirectFBPalette     *palette;

/* Input interfaces: device and its buffer */
static IDirectFBEventBuffer *eventsbuffer;

IDirectFBInputDevice    *keyboard;
IDirectFBEventBuffer    *keybuffer;
DFBInputEvent            devt;


static int screen_width, screen_height;
/* new defines for using goggles joystick and keys */
#define SYMPOL_KEY_CHANNEL_UP 62724
#define SYMPOL_KEY_LEFT_JOY 61521
#define SYMPOL_KEY_RIGHT_JOY 61530
#define SYMPOL_KEY_BACK 61450

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x...) \
        {                                                                      \
           err = x;                                                            \
           if (err != DFB_OK) {                                                \
              fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );           \
              DirectFBErrorFatal( #x, err );                                   \
           }                                                                   \
        }
        

void I_InitGraphics (void)
{
      DFBResult              err;
     DFBSurfaceDescription  sdsc;
     DFBSurfaceDescription  pdsc;
    


     DFBCHECK(DirectFBInit( &myargc, &myargv ));

     /* create the super interface */
     DFBCHECK(DirectFBCreate( &dfb ));

     /* create an event buffer for all devices */
     /*DFBCHECK(dfb->CreateInputEventBuffer( dfb, DICAPS_ALL,
                                           DFB_FALSE, &eventsbuffer ));*/

     /* set our cooperative level to DFSCL_FULLSCREEN
        for exclusive access to the primary layer */
     dfb->SetCooperativeLevel( dfb, DFSCL_FULLSCREEN );

    pdsc.caps =        DSCAPS_SYSTEMONLY;
    pdsc.width       = SCREENWIDTH;
    pdsc.height       = SCREENHEIGHT;
    pdsc.flags =  DSDESC_HEIGHT | DSDESC_WIDTH | DSDESC_PIXELFORMAT;
    pdsc.pixelformat = DSPF_ARGB;

     pdsc.pixelformat = DSPF_ARGB;

     DFBCHECK(dfb->CreateSurface( dfb, &pdsc, &primary ));

     primary->Clear( primary, 0, 0, 0, 0 );
     sdsc.flags       = DSDESC_CAPS;// | DSDESC_PIXELFORMAT;
     sdsc.caps        = DSCAPS_PRIMARY;
     sdsc.pixelformat = DSPF_ARGB;

     DFBCHECK(dfb->CreateSurface( dfb, &sdsc, &screen ));


     screen->GetSize( primary, &screen_width, &screen_height );
    /* Figure out the size of the screen in bytes */
    screensize = screen_width * screen_height * 32 / 8;
    printf("Screen width is %d, height is %d size is %d\n",screen_width, screen_height,screensize);

        
    DFBCHECK(dfb->GetInputDevice( dfb, DIDID_ANY, &keyboard ));
    DFBCHECK(keyboard->CreateEventBuffer( keyboard, &keybuffer ));

            
}


//
//  Translates the key currently in dfbinputevent
//

int dlatekey(void)
{

    int rc;

    switch(rc = devt.key_symbol)
    {
      case SYMPOL_KEY_LEFT_JOY:
        rc = KEY_LEFTARROW;
        break;
    case SYMPOL_KEY_RIGHT_JOY:
        rc = KEY_RIGHTARROW;
        break;
    case DIKS_CURSOR_DOWN:
        rc = KEY_DOWNARROW;
        break;
    case SYMPOL_KEY_CHANNEL_UP:
        rc = KEY_UPARROW;
        break;
    case DIKS_ESCAPE:
        rc = KEY_ESCAPE;
        break;
    case DIKS_RETURN:
        rc = KEY_ENTER;
        break;
    case DIKS_TAB:
        rc = KEY_TAB;
        break;
    case DIKS_F1:
        rc = KEY_F1;
        break;
    case DIKS_F2:
        rc = KEY_F2;
        break;
    case DIKS_F3:
        rc = KEY_F3;
        break;
    case DIKS_F4:
        rc = KEY_F4;
        break;
    case DIKS_F5:
        rc = KEY_F5;
        break;
    case DIKS_F6:
        rc = KEY_F6;
        break;
    case DIKS_F7:
        rc = KEY_F7;
        break;
    case DIKS_F8:
        rc = KEY_F8;
        break;
    case DIKS_F9:
        rc = KEY_F9;
        break;
    case DIKS_F10:
        rc = KEY_F10;
        break;
    case DIKS_F11:
        rc = KEY_F11;
        break;
    case DIKS_F12:
        rc = KEY_F12;
        break;

    case DIKS_BACKSPACE:
    case DIKS_DELETE:
        rc = KEY_BACKSPACE;
        break;

        //   case DIKS_PAUSE:	rc = KEY_PAUSE;		break;

    case DIKS_EQUALS_SIGN:
        rc = KEY_EQUALS;
        break;

    case DIKS_MINUS_SIGN:
        rc = KEY_MINUS;
        break;

    case DIKS_SHIFT:
        rc = KEY_RSHIFT;
        break;

    case SYMPOL_KEY_BACK:
        rc = KEY_RCTRL;
        // rc = KEY_SPACEBAR;

        break;

    case DIKS_ALT:
    case DIKS_ALTGR:
        rc = KEY_RALT;
        break;

    default:
        if (rc >= DIKS_SPACE && rc <= DIKS_TILDE)
            rc = rc - DIKS_SPACE + ' ';
        if (rc >= 'A' && rc <= 'Z')
            rc = rc - 'A' + 'a';
        break;
    }

    return rc;
}


struct timeval lastReleaseTime;
int lastKey = 0;


void I_StartTic(void)
{

    event_t event;
    while (keybuffer->GetEvent(keybuffer, DFB_EVENT(&devt)) == DFB_OK)
    {
        printf("Key event type %d code %d id %d symbol %d\n", devt.type, devt.key_code, devt.key_id, devt.device_id);

        switch (devt.type)
        {
        case DIET_KEYPRESS:

            event.type = ev_keydown;
            event.data1 = dlatekey();
            D_PostEvent(&event);

            fprintf(stderr, "k");


            break;

        case DIET_KEYRELEASE:
            printf("Key up %d\n", devt.key_symbol);

            struct timeval currentTime = devt.timestamp;

            int timeuse = (1000000 * (currentTime.tv_sec) + currentTime.tv_usec) - (1000000 * (lastReleaseTime.tv_sec) + lastReleaseTime.tv_usec);

            // double tap
            if ((timeuse) <= 300000)
            {

                // remapping goggle's joystick left to Enter key
                if (dlatekey() == lastKey && lastKey == KEY_LEFTARROW)
                {

                    printf("trigge enter ");
                    event.type = ev_keyup;
                    event.data1 = KEY_LEFTARROW;
                    D_PostEvent(&event);
                    event.type = ev_keydown;
                    event.data1 = KEY_ENTER;
                    D_PostEvent(&event);
                    event.type = ev_keyup;
                    D_PostEvent(&event);

                    lastReleaseTime = currentTime;
                    lastKey = KEY_LEFTARROW;
                    break;
                }
                // remapping goggle's "channel up" btn to Open/Use 
                else if (dlatekey() == lastKey && lastKey == KEY_UPARROW)
                {

                    printf("trigge space ");

                    event.type = ev_keyup;
                    event.data1 = KEY_UPARROW;
                    D_PostEvent(&event);

                    event.type = ev_keydown;
                    event.data1 = KEY_SPACEBAR;
                    D_PostEvent(&event);

                    lastReleaseTime = currentTime;
                    lastKey = KEY_UPARROW;

                    break;
                }
                // remapping goggle's "back" btn  to enable/disable god mode 
                else if (dlatekey() == lastKey && lastKey == KEY_RCTRL)
                {

                
                    printf("toggle god mode ");

                    event.type = ev_keyup;
                    event.data1 = KEY_RCTRL;
                    D_PostEvent(&event);

                    event.type = ev_keydown;

                    event.data1 = KEY_GOD_MODE; // iddqd
                    D_PostEvent(&event);

                    lastReleaseTime = currentTime;
                    lastKey = KEY_RCTRL;

                    break;
                }
                 // remapping goggle's joystick right to switch between weapons
                else if (dlatekey() == lastKey && lastKey == KEY_RIGHTARROW)
                {
                    printf("trigge next weapons ");

                    event.type = ev_keyup;
                    event.data1 = KEY_RIGHTARROW;
                    D_PostEvent(&event);

                    event.type = ev_keydown;
                    event.data1 = KEY_SWITCH_WEAPONS;
                    D_PostEvent(&event);

                    lastReleaseTime = currentTime;
                    lastKey = KEY_RIGHTARROW;

                    break;
                }
            }

            lastReleaseTime = currentTime;
            event.type = ev_keyup;
            event.data1 = dlatekey();
            D_PostEvent(&event);
            lastKey = dlatekey();

            fprintf(stderr, "ku");
            break;
        }
    }
}


void I_ShutdownGraphics(void)
{
    eventsbuffer->Release( eventsbuffer );
    keyboard->Release( keyboard );
    dfb->Release( dfb );

}

void I_StartFrame (void)
{

}
__attribute__((packed))
struct Color
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

union ColorInt
{
    struct Color col;
    uint32_t raw;
};

static union ColorInt colors[256];

// Takes full 8 bit values.
void I_SetPalette (byte* palette)
{
    byte c;
    // set the X colormap entries
    for (int i=0 ; i<256 ; i++)
    {
        c = gammatable[usegamma][*palette++];
        colors[i].col.r = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.g = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.b = (c<<8) + c;
        colors[i].col.a = 255;
    }
}

void I_UpdateNoBlit (void)
{

}

/*uint16_t colorTo16bit(struct Color col)
{
    return  (col.r >> 3) << 11 | (col.g >> 2) << 5 | (col.b >> 3);
    //return (col.b & 0x1F) << 10 | (col.g & 0x1F) << 5 | (col.r & 0x1F);
}*/


void I_FinishUpdate (void)
{
    DFBResult  err;
     int        x;
     int        y;
     void      *ptr;
     int        pitch;
     uint32_t        *dst;
     DFBCHECK(primary->Lock( primary, DSLF_WRITE, &ptr, &pitch ));

    for (y=0; y<SCREENHEIGHT; y++) {
        dst = ptr + y * pitch;
        for (x=0; x<SCREENWIDTH; x++) {
            dst[x] = colors[*(screens[0]+(y*SCREENWIDTH+x))].raw;
            
       }
    }
          
     
     DFBCHECK(primary->Unlock( primary ));
     int scale = 4;
     DFBRectangle rect;
     rect.x = 80;
     rect.y = 5;
     rect.w=SCREENWIDTH*scale;
     rect.h=SCREENHEIGHT*scale;
     //DFBCHECK(screen->Blit( screen, primary, NULL, 0, 0));
     DFBCHECK(screen->StretchBlit( screen, primary, NULL, &rect));

    
} 

/*void I_FinishUpdate (void)
{
    for (int gy=0; gy<SCREENHEIGHT; gy++)
    {
        for (int gx=0; gx<SCREENWIDTH; gx++)
        {
            int fbPos = location(gx,gy);
            if (vinfo.bits_per_pixel == 32)
            {
                *((uint32_t*)(fbp+fbPos + 0)) = colors[*(screens[0]+gy*SCREENWIDTH+gx)].raw;
            }
            else if (vinfo.bits_per_pixel == 16)
            {
                *((uint16_t*)(fbp+fbPos)) = colorTo16bit(colors[*(screens[0]+gy*SCREENWIDTH+gx)].col);
            }
        }
    }
}*/



void I_ReadScreen (byte* scr)
{
    memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}