// pop2beeb.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "CImg.h"

using namespace cimg_library;

static unsigned char imagetab[48 * 1024];
static int image_addrs[256];
static int image_size[256][2];
static unsigned char image_data[256][16*1024];

static int pixel_size[256][2];
static unsigned char pixels[256][10000];

static unsigned char colours[256][10000];
static int colour_width[256];
static bool half[256];

#define BLACK0 0
#define PURPLE 1
#define GREEN 2
#define WHITE0 3
#define BLACK1 4
#define BLUE 5
#define ORANGE 6
#define WHITE1 7

#define GET_16BIT(ptr)	(*(ptr) + *(ptr+1)*256)
#define LO(val)			((int)(val) & 0xff)
#define HI(val)			(((int)(val) >> 8) & 0xff)

unsigned char palette[8][3] = 
{
	{ 0, 0, 0 },				// black
	{ 255, 68, 253 },			// purple
	{ 20, 245, 60 },			// green
	{ 255, 255, 255 },			// white
	{ 0, 0, 0 },				// black
	{ 20, 207, 253 },			// blue
	{ 255, 106, 60 },			// orange
	{ 255, 255, 255 },			// white
};

unsigned char dhr_palette[16][3] =
{
	{ 0, 0, 0 },			// black
	{ 96, 78, 189 },		// dk blue
	{ 0, 163, 96 },			// dk green
	{ 20, 207, 253 },		// med blue
	{ 96, 114, 3 },			// brown
	{ 156, 156, 156 },		// grey2
	{ 20, 245, 60 },		// green
	{ 114, 255, 208 },		// aqua
	{ 255, 68, 253 },		// magenta
	{ 255, 68, 253 },		// violet
	{ 156, 156, 156 },		// grey1
	{ 208, 195, 255 },		// lt blue
	{ 255, 106, 60 },		// orange
	{ 255, 160, 208 },		// pink
	{ 208, 221, 141 },		// yellow
	{ 255, 255, 255 },		// white
};

unsigned char odd_columns[8] =
{
	BLACK0,
	BLACK0,
	GREEN,						// orange
	WHITE0,
	BLACK0,
	PURPLE,						// blue
	WHITE0,
	WHITE0
};

unsigned char even_columns[8] =
{
	BLACK0,
	BLACK0,
	PURPLE,						// blue
	WHITE0,
	BLACK0,
	GREEN,						// orange
	WHITE0,
	WHITE0
};

unsigned char apple_colour_to_beeb_logical_colour[8] =			// maps apple colour to our MODE 5 colour indices
{
	0,							// black
	1,							// purple = magenta
	2,							// green
	3,							// white
	0,							// black
	1,							// blue = blue or cyan
	2,							// orange = red or yellow or stiple?
	3							// white
};

unsigned char beeb_mode5_colour_to_screen_pixel[4][4] =			// maps our MODE 5 colour indices to MODE 5 pixel bytes
{
	{ 0x00, 0x00, 0x00, 0x00 },	// 0 = black
	{ 0x08, 0x04, 0x02, 0x01 }, // 1 = blue
	{ 0x80, 0x40, 0x20, 0x10 }, // 2 = orange
	{ 0x88, 0x44, 0x22, 0x11 }, // 3 = white
};

unsigned char beeb_mode2_colour_to_screen_pixel[16][2] =		// maps our MODE 2 colour indices to MODE 2 left + right pixel bytes
{
	{ 0x00, 0x00 },		// 0=black
	{ 0x02, 0x01 },		// 1=red
	{ 0x08, 0x04 },		// 2=green
	{ 0x0A, 0x05 },		// 3=yellow
	{ 0x20, 0x10 },		// 4=blue
	{ 0x22, 0x11 },		// 5=magenta
	{ 0x28, 0x14 },		// 6=cyan
	{ 0x2A, 0x15 },		// 7=white

	{ 0x80, 0x40 },		// 8=black
	{ 0x82, 0x41 },		// 9=red
	{ 0x88, 0x44 },		// 10=green
	{ 0x8A, 0x45 },		// 11=yellow
	{ 0xA0, 0x50 },		// 12=blue
	{ 0xA2, 0x51 },		// 13=magenta
	{ 0xA8, 0x54 },		// 14=cyan
	{ 0xAA, 0x55 },		// 15=white
	
};

unsigned char nula_colours[16][3] =
{
	{ 0, 0, 0 },			// black (MUST BE BLACK)
	{ 255, 0, 0, },			// red
	{ 0, 255, 0, },			// green
	{ 255, 255, 0, },		// yellow
	{ 0, 0, 255, },			// blue
	{ 255, 0, 255, },		// magenta
	{ 0, 255, 255, },		// cyan
	{ 255, 255, 255, },		// white

	{ 64, 64, 64 },			// black
	{ 255, 0, 0, },			// red
	{ 0, 255, 0, },			// green
	{ 255, 255, 0, },		// yellow
	{ 0, 0, 255, },			// blue
	{ 255, 0, 255, },		// magenta
	{ 0, 255, 255, },		// cyan
	{ 255, 255, 255, },		// white
};

#define MAX_PALETTES 17

unsigned char palette_selection[MAX_PALETTES][3] =
{
	{ 4, 1, 7 },			// 0=blue, red, white = closest to Apple II default colours (blue, orange, white)
	{ 4, 6, 3 },			// 1=blue, cyan, yellow
	{ 1, 3, 7 },			// 2=red, yellow, white
	{ 4, 5, 3 },			// 3=blue, magenta, yellow

	{ 1, 5, 3 },			// 4=red, magenta, yellow
	{ 4, 1, 3 },			// 5=blue, red, yellow
	{ 6, 1, 3 },			// 6=cyan, red, yellow
	{ 4, 2, 3 },			// 7=blue, green, yellow

	{ 4, 1, 6 },			// 8=blue, red, cyan
	{ 4, 1, 2 },			// 9=blue, red, green
	{ 1, 6, 7 },			// 10=red, cyan, white
	{ 3, 5, 7 },			// 11=yellow, cyan, white

	{ 6, 5, 7 },			// 12=cyan, magenta, white (player)
	{ 4, 7, 6 },			// 13=blue, white, cyan (shadow)
	{ 3, 5, 7 },			// 14=yellow, magenta, white (font)

	{ 4, 5, 7 },			// 15=blue, magenta, white (guard)
	{ 2, 5, 7 },			// 16=green, magenta, white (guard)

//	{ 1, 5, 6 },			// 16=red, magenta, cyan (cutscene)
//	{ 1, 2, 3 },			// 17=red, green, yellow
};

int palette_used[MAX_PALETTES];

int convert_apple_to_pixels(unsigned char *apple_data, int apple_width, int apple_height, unsigned char *pixel_data)
{
	int pixel_width = apple_width * 7;

	for (int y = 0; y < apple_height; y++)
	{
		int x = 0;

		for (int a = 0; a < apple_width; a++)
		{
			unsigned char byte = apple_data[y * apple_width + a];
			
			int group = byte & 0x80;			// 0=purple+green, 1=blue+orange

			unsigned char bit = 1;

			for (int b = 0; b < 7; b++, bit <<= 1, x++)
			{
				pixel_data[y * pixel_width + x] = (group ? 4 : 0) + (byte & bit ? 1 : 0);
			}
		}
	}

#if 0
	for (int y = 0; y < apple_height; y++)
	{
		for (int x = 0; x < pixel_width; x++)
		{
			printf("%d ", pixel_data[y*pixel_width + x]);
		}
		printf("\n");
	}
#endif

	return pixel_width;
}

