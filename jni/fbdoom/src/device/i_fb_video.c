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
static IDirectFBEventBuffer *events;

static int screen_width, screen_height;

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
     DFBCHECK(dfb->CreateInputEventBuffer( dfb, DICAPS_ALL,
                                           DFB_FALSE, &events ));

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
            
}


void I_ShutdownGraphics(void)
{
     events->Release( events );
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