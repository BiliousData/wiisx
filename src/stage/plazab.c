/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "plazab.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Week 4 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //PlazaB floor left
	Gfx_Tex tex_back1; //PlazaB floor right

	Gfx_Tex tex_glitch0; //Hall left
	Gfx_Tex tex_glitch1; //Hall right

	Gfx_Tex tex_noise0; //Static left
	Gfx_Tex tex_noise1; //Static right
	
} Back_PlazaB;

void Back_PlazaB_DrawNoise(StageBack *back)
{
	Back_PlazaB *this = (Back_PlazaB*)back;

	fixed_t fx, fy;

	if (stage.noise == 1)
	{
	RECT noisel_src = {0, 0, 256, 256};
	RECT_FIXED noisel_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};

	RECT noiser_src = {0, 0, 256, 256};
	RECT_FIXED noiser_dst = {
		FIXED_DEC(173 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};

	Stage_DrawTex(&this->tex_noise0, &noisel_src, &noisel_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_noise1, &noiser_src, &noiser_dst, stage.camera.bzoom);
	}

}





void Back_PlazaB_DrawFG(StageBack *back)
{
	Back_PlazaB *this = (Back_PlazaB*)back;
	
	fixed_t fx, fy;
	
	
}

void Back_PlazaB_DrawBG(StageBack *back)
{
	Back_PlazaB *this = (Back_PlazaB*)back;
	

	
	fixed_t fx, fy;
	
	
	
	//Draw sunset
	fx = stage.camera.x;
	fy = stage.camera.y;

	if (stage.song_step >= 447 && stage.song_step <= 575)
	{
	RECT glitl_src = {0, 0, 256, 256};
	RECT_FIXED glitl_dst = {
		FIXED_DEC(-135 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};

	RECT glitr_src = {0, 0, 256, 256};
	RECT_FIXED glitr_dst = {
		FIXED_DEC(143 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};


    Stage_DrawTex(&this->tex_glitch0, &glitl_src, &glitl_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_glitch1, &glitr_src, &glitr_dst, stage.camera.bzoom);
	}
	
	RECT plazabl_src = {0, 0, 256, 256};
	RECT_FIXED plazabl_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};

	RECT plazabr_src = {0, 0, 256, 256};
	RECT_FIXED plazabr_dst = {
		FIXED_DEC(173 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};

	Stage_DrawTex(&this->tex_back0, &plazabl_src, &plazabl_dst, stage.camera.bzoom);
	Stage_DrawTex(&this->tex_back1, &plazabr_src, &plazabr_dst, stage.camera.bzoom);

	




}


void Back_PlazaB_Free(StageBack *back)
{
	Back_PlazaB *this = (Back_PlazaB*)back;
	
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_PlazaB_New(void)
{
	//Allocate background structure
	Back_PlazaB *this = (Back_PlazaB*)Mem_Alloc(sizeof(Back_PlazaB));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_overlay = Back_PlazaB_DrawNoise;
	this->back.draw_fg = Back_PlazaB_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_PlazaB_DrawBG;
	this->back.free = Back_PlazaB_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\MIICHANNEL\\PLAZAB.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_glitch0, Archive_Find(arc_back, "glitch0.tim"), 0);
	Gfx_LoadTex(&this->tex_glitch1, Archive_Find(arc_back, "glitch1.tim"), 0);
	Gfx_LoadTex(&this->tex_noise0, Archive_Find(arc_back, "noise0.tim"), 0);
	Gfx_LoadTex(&this->tex_noise1, Archive_Find(arc_back, "noise1.tim"), 0);

	Mem_Free(arc_back);
	
	
	
	
	return (StageBack*)this;
}