void flip_pixels_in_y(unsigned char *pixel_data, int pixel_width, int pixel_height)
{
	for (int y = 0; y < pixel_height / 2; y++)
	{
		for (int x = 0; x < pixel_width; x++)
		{
			unsigned char byte1 = pixel_data[y * pixel_width + x];
			unsigned char byte2 = pixel_data[(pixel_height - 1 - y) * pixel_width + x];

			pixel_data[(pixel_height - 1 - y) * pixel_width + x] = byte1;
			pixel_data[y * pixel_width + x] = byte2;
		}
	}
}

int calc_image_width_from_colour(unsigned char *colour_data, int pixel_width, int pixel_height)
{
	int colour_width = pixel_width;

	for (int x = pixel_width - 1; x >= 0; x--)
	{
		int y;

		for (y = 0; y < pixel_height; y++)
		{
			if (colour_data[y*pixel_width + x] != BLACK0 && colour_data[y*pixel_width + x] != BLACK1)
				break;
		}

		if (y == pixel_height)
			colour_width--;
		else
			break;
	}

	return colour_width;
}

int convert_pixels_to_colour(unsigned char *pixel_data, int pixel_width, int pixel_height, unsigned char *colour_data, bool odd, bool simple, int remove)
{
	for (int y = 0; y < pixel_height; y++)
	{
		for (int x = 0; x < pixel_width; x++)
		{
			if (simple)
			{
				// For POP data everything is group 1 (blue + orange)
				// Simplistic colour conversion - just look at pairs of pixels

				unsigned char byte1 = pixel_data[y*pixel_width + x];
				unsigned char byte2 = x < (pixel_width - 1) ? pixel_data[y*pixel_width + x + 1] : 0;
				unsigned char colour = 4;

				if (odd)
					colour |= ((byte2 & 1) << 1) | (byte1 & 1);
				else
					colour |= ((byte1 & 1) << 1) | (byte2 & 1);

				if (remove && colour == remove)
				{
					colour = BLACK1;
				}

				colour_data[y*pixel_width + x] = colour;
				if (x < (pixel_width - 1))
					colour_data[y*pixel_width + x + 1] = colour;

				x++;
			}
			else
			{
				// Use three b&w Apple II pixels to look up colour (see emulator notes)

				unsigned char byte0 = x > 0 ? pixel_data[y*pixel_width + x - 1] : 0;
				unsigned char byte1 = pixel_data[y*pixel_width + x];
				unsigned char byte2 = x < (pixel_width - 1) ? pixel_data[y*pixel_width + x + 1] : 0;

				int group0 = byte0 & 4;
				int group1 = byte1 & 4;
				int group2 = byte2 & 4;

				int pixel0 = byte0 & 1;
				int pixel1 = byte1 & 1;
				int pixel2 = byte2 & 1;

				unsigned char colour;

				if ((x & 1) == odd)
				{
					colour = group1 | odd_columns[pixel0 + pixel1 * 2 + pixel2 * 4];
				}
				else
				{
					colour = group1 | even_columns[pixel0 + pixel1 * 2 + pixel2 * 4];
				}

				if (remove && colour == remove)
				{
					colour = BLACK1;
				}

				colour_data[y*pixel_width + x] = colour;
			}
		}
	}

	return calc_image_width_from_colour(colour_data, pixel_width, pixel_height);
}

int convert_pixels_to_dhr(unsigned char *pixel_data, int pixel_width, int pixel_height, unsigned char *colour_data)
{
	for (int y = 0; y < pixel_height; y++)
	{
		for (int x = 0; x < pixel_width; x++)
		{
			// Use three b&w Apple II pixels to look up colour (see emulator notes)

			unsigned char byte0 = x > 0 ? pixel_data[y*pixel_width + x - 1] : 0;
			unsigned char byte1 = pixel_data[y*pixel_width + x];
			unsigned char byte2 = x < (pixel_width - 1) ? pixel_data[y*pixel_width + x + 1] : 0;
			unsigned char byte3 = x < (pixel_width - 2) ? pixel_data[y*pixel_width + x + 2] : 0;
			unsigned char byte4 = x < (pixel_width - 3) ? pixel_data[y*pixel_width + x + 3] : 0;

			int pixel0 = byte0 & 1;
			int pixel1 = byte1 & 1;
			int pixel2 = byte2 & 1;
			int pixel3 = byte3 & 1;
			int pixel4 = byte4 & 1;

			unsigned char colour = (pixel0 << 3) | (pixel1 << 2) | (pixel2 << 1) | (pixel3);
//			unsigned char colour = (pixel1 << 3) | (pixel2 << 2) | (pixel3 << 1) | (pixel4);

			if(colour==14)
				printf("%d%d%d%d=%d ", pixel0, pixel1, pixel2, pixel3, colour);

			colour_data[y*pixel_width + x] = colour;
		}

		printf("\n");\
	}

	return pixel_width;
}

int calc_mode5_size(unsigned char *colour_data, int pixel_width, int pixel_height, bool verbose)
{
	int expanded_width = 8 * pixel_width / 7;
	int reduced_width = expanded_width / 2;
	int mode5_width = (reduced_width + 3) / 4;
	int mode5_height = pixel_height;

	int mode5_bytes = mode5_width * mode5_height;

	if( verbose )
	{
		printf("%d x %d = %d bytes, %d x %d pixels at 2bpp half width\n", mode5_width, mode5_height, mode5_bytes, reduced_width, pixel_height);
	}

	return mode5_bytes + 4;
}

int get_colour(unsigned char *colour_data, int pixel_width, int pixel_height, int x, int y)
{
	if (x < 0 || x >= pixel_width || y < 0 || y >= pixel_height)
		return BLACK1;

	return colour_data[y * pixel_width + x];
}

void sample_apple_data(unsigned char *colour_data, int pixel_width, int pixel_height, int apple_width, int x8, int y, int &c0, int &c1, int &c2, int &c3, bool point, bool even)
{
	int width_parity = apple_width & 1;
	int x_parity = point ? 0 : ((x8 & 1)==even);

	// Or select specific pixels to double-up

	if (x_parity == 0)
	{
		c0 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 0, y);
		c1 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 2, y);
		c2 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 4, y);
		c3 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 6, y);
	}
	else
	{
		if (width_parity == 1)
		{
			c0 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 - 1, y);
			c1 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 1, y);
			c2 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 3, y);
			c3 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 5, y);
		}
		else
		{
			c0 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 1, y);
			c1 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 3, y);
			c2 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 3, y);
			c3 = get_colour(colour_data, pixel_width, pixel_height, x8 * 7 + 5, y);
		}
	}
}

