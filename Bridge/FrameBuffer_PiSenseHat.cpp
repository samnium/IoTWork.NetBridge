


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <poll.h>
#include <dirent.h>
#include <string.h>

#include <linux/input.h>
#include <linux/fb.h>
#include <unistd.h>

#include "netbridge.h"
#include "FrameBuffer_PiSenseHat.h"
#include "FrameBuffer_CharectersMaps.h"

#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

static int is_framebuffer_device(const struct dirent *dir)
{
        return strncmp(FB_DEV_NAME, dir->d_name,
                       strlen(FB_DEV_NAME)-1) == 0;
}

FrameBuffer_PiSenseHat::FrameBuffer_PiSenseHat()
{
	fbfd = -1;
	fb = NULL;
}

FrameBuffer_PiSenseHat::~FrameBuffer_PiSenseHat ()
{
	if (fbfd > 0 || fb)
	   Close ();
}

bool FrameBuffer_PiSenseHat::Open ()
{
	fbfd = open_fbdev();
    DEBUG_PRINT("fbfd %d\n", fbfd);
	if (fbfd > 0)
	{
		fb = (struct fb_t *)mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
		DEBUG_PRINT("fbfd %p\n", fb);
		if (!fb) {
			printf("Failed to mmap.\n");
			exit(1);
		}
		memset(fb, 0, 128);
		return true;
	}
	else
		return false;
	return false;
}

void FrameBuffer_PiSenseHat::Close ()
{
	if (fb != NULL)
	{
		memset(fb, 0, 128);
		munmap(fb, 128);
		fb = NULL;
	}

	if (fbfd > 0)
	{
		close(fbfd);
		fbfd = -1;
	}
}

void FrameBuffer_PiSenseHat::Draw (char c)
{
	DEBUG_PRINT("INPUT c %c\n", c);

	uint16_t (*_charmap)[8][8] = NULL;

	DEBUG_PRINT("_charmap %p\n", _charmap);

	if (c == 'H')
		_charmap = &_fb_char_map_8x8_H;
	else
	if (c == 'P')
		_charmap = &_fb_char_map_8x8_P;
	else
	if (c == 'T')
		_charmap = &_fb_char_map_8x8_T;
		
	DEBUG_PRINT("_charmap %p\n", _charmap);
	
	memset(fb, 0, 128);

	if (_charmap != NULL && fb != NULL)
	{
		int x = 0;
		int y = 0;

		for(x = 0; x < 8; x++)
		{
			for(y = 0; y < 8; y++)
			{
				fb->pixel[x][y] = (*_charmap)[x][y];
			}
		}
	}

}


int FrameBuffer_PiSenseHat::open_fbdev()
{
	const char *dev_name = "RPi-Sense FB";
	struct dirent **namelist;
	int i, ndev;
	int fd = -1;
	struct fb_fix_screeninfo fix_info;

	ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
	if (ndev <= 0)
		return ndev;

	for (i = 0; i < ndev; i++)
	{
		char fname[64];

		snprintf(fname, sizeof(fname),
			 "%s/%s", DEV_FB, namelist[i]->d_name);
		fd = open(fname, O_RDWR);
		if (fd < 0)
			continue;
		ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
		if (strcmp(dev_name, fix_info.id) == 0)
			break;
		close(fd);
		fd = -1;
	}
	for (i = 0; i < ndev; i++)
		free(namelist[i]);

	return fd;
}

void FrameBuffer_PiSenseHat::render()
{
	/*
	struct segment_t *seg_i;
	memset(fb, 0, 128);
	fb->pixel[apple.x][apple.y]=0xF800;
	for(seg_i = snake.tail; seg_i->next; seg_i=seg_i->next) {
		fb->pixel[seg_i->x][seg_i->y] = 0x7E0;
	}
	fb->pixel[seg_i->x][seg_i->y]=0xFFFF;
	*/
}


// ---------------------------------------------------------------
//
// C for marshalling
//
// ---------------------------------------------------------------


extern "C"  FrameBuffer_PiSenseHat * __FrameBuffer_PiSenseHat_Create()
{
   return new FrameBuffer_PiSenseHat();
}


extern "C"  void __FrameBuffer_PiSenseHat_Dispose(FrameBuffer_PiSenseHat * FrameBuffer)
{
   if (FrameBuffer != NULL)
   {
      delete FrameBuffer;
      FrameBuffer = NULL;
   }
}


extern "C" bool __FrameBuffer_PiSenseHat_Open (FrameBuffer_PiSenseHat * FrameBuffer)
{
   if (FrameBuffer != NULL)
   {
      return FrameBuffer->Open();
   }
   else
   	return false;	
}

extern "C" void __FrameBuffer_PiSenseHat_Close (FrameBuffer_PiSenseHat * FrameBuffer)
{
   if (FrameBuffer != NULL)
   {
      FrameBuffer->Close();
   }
}

extern "C" void __FrameBuffer_PiSenseHat_Draw (FrameBuffer_PiSenseHat * FrameBuffer, char c)
{
   if (FrameBuffer != NULL)
   {
      return FrameBuffer->Draw(c);
   }
}