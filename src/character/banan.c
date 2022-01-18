/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "banan.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Banan character structure
enum
{
	Banan_ArcMain_Idle,
	
	Banan_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Banan_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Banan;

//Banan character definitions
static const CharFrame char_banan_frame[] = {
	{Banan_ArcMain_Idle, {  0,   0, 206, 256}, { 12, 183}}, //0 idle 1
};

static const Animation char_banan_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Banan character functions
void Char_Banan_SetFrame(void *user, u8 frame)
{
	Char_Banan *this = (Char_Banan*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_banan_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Banan_Tick(Character *character)
{
	Char_Banan *this = (Char_Banan*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Banan_SetFrame);
	Character_Draw(character, &this->tex, &char_banan_frame[this->frame]);
}

void Char_Banan_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Banan_Free(Character *character)
{
	Char_Banan *this = (Char_Banan*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Banan_New(fixed_t x, fixed_t y)
{
	//Allocate banan object
	Char_Banan *this = Mem_Alloc(sizeof(Char_Banan));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Banan_New] Failed to allocate banan object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Banan_Tick;
	this->character.set_anim = Char_Banan_SetAnim;
	this->character.free = Char_Banan_Free;
	
	Animatable_Init(&this->character.animatable, char_banan_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BANAN.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