int convert_colour_to_mode5(unsigned char *colour_data, int pixel_width, int pixel_height, int height_step, unsigned char *beebptr, bool test, bool point, bool even, int pal)
{
	int expanded_width = 8 * pixel_width / 7;
	int reduced_width = expanded_width / 2;
	int mode5_width = (reduced_width + 3) / 4;
	int mode5_height = pixel_height / height_step;

	int mode5_bytes = mode5_width * mode5_height;

	unsigned char *temp = beebptr;

	if (beebptr)
	{
		*beebptr++ = mode5_width;
		*beebptr++ = pixel_height; // don't tell POP that the height has changed - we'll hack that in code :(

		*beebptr++ = pal | ((height_step>1) ? 0x40 : 0);			// experiment - put palette index into sprite header!!!
	}

	for (int y = 0; y < pixel_height; y += height_step)
	{
		for (int x8 = 0; x8 < mode5_width; x8++)
		{
			unsigned char beebbyte = 0;

			// Turn 7 Apple II B&W pixels into 4 Beeb colour pixels
			// Eventually ask an artist to redraw everything

			// For now just point sample

			int c0;// = get_colour(colour_data, pixel_width, pixel_height, ((x8 * 4) + 0)*pixel_width / reduced_width, y);
			int c1;// = get_colour(colour_data, pixel_width, pixel_height, ((x8 * 4) + 1)*pixel_width / reduced_width, y);
			int c2;// = get_colour(colour_data, pixel_width, pixel_height, ((x8 * 4) + 2)*pixel_width / reduced_width, y);
			int c3;// = get_colour(colour_data, pixel_width, pixel_height, ((x8 * 4) + 3)*pixel_width / reduced_width, y);

			// Or select specific pixels to double-up

			sample_apple_data(colour_data, pixel_width, pixel_height, mode5_width, x8, y, c0, c1, c2, c3, point, even);

			// Convert 4x pixels to MODE5 2bpp

			beebbyte = beeb_mode5_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c0]][0]
				| beeb_mode5_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c1]][1]
				| beeb_mode5_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c2]][2]
				| beeb_mode5_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c3]][3];

//			printf("y=%d x8=%d c0=%d c1=%d c2=%d c3=%d l0=%d l1=%d l2=%d l3=%d p0=0x%2x p1=0x%2x p2=0x%2x p3=0x%2x b=0x%2x\n", y, x8, c0, c1, c2, c3, apple_colour_to_beeb_logical_colour[c0], apple_colour_to_beeb_logical_colour[c1], apple_colour_to_beeb_logical_colour[c2], apple_colour_to_beeb_logical_colour[c3], beeb_logical_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c0]][0], beeb_logical_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c1]][1], beeb_logical_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c2]][2], beeb_logical_colour_to_screen_pixel[apple_colour_to_beeb_logical_colour[c3]][3], beebbyte);

			*beebptr++ = beebbyte;
		}
	}

	return (beebptr - temp);
}

int get_pixel(unsigned char *pixel_data, int pixel_width, int pixel_height, int x, int y)
{
	if (x < 0 || x >= pixel_width || y < 0 || y >= pixel_height)
		return 0;

	return pixel_data[y * pixel_width + x];
}

int calc_mode4_size(unsigned char *colour_data, int pixel_width, int pixel_height, bool verbose)
{
	int expanded_width = 8 * pixel_width / 7;
	int reduced_width = expanded_width / 2;

	int mode4_width = (reduced_width + 7) / 8;
	int mode4_height = pixel_height;

	int mode4_bytes = mode4_width * mode4_height;

	if (verbose)
	{
		printf("%d x %d = %d bytes, %d x %d pixels at 1bpp half width\n", mode4_width, mode4_height, mode4_bytes, reduced_width, pixel_height);
	}

	return mode4_bytes + 4;
}

int convert_pixels_to_mode4(unsigned char *pixel_data, int pixel_width, int pixel_height, int colour_width, unsigned char *beebptr)
{
	// In this case colour_width is <= pixel_width
	// Now not using colour_width

	int mode4_width = (pixel_width + 7) / 8;
	int mode4_height = pixel_height;

	int mode4_bytes = mode4_width * mode4_height;

	unsigned char *temp = beebptr;

	if (beebptr)
	{
		*beebptr++ = mode4_width;
		*beebptr++ = mode4_height;

		for (int y = 0; y < mode4_height; y++)
		{
			for (int x8 = 0; x8 < mode4_width; x8++)
			{
				unsigned char beebbyte = 0;
				
				for (int p = 0; p < 8; p++)
				{
					if( get_pixel(pixel_data, pixel_width, pixel_height, x8 * 8 + p, y) & 0x3 )
						beebbyte |= 1 << (7 - p);
				}

				*beebptr++ = beebbyte;
			}
		}
	}

	return 2 + mode4_bytes;
}

int calc_attribute6_size(unsigned char *colour_data, int pixel_width, int pixel_height, bool verbose, bool point)
{
	int expanded_width = 8 * pixel_width / 7;
	int reduced_width = expanded_width / 2;

	int mode5_width = (reduced_width + 3) / 4;
	int mode5_height = pixel_height;

	// We turn 3 pixels into 4 bits (1x attribute bit + 3x pixel bits)

	int attr_bytes = (((((mode5_width * 4) + 2) / 3) + 1) / 2) * mode5_height;

	if (verbose)
	{
		printf("%d x %d = %d bytes, %d x %d trixels at 1.333bpp\n", (((((mode5_width * 4) + 2) / 3) + 1) / 2), mode5_height, attr_bytes, ((mode5_width * 4) + 2) / 3, mode5_height);
	}

	return attr_bytes + 4;
}

static int CrnDatPtr;

void PutScrByte(unsigned char ByteHld, int XClmPos, int YScrPos)
{
	image_data[0][YScrPos * 80 + XClmPos] = ByteHld;
}

void ExpClmSeq(unsigned char ByteHld, unsigned char ByteCount, int XClmPos, int &YScrPos)
{
	while (ByteCount)
	{
		PutScrByte(ByteHld, XClmPos, YScrPos);

		YScrPos += 2;

		ByteCount--;
	}
}

void ExpandClm(int XClmPos, int YScrPos)
{
	while (YScrPos < 192)
	{
		unsigned char ByteHld = imagetab[CrnDatPtr];

		if (ByteHld & 0x80)
		{
			unsigned char ByteCount = imagetab[CrnDatPtr + 1];

			ExpClmSeq(ByteHld & 0x7f, ByteCount, XClmPos, YScrPos);

			CrnDatPtr += 2;
		}
		else
		{
			// ExpandOne

			ExpClmSeq(ByteHld, 1, XClmPos, YScrPos);

			CrnDatPtr++;
		}
	}
}

void unpack_double_hires(void)
{
	CrnDatPtr = 1;

	// WipeRgtExp

	int XClmPos = 0;

	while (XClmPos < 80)
	{
		ExpandClm(XClmPos, 0);
		ExpandClm(XClmPos, 1);

		XClmPos++;
	}
}

int get_distance_to_colour(int i, unsigned char r, unsigned char g, unsigned char b)
{
	return (r - nula_colours[i][0]) * (r - nula_colours[i][0]) + (g - nula_colours[i][1]) * (g - nula_colours[i][1]) + (b - nula_colours[i][2]) * (b - nula_colours[i][2]);
}

int find_nearest_nula_colour(unsigned char r, unsigned char g, unsigned char b)
{
	int c = 0;
	int min_distance = INT_MAX;

	for (int i = 0; i < 16; i++)
	{
		int distance = get_distance_to_colour(i, r, g, b);

		if (distance < min_distance)
		{
			c = i;
			min_distance = distance;
		}
	}

	return c;
}

int get_beeb_byte_for_palette(int index, unsigned char r, unsigned char g, unsigned char b, int pixel)
{
	int c = 0;
	int min_distance = get_distance_to_colour(0, r, g, b);

	for (int i = 1; i < 4; i++)
	{
		int distance = get_distance_to_colour(palette_selection[index][i-1], r, g, b);
		if ( distance < min_distance )
		{
			c = i;
			min_distance = distance;
		}
	}

	return beeb_mode5_colour_to_screen_pixel[c][pixel];
}


int get_beeb_byte_for_colours_in_palette_selection(int index, int c0, int c1, int c2, int c3)
{
	int p0=0, p1=0, p2=0, p3=0;

	for (int m = 0; m < 3; m++)
	{
		if (palette_selection[index][m] == c0) p0 = m+1;
		if (palette_selection[index][m] == c1) p1 = m+1;
		if (palette_selection[index][m] == c2) p2 = m+1;
		if (palette_selection[index][m] == c3) p3 = m+1;
	}

	return beeb_mode5_colour_to_screen_pixel[p0][0] | beeb_mode5_colour_to_screen_pixel[p1][1] | beeb_mode5_colour_to_screen_pixel[p2][2] | beeb_mode5_colour_to_screen_pixel[p3][3];
}

int get_palette_selection_for_colour(unsigned char *colour_data, int pixel_width, int pixel_height, int ystep)
{
	int counts[MAX_PALETTES];
	int num_colours = 0;
	int palette[MAX_PALETTES];

	for (int c = 0; c < MAX_PALETTES; c++) counts[c] = 0;

	for (int y = 0; y < pixel_height; y+=ystep)
	{
		for (int x = 0; x < pixel_width; x++)
		{
			counts[colour_data[y*pixel_width + x]]++;
		}
	}

	for (int c = 1; c < MAX_PALETTES; c++)
	{
		if (counts[c] > 0)
			palette[num_colours++] = c;
	}

	if( num_colours > 3 )
		printf("WARNING: Found %d colours in sprite!\n", num_colours);

	for (int p = 0; p < MAX_PALETTES; p++)
	{
		int m;

		for (m = 0; m < 3 && m < num_colours; m++)
		{
		// Does this palette colour match one of this selection?

			if (!(palette[m] == palette_selection[p][0] || palette[m] == palette_selection[p][1] || palette[m] == palette_selection[p][2]))
				break;
		}

		if (m >= num_colours || m >= 3)
			return p;
	}

	printf("WARNING: Failed to match a palette selection against [%d %d %d]\n", palette[0], palette[1], palette[2]);

	return 0;
}

void process_font(const char *bitmapname, int font_width, int font_height,const char *outputname, bool verbose)
{
	printf("Reading bitmap data from '%s'...\n", bitmapname);

	CImg<unsigned char> bitmap(bitmapname);

	int total_width = bitmap.width();

	//		int num_glyphs = total_width / (font_width * 2 + 8);
	//		printf("Num glyphs = %d\n", num_glyphs);

	int current_x = 0;

	while (current_x < total_width)
	{
		if (bitmap(current_x, 1, 0) == 127 && bitmap(current_x, 1, 1) == 127 && bitmap(current_x, 1, 2) == 127)
		{
			break;
		}

		current_x++;
	}

	current_x++;

	int max_height = font_height + 2;
	int num_glyphs = 0;

	while (current_x < total_width)
	{
		int x = 0;

		while (current_x + x < total_width)
		{
			if (bitmap(current_x + x, 1, 0) == 127 && bitmap(current_x + x, 1, 1) == 127 && bitmap(current_x + x, 1, 2) == 127)
			{
				break;
			}
			x++;
		}

		if (verbose)
		{
			printf("Glyph %d has %d pixels starting at x=%d\n", num_glyphs, x, current_x);
		}

		pixel_size[num_glyphs][0] = current_x;
		colour_width[num_glyphs] = x / 2;
		num_glyphs++;

		current_x += x + 1;

		while (current_x < total_width)
		{
			if (bitmap(current_x, 1, 0) == 127 && bitmap(current_x, 1, 1) == 127 && bitmap(current_x, 1, 2) == 127)
			{
				break;
			}

			current_x++;
		}

		current_x++;
	}

	printf("Found %d glyphs total\n", num_glyphs);

	for (int i = 0; i < num_glyphs; i++)
	{
		int pixel_height = font_height;
		int current_y = 1;

		int current_x = pixel_size[i][0];

		for (int y = 0; y < pixel_height; y += 1)
		{
			int actual_y = y;

			for (int x = 0; x < colour_width[i]; x++)
			{
				//	printf("%d %d %d %d\n", c0, c1, c2, c3);

				int c = find_nearest_nula_colour(bitmap(current_x + x * 2, current_y + actual_y, 0), bitmap(current_x + x * 2, current_y + actual_y, 1), bitmap(current_x + x * 2, current_y + actual_y, 2));;

			//	printf("%d ", c);

				colours[i][y * colour_width[i] + x] = c;
			}

		//	printf("\n");
		}

	//	printf("\n");
	}

	if (outputname)
	{
		FILE *output = fopen(outputname, "wb");

		if (output)
		{
			unsigned char *beebdata = (unsigned char*)malloc(3 + num_glyphs * 5 + num_glyphs * (font_width / 4) * font_height);
			unsigned char *beebptr = beebdata;

			*beebptr++ = font_height;	// if we put this first then everything that follows looks like a regular sprite table
			*beebptr++ = num_glyphs;

			for (int i = 0; i < num_glyphs; i++)
			{
				*beebptr++ = 0xff;
				*beebptr++ = 0xff;		// don't know pointers yet
			}
			*beebptr++ = 0xff;
			*beebptr++ = 0xff;			// don't know free yet

			// Write Beeb data

			for (int i = 0; i < num_glyphs; i++)
			{
				int pixel_height = font_height;
				int current_y = 1;

				int palsel = get_palette_selection_for_colour(colours[i], colour_width[i], pixel_height, 1);

				if (verbose)
				{
					printf("Glyph %d matched palette %d\n", i + 1, palsel);
				}

				// Now we know our address

				beebdata[2 + i * 2] = LO(beebptr - beebdata);
				beebdata[3 + i * 2] = HI(beebptr - beebdata);

				// Write bytes directly from bitmap

				*beebptr++ = colour_width[i] / 2;			// this many MODE 2 bytes

															// Don't write these for fonts
															//	*beebptr++ = pixel_height;	// write this once for all glyphs
															//	*beebptr++ = pal;			// experiment - put palette index into sprite header!!!

				for (int y = 0; y < pixel_height; y += 1)
				{
					for (int x = 0; x < colour_width[i]; x += 4)
					{
						int c0 = colours[i][y * colour_width[i] + x + 0];
						int c1 = (x + 1) < colour_width[i] ? colours[i][y * colour_width[i] + x + 1] : 0;
						int c2 = (x + 2) < colour_width[i] ? colours[i][y * colour_width[i] + x + 2] : 0;
						int c3 = (x + 3) < colour_width[i] ? colours[i][y * colour_width[i] + x + 3] : 0;

						//	printf("%d %d %d %d ", c0, c1, c2, c3);

						unsigned char beebbyte = get_beeb_byte_for_colours_in_palette_selection(palsel, c0, c1, c2, c3);

						//	unsigned char r0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 0), g0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 1), b0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 2);
						//	unsigned char r1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 0), g1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 1), b1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 2);
						//	unsigned char r2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 0), g2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 1), b2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 2);
						//	unsigned char r3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 0), g3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 1), b3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 2);

						//	unsigned char beebbyte = get_beeb_byte_for_palette(palsel, r0, g0, b0, 0) | get_beeb_byte_for_palette(palsel, r1, g1, b1, 1) | get_beeb_byte_for_palette(palsel, r2, g2, b2, 2) | get_beeb_byte_for_palette(palsel, r3, g3, b3, 3);

						*beebptr++ = beebbyte;
					}

					//	printf("\n");
				}
			}

			// Write free address

			beebdata[2 + num_glyphs * 2] = LO(beebptr - beebdata);
			beebdata[3 + num_glyphs * 2] = HI(beebptr - beebdata);

			// Write file

			fwrite(beebdata, 1, beebptr - beebdata, output);
			fclose(output);
			output = NULL;

			printf("Output bytes written = %d\n", beebptr - beebdata);
			printf("Average %d bytes per glyph\n", (beebptr - beebdata) / num_glyphs);

			free(beebdata);
			beebdata = NULL;
		}
	}
}

void process_dhr(const char *inputname)
{
	unpack_double_hires();

	image_size[0][0] = 80;
	image_size[0][1] = 192;

	pixel_size[0][0] = convert_apple_to_pixels(image_data[0], image_size[0][0], image_size[0][1], pixels[0]);
	pixel_size[0][1] = image_size[0][1];

	colour_width[0] = convert_pixels_to_dhr(pixels[0], pixel_size[0][0], pixel_size[0][1], colours[0]);

	CImg<unsigned char> img(pixel_size[0][0], pixel_size[0][1], 1, 3, 0);	// was total_width

	int current_x = 0;

	int height = pixel_size[0][1];
	int current_y = 0;
	int width = pixel_size[0][0];
	unsigned char color[] = { 255, 255, 255 };

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			img(current_x + x, current_y + y, 0) = dhr_palette[colours[0][y*width + x]][0];
			img(current_x + x, current_y + y, 1) = dhr_palette[colours[0][y*width + x]][1];
			img(current_x + x, current_y + y, 2) = dhr_palette[colours[0][y*width + x]][2];

			//	img(current_x + x, current_y + y, 0) = (pixels[0][y*width + x] & 1) * 255;
			//	img(current_x + x, current_y + y, 1) = (pixels[0][y*width + x] & 1) * 255;
			//	img(current_x + x, current_y + y, 2) = (pixels[0][y*width + x] & 1) * 255;
		}
	}

	//	img.draw_rectangle(current_x - 1, current_y - 1, current_x + width, current_y + height, color, 0.5f, 0xffffffff);

	char testname[256];
	sprintf(testname, "%s.png", inputname);
	img.save(testname);
}

int main(int argc, char **argv)
{
	cimg_usage("POP asset convertor.\n\nUsage : pop2beeb [options]");
	const char *const inputname = cimg_option("-i", (char*)0, "Input filename");
	const char *const outputname = cimg_option("-o", (char*)0, "Output filename");
	const char *const bitmapname = cimg_option("-b", (char*)0, "Bitmap filename to use instead of automatic conversion");
	const int mode = cimg_option("-mode", 5, "BBC MODE number (6='attribute' mode)");
	const int remove = cimg_option("-remove", 0, "Remove Apple II colour # from data");
	const int pal = cimg_option("-pal", 0, "Palette selection");
	const bool test = cimg_option("-test", false, "Save test images");
	const bool flip = cimg_option("-flip", false, "Flip pixels in Y");
	const bool halfv = cimg_option("-halfv", false, "Halve vertical resolution");
	const bool simple = cimg_option("-simple", false, "Use simple colour conversion");
	const bool point = cimg_option("-point", true, "Use simple point sample (best for characters)");
	const bool even = cimg_option("-even", true, "Start with odd or even bytes when parity sampling (DUN=true PAL=false)");
	const bool verbose = cimg_option("-v", false, "Verbose output");
	const bool dhr = cimg_option("-dhr", false, "Double hi-res pac file input not sprite table");
	int start_image = cimg_option("-s", 1, "Start image #");
	int end_image = cimg_option("-e", 127, "End image #");
	const char *const fontgeom = cimg_option("-font", (char*)0, "Font geometry wxh");
	int font_width = 0, font_height = 0;

	if (cimg_option("-h", false, 0)) std::exit(0);

	if (fontgeom)
	{
		std::sscanf(fontgeom, "%d%*c%d", &font_width, &font_height);

		if (font_width <= 0 || font_height <= 0)
		{
			printf("Bad font geometry: %d x %d\n", font_width, font_height);
			std::exit(0);
		}

		if (bitmapname == NULL)
		{
			printf("Need bitmap for font. Use -b option.\n");
			std::exit(0);
		}

		process_font(bitmapname, font_width, font_height, outputname, verbose);

		std::exit(0);
	}

	if (inputname == NULL)  std::exit(0);

	FILE *input = fopen(inputname, "rb");
	if (!input) std::exit(0);

	char parityfile[256];
	sprintf(parityfile, "%s.txt", inputname);

	FILE *parity = fopen(parityfile, "rb");

	char halffile[256];
	sprintf(halffile, "%s.half.txt", inputname);

	FILE *halff = fopen(halffile, "rb");

	fread(imagetab, 1, 48 * 1024, input);				// forgotten how to file length of file!
	fclose(input);
	input = NULL;

	if (dhr)
	{
		process_dhr(inputname);

		std::exit(0);
	}

	int num_images = imagetab[0];

	printf("Num images = %d\n", num_images);
	if (verbose)
	{
		printf("Image addresses:\n");
	}

	for (int i = 0; i < num_images; i++)
	{
		image_addrs[i] = GET_16BIT(imagetab + 1 + i * 2);
		if (verbose)
		{
			printf("[%d] 0x%x\n", i+1, image_addrs[i]);
		}
	}
	image_addrs[num_images] = GET_16BIT(imagetab + 1 + num_images * 2);
	printf("First free address = 0x%x (%d)\n", image_addrs[num_images], image_addrs[num_images]-0x6000);

	unsigned char *image_ptr = imagetab + 1 + num_images * 2 + 2;

	int total_bytes = 3;
	int total_width = 0;
	int max_height = 0;

	for (int i = 0; i < num_images; i++)
	{
		image_size[i][0] = *image_ptr++;
		image_size[i][1] = *image_ptr++;

		int bytes = image_size[i][0] * image_size[i][1];
		total_bytes += 4 + bytes;

		for (int d = 0; d < bytes; d++)
		{
			image_data[i][d] = *image_ptr++;
		}

		pixel_size[i][0] = convert_apple_to_pixels(image_data[i], image_size[i][0], image_size[i][1], pixels[i]);
		pixel_size[i][1] = image_size[i][1];

		if (pixel_size[i][1] > max_height)
			max_height = pixel_size[i][1];

		total_width += pixel_size[i][0] + 8;

		if( flip )
		{
			flip_pixels_in_y(pixels[i], pixel_size[i][0], pixel_size[i][1]);
		}
		
		bool odd = 1;

		if (parity)
		{
			odd = fgetc(parity) == '1' ? 1 : 0;
		}

		if (halff)
		{
			half[i] = fgetc(halff) == '1' ? 1 : 0;
		}
		else
		{
			half[i] = halfv;
		}

		if (verbose)
		{
			printf("Image %d: %d x %d = %d bytes, %d x %d pixels, parity=%d halfv=%d\n", i + 1, image_size[i][0], image_size[i][1], bytes, pixel_size[i][0], pixel_size[i][1], odd, half[i]);
		}

		colour_width[i] = convert_pixels_to_colour(pixels[i], pixel_size[i][0], pixel_size[i][1], colours[i], odd, simple, remove);
	}

	max_height += 2;

	if (parity)
	{
		fclose(parity);
		parity = NULL;
	}
	
	if (test && flip)
	{
		if (verbose)
		{
			printf("Test: %d x %d\n", total_width, max_height);
		}

		int mode5_total_width = total_width - (num_images * 8);
		mode5_total_width = 8 * mode5_total_width / 7;
		mode5_total_width += num_images * 8;

		CImg<unsigned char> img(mode5_total_width, max_height, 1, 3, 0);	// was total_width

		int current_x = 4;

		for (int i = 0; i < num_images; i++)
		{
			int height = pixel_size[i][1];
			int current_y = max_height - height - 1;
			int width = pixel_size[i][0];
			unsigned char color[] = { 255, 255, 255 };

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					img(current_x + x, current_y + y, 0) = palette[colours[i][y*width + x]][0];
					img(current_x + x, current_y + y, 1) = palette[colours[i][y*width + x]][1];
					img(current_x + x, current_y + y, 2) = palette[colours[i][y*width + x]][2];
				}
			}

			img.draw_rectangle(current_x - 1, current_y - 1, current_x + width, current_y + height, color, 0.5f, 0xffffffff);

			int expanded_width = 8 * width / 7;
			int reduced_width = expanded_width / 2;
			int mode5_width = (reduced_width + 3) / 4;
			current_x += (mode5_width * 4 * 2) + 8;
//			current_x += width + 8;
		}

		char testname[256];
		sprintf(testname, "%s.png", inputname);
		img.save(testname);
	}


// This should really be dumped when writing MODE 5 data so is actual code being used to generate Beeb data //

	if (test && mode == 5 && !flip)
	{
		int mode5_total_width = total_width - (num_images * 8);
		mode5_total_width = 8 * mode5_total_width / 7;
		mode5_total_width += num_images * 8;

		if (verbose)
		{
			printf("Test: %d x %d\n", mode5_total_width, max_height);
		}

		CImg<unsigned char> img(mode5_total_width, max_height, 1, 3, 0);

		int current_x = 4;

		for (int i = 0; i < num_images; i++)
		{
			int pixel_height = pixel_size[i][1];
			int current_y = max_height - pixel_height - 1;
			unsigned char color[] = { 255, 255, 255 };

			int pixel_width = pixel_size[i][0];
			int expanded_width = 8 * pixel_width / 7;
			int reduced_width = expanded_width / 2;
			int mode5_width = (reduced_width + 3) / 4;

			for (int y = 0; y < pixel_height; y++)
			{
				int actual_y = (pixel_height - 2 - y);			// assume flip

				for (int x8 = 0, x=0; x8 < mode5_width; x8++)
				{
					int c[4];

					sample_apple_data(colours[i], pixel_width, pixel_height, mode5_width, x8, y, c[0], c[1], c[2], c[3], point, even);

					for (int j = 0; j < 4; j++)
					{
						if (half[i] && y < pixel_height - 1)
						{
							img(current_x + x, current_y + actual_y, 0) = palette[c[j]][0];
							img(current_x + x, current_y + actual_y, 1) = palette[c[j]][1];
							img(current_x + x, current_y + actual_y, 2) = palette[c[j]][2];
						}

						img(current_x + x, current_y + actual_y + 1, 0) = palette[c[j]][0];
						img(current_x + x, current_y + actual_y + 1, 1) = palette[c[j]][1];
						img(current_x + x, current_y + actual_y + 1, 2) = palette[c[j]][2];

						x++;

						if (half[i] && y < pixel_height - 1)
						{
							img(current_x + x, current_y + actual_y, 0) = palette[c[j]][0];
							img(current_x + x, current_y + actual_y, 1) = palette[c[j]][1];
							img(current_x + x, current_y + actual_y, 2) = palette[c[j]][2];
						}

						img(current_x + x, current_y + actual_y + 1, 0) = palette[c[j]][0];
						img(current_x + x, current_y + actual_y + 1, 1) = palette[c[j]][1];
						img(current_x + x, current_y + actual_y + 1, 2) = palette[c[j]][2];

						x++;
					}
				}

				if (half[i])	y++;
			}

			img.draw_rectangle(current_x-1, current_y-1, current_x + (mode5_width * 4 * 2), current_y + pixel_height, color, 0.5f, 0xffffffff);

			current_x += (mode5_width * 4 * 2) + 8;
		}

		char testname[256];
		sprintf(testname, "%s.mode5.png", inputname);
		img.save(testname);
	}


// This should really be dumped when writing ATTRIBUTE 6 data so is actual code being used to generate Beeb data //

	if (test && mode == 6)
	{
		int mode5_total_width = total_width - (num_images * 8);
		mode5_total_width = 8 * mode5_total_width / 7;
		mode5_total_width += num_images * 8;

		if (verbose)
		{
			printf("Test: %d x %d\n", mode5_total_width, max_height);
		}

		CImg<unsigned char> img(mode5_total_width, max_height, 1, 3, 0);

		int current_x = 4;

		for (int i = 0; i < num_images; i++)
		{
			int pixel_height = pixel_size[i][1];
			int current_y = max_height - pixel_height - 1;
			unsigned char color[] = { 255, 255, 255 };

			int pixel_width = pixel_size[i][0];
			int expanded_width = 8 * pixel_width / 7;
			int reduced_width = expanded_width / 2;
			int mode5_width = (reduced_width + 3) / 4;

			for (int y = 0; y < pixel_height; y++)
			{
				int c[256];

				for (int x8 = 0, x = 0; x8 < mode5_width; x8++, x += 4)
				{
					sample_apple_data(colours[i], pixel_width, pixel_height, mode5_width, x8, y, c[x + 0], c[x + 1], c[x + 2], c[x + 3], point, even);
				}

				for(int p=0, x=0; p < (mode5_width*4); p += 3)
				{
					int num_white = 0;
					int num_orange = 0;

					for (int j = 0; j < 3; j++)
					{
						if (c[p + j] == ORANGE) num_orange++;
						if (c[p + j] == WHITE1) num_white++;
					}

					// Choose attribute
					int attribute = num_orange >= num_white ? ORANGE : WHITE1;

					for (int j = 0; j < 3 && (p+j) < (mode5_width*4); j++)
					{
						int pixel = c[p + j] != BLACK1 ? attribute : BLACK1;

						img(current_x + x, current_y + y, 0) = palette[pixel][0];
						img(current_x + x, current_y + y, 1) = palette[pixel][1];
						img(current_x + x, current_y + y, 2) = palette[pixel][2];

						if (half[i] && y < pixel_height - 1)
						{
							img(current_x + x, current_y + y + 1, 0) = palette[pixel][0];
							img(current_x + x, current_y + y + 1, 1) = palette[pixel][1];
							img(current_x + x, current_y + y + 1, 2) = palette[pixel][2];

						}

						x++;

						img(current_x + x, current_y + y, 0) = palette[pixel][0];
						img(current_x + x, current_y + y, 1) = palette[pixel][1];
						img(current_x + x, current_y + y, 2) = palette[pixel][2];

						if (half[i] && y < pixel_height - 1)
						{
							img(current_x + x, current_y + y + 1, 0) = palette[pixel][0];
							img(current_x + x, current_y + y + 1, 1) = palette[pixel][1];
							img(current_x + x, current_y + y + 1, 2) = palette[pixel][2];

						}
						
						x++;
					}
				}

				if (half[i])	y++;
			}

			img.draw_rectangle(current_x - 1, current_y - 1, current_x + (mode5_width * 4 * 2), current_y + pixel_height, color, 0.5f, 0xffffffff);

			current_x += (mode5_width * 4 * 2) + 8;
		}

		char testname[256];
		sprintf(testname, "%s.mode5.attr.png", inputname);
		img.save(testname);
	}


	int total_mode5 = 3;		// num_images + free ptr
	int total_mode4 = 3;
	int total_attr6 = 3;

	for (int i = 0; i < num_images; i++)
	{
		if (mode == 5 || mode == 2)
		{
			if (verbose)
			{
				printf("Image[%d]: MODE5=", i+1);
			}
			total_mode5 += calc_mode5_size(colours[i], pixel_size[i][0], pixel_size[i][1], verbose);
		}

		if (mode == 4)
		{
			if (verbose)
			{
				printf("Image[%d]: MODE4=", i+1);
			}
			total_mode4 += calc_mode4_size(colours[i], pixel_size[i][0], pixel_size[i][1], verbose);
		}

		if (mode == 6)
		{
			if (verbose)
			{
				printf("Image [%d]: ATTR6=", i + 1);
			}

			total_attr6 += calc_attribute6_size(colours[i], pixel_size[i][0], pixel_size[i][1], verbose, point);
		}
	}

	printf("Original Apple bytes = %d\n", total_bytes);

	if (mode == 5 || mode == 2)
	{
		printf("Total MODE5 bytes = %d\n", total_mode5);
		printf("Size vs Apple = %f%%\n", 100.0f * total_mode5 / (float)total_bytes);

	}

	if (mode == 4)
	{
		printf("Total MODE4 bytes = %d\n", total_mode4);
		printf("Size vs Apple = %f%%\n", 100.0f * total_mode4 / (float)total_bytes);
	}

	if (mode == 6)
	{
		printf("Total ATTRIBUTE6 bytes = %d\n", total_attr6);
		printf("Size vs Apple = %f%%\n", 100.0f * total_attr6 / (float)total_bytes);
	}

	if (bitmapname)
	{
		int mode5_total_width = total_width - (num_images * 8);
		mode5_total_width = 8 * mode5_total_width / 7;
		mode5_total_width += num_images * 8;

		for (int i = 0; i < MAX_PALETTES; i++) palette_used[i] = 0;

		printf("Reading bitmap data from '%s'...\n", bitmapname);

		CImg<unsigned char> bitmap(bitmapname);

		printf("Converting bitmap data to sprites...\n");

		int current_x = 4;

		for (int i = 0; i < num_images; i++)
		{
			int pixel_height = pixel_size[i][1];

			int pixel_width = pixel_size[i][0];
			int expanded_width = 8 * pixel_width / 7;
			int reduced_width = expanded_width / 2;
			int mode5_width = (reduced_width + 3) / 4;

			colour_width[i] = mode5_width * 4;

			// Hacky way to look for the height of the sprite in the image

			int current_y = 0;

			while (current_y < max_height)
			{
				if (bitmap(current_x, current_y, 0) == 127 && bitmap(current_x, current_y, 1) == 127 && bitmap(current_x, current_y, 2) == 127)
				{
					break;
				}
				current_y++;
			}

			int found_height = 0;
			current_y++;

			while (current_y < max_height)
			{
				if (bitmap(current_x, current_y, 0) == 127 && bitmap(current_x, current_y, 1) == 127 && bitmap(current_x, current_y, 2) == 127)
				{
					break;
				}
				current_y++;
				found_height++;
			}

			// Check this against the size in the image table just in case of cut & paste errors!

			if (found_height != pixel_height)
			{
				printf("WARNING: Image %d found height=%d but pixel height=%d\n", i + 1, found_height, pixel_height);
			}

			current_y = max_height - pixel_height - 1;

			for (int y = 0; y < pixel_height; y += (half[i] ? 2 : 1))
			{
				int actual_y = !flip ? (pixel_height - 1 - y) : y;

				for (int x8 = 0; x8 < mode5_width; x8++)
				{
					int c0 = find_nearest_nula_colour(bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 0), bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 1), bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 2));
					int c1 = find_nearest_nula_colour(bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 0), bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 1), bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 2));
					int c2 = find_nearest_nula_colour(bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 0), bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 1), bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 2));
					int c3 = find_nearest_nula_colour(bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 0), bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 1), bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 2));

//					unsigned char r0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 0), g0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 1), b0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 2);
//					unsigned char r1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 0), g1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 1), b1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 2);
//					unsigned char r2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 0), g2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 1), b2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 2);
//					unsigned char r3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 0), g3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 1), b3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 2);

					colours[i][y * colour_width[i] + x8 * 4 + 0] = c0;
					colours[i][y * colour_width[i] + x8 * 4 + 1] = c1;
					colours[i][y * colour_width[i] + x8 * 4 + 2] = c2;
					colours[i][y * colour_width[i] + x8 * 4 + 3] = c3;

				//	printf("%d %d %d %d ", c0, c1, c2, c3);
				}

			//	printf("\n");
			}

			current_x += (mode5_width * 4 * 2) + 8;
		}

		if (outputname)
		{
			FILE *output = fopen(outputname, "wb");

			if (output)
			{
				unsigned char *beebdata = (unsigned char*)malloc((total_mode5 + num_images * 4 + 3) * 2);
				unsigned char *beebptr = beebdata;

				if (end_image > num_images)
					end_image = num_images;

				num_images = end_image - start_image + 1;

				printf("Output start image = %d\nOutput end image = %d\nOutput total images = %d\n", start_image, end_image, num_images);

				*beebptr++ = num_images;

				for (int i = start_image - 1; i < end_image; i++)
				{
					*beebptr++ = 0xff;
					*beebptr++ = 0xff;		// don't know pointers yet
				}
				*beebptr++ = 0xff;
				*beebptr++ = 0xff;			// don't know free yet

											// Write Beeb data
				for (int j = 0, i = start_image - 1; i < end_image; i++, j++)
				{
					int pixel_height = pixel_size[i][1];
					int current_y = max_height - pixel_height - 1;

					int pixel_width = pixel_size[i][0];
					int expanded_width = 8 * pixel_width / 7;
					int reduced_width = expanded_width / 2;
					int mode5_width = (reduced_width + 3) / 4;

					// Now we know our address

					beebdata[1 + j * 2] = LO(beebptr - beebdata);
					beebdata[2 + j * 2] = HI(beebptr - beebdata);

					*beebptr++ = mode5_width;	// don't tell POP that the width has changed for MODE 2 - we'll hack that in code :)
					*beebptr++ = pixel_height; // don't tell POP that the height has changed - we'll hack that in code :(

					if (mode == 2)
					{
						// Write bytes directly from bitmap

						*beebptr++ = 0x80 | (half[i]?0x40:0);			// no palette - store MODE 2 pixel pairs directly

						for (int y = 0; y < pixel_height; y += (half[i] ? 2 : 1))
						{
							for (int x8 = 0; x8 < mode5_width; x8++)
							{
								int c0 = colours[i][y * colour_width[i] + x8 * 4 + 0];
								int c1 = colours[i][y * colour_width[i] + x8 * 4 + 1];

								int c2 = colours[i][y * colour_width[i] + x8 * 4 + 2];
								int c3 = colours[i][y * colour_width[i] + x8 * 4 + 3];

								//	printf("%d %d %d %d ", c0, c1, c2, c3);

								*beebptr++ = beeb_mode2_colour_to_screen_pixel[c0][0] | beeb_mode2_colour_to_screen_pixel[c1][1];
								*beebptr++ = beeb_mode2_colour_to_screen_pixel[c2][0] | beeb_mode2_colour_to_screen_pixel[c3][1];
							}

							//	printf("\n");
						}
					}
					else
					{
						int palsel = get_palette_selection_for_colour(colours[i], colour_width[i], pixel_height, (half[i] ? 2 : 1));

						if (verbose)
						{
							printf("Image %d matched palette %d\n", i + 1, palsel);
						}

						palette_used[palsel]++;

						// Write bytes directly from bitmap

						*beebptr++ = palsel | (half[i] ? 0x40 : 0);			// experiment - put palette index into sprite header!!!

						for (int y = 0; y < pixel_height; y += (half[i] ? 2 : 1))
						{
							for (int x8 = 0; x8 < mode5_width; x8++)
							{
								int c0 = colours[i][y * colour_width[i] + x8 * 4 + 0];
								int c1 = colours[i][y * colour_width[i] + x8 * 4 + 1];
								int c2 = colours[i][y * colour_width[i] + x8 * 4 + 2];
								int c3 = colours[i][y * colour_width[i] + x8 * 4 + 3];

								//	printf("%d %d %d %d ", c0, c1, c2, c3);

								unsigned char beebbyte = get_beeb_byte_for_colours_in_palette_selection(palsel, c0, c1, c2, c3);

								//	unsigned char r0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 0), g0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 1), b0 = bitmap(current_x + x8 * 8 + 0, current_y + actual_y, 2);
								//	unsigned char r1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 0), g1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 1), b1 = bitmap(current_x + x8 * 8 + 2, current_y + actual_y, 2);
								//	unsigned char r2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 0), g2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 1), b2 = bitmap(current_x + x8 * 8 + 4, current_y + actual_y, 2);
								//	unsigned char r3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 0), g3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 1), b3 = bitmap(current_x + x8 * 8 + 6, current_y + actual_y, 2);

								//	unsigned char beebbyte = get_beeb_byte_for_palette(palsel, r0, g0, b0, 0) | get_beeb_byte_for_palette(palsel, r1, g1, b1, 1) | get_beeb_byte_for_palette(palsel, r2, g2, b2, 2) | get_beeb_byte_for_palette(palsel, r3, g3, b3, 3);

								*beebptr++ = beebbyte;
							}

							//	printf("\n");
						}
					}

					current_x += (mode5_width * 4 * 2) + 8;
				}

				// Write free address

				beebdata[1 + num_images * 2] = LO(beebptr - beebdata);
				beebdata[2 + num_images * 2] = HI(beebptr - beebdata);

				// Write file

				fwrite(beebdata, 1, beebptr - beebdata, output);
				fclose(output);
				output = NULL;

				printf("Output bytes written = %d\n", beebptr - beebdata);
				printf("Size vs original = %f%%\n", 100.0f * (beebptr - beebdata) / (float)total_bytes);

				free(beebdata);
				beebdata = NULL;

				if (verbose)
				{
					printf("USED PALETTES: ");
					for (int i = 0; i < MAX_PALETTES; i++)
						if (palette_used[i]) printf("%d ", i);
					printf("\n");
				}
			}
		}
	}
	else
	{
		if (outputname)
		{
			FILE *output = fopen(outputname, "wb");

			if (output)
			{
				unsigned char *beebdata = (unsigned char*)malloc((mode == 4 ? total_mode4 : total_mode5) + num_images * 4 + 3);
				unsigned char *beebptr = beebdata;

				if (end_image > num_images)
					end_image = num_images;

				num_images = end_image - start_image + 1;

				printf("Output start image = %d\nOutput end image = %d\nOutput total images = %d\n", start_image, end_image, num_images);

				*beebptr++ = num_images;

				for (int i = start_image - 1; i < end_image; i++)
				{
					*beebptr++ = 0xff;
					*beebptr++ = 0xff;		// don't know pointers yet
				}
				*beebptr++ = 0xff;
				*beebptr++ = 0xff;			// don't know free yet

				// Write Beeb data

				for (int j = 0, i = start_image - 1; i < end_image; i++, j++)
				{
					// Now we know our address

					beebdata[1 + j * 2] = LO(beebptr - beebdata);
					beebdata[2 + j * 2] = HI(beebptr - beebdata);

					int bytes_written = 0;

					if (mode == 4)
					{
						bytes_written += convert_pixels_to_mode4(pixels[i], pixel_size[i][0], pixel_size[i][1], colour_width[i], beebptr);
					}

					if (mode == 5)
					{
						bytes_written += convert_colour_to_mode5(colours[i], pixel_size[i][0], pixel_size[i][1], half[i] ? 2 : 1, beebptr, test, point, even, pal);
					}

					if (mode == 6)
					{
						// TODO
						//	bytes_written += convert_colour_to_attr6(colours[i], pixel_size[i][0], pixel_size[i][1], half[i] ? 2 : 1, beebptr, test, point);
					}

					beebptr += bytes_written;
				}

				// Write free address

				beebdata[1 + num_images * 2] = LO(beebptr - beebdata);
				beebdata[2 + num_images * 2] = HI(beebptr - beebdata);

				// Write file

				fwrite(beebdata, 1, beebptr - beebdata, output);
				fclose(output);
				output = NULL;

				printf("Output bytes written = %d\n", beebptr - beebdata);
				printf("Size vs original = %f%%\n", 100.0f * (beebptr - beebdata) / (float)total_bytes);

				free(beebdata);
				beebdata = NULL;
			}
		}
	}

	return 0;
}
