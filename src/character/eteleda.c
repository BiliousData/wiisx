/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "eteleda.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//EteledA character structure
enum
{
	EteledA_ArcMain_Idle0,
	EteledA_ArcMain_Hit0,
	EteledA_ArcMain_Hit1,
	EteledA_ArcMain_Hit2,
	EteledA_ArcMain_Hit3,
	
	EteledA_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[EteledA_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_EteledA;

//EteledA character definitions
static const CharFrame char_eteleda_frame[] = {
	{EteledA_ArcMain_Idle0, {  0,   0, 69, 159}, { 42, 183}}, //0 idle 1
	{EteledA_ArcMain_Idle0, { 69,   0, 70, 160}, { 42, 183}}, //1 idle 2
	{EteledA_ArcMain_Idle0, {139,   0, 70, 160}, { 42, 184}}, //2 idle 3
	{EteledA_ArcMain_Hit0,  {  0,   0, 70, 161}, { 42, 185}}, //3 idle 4
	
	{EteledA_ArcMain_Hit0, { 70,   0,  91, 162}, { 57, 186}}, //4 left 1
	{EteledA_ArcMain_Hit0, {161,   0,  91, 162}, { 58, 187}}, //5 left 2
	
	{EteledA_ArcMain_Hit1, {  0,   0,  80, 149}, { 42, 173}}, //6 down 1
	{EteledA_ArcMain_Hit1, { 80,   0,  80, 147}, { 42, 171}}, //7 down 2
	
	{EteledA_ArcMain_Hit1, {160,   0,  81, 161}, { 43, 186}}, //8 up 1
	{EteledA_ArcMain_Hit2, {  0,   0,  81, 162}, { 43, 187}}, //9 up 2
	
	{EteledA_ArcMain_Hit2, { 81,   0, 102, 152}, { 41, 178}}, //10 right 1
	{EteledA_ArcMain_Hit3, {  0,   0, 104, 153}, { 41, 177}}, //11 right 2
};

static const Animation char_eteleda_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1,  2,  3,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//EteledA character functions
void Char_EteledA_SetFrame(void *user, u8 frame)
{
	Char_EteledA *this = (Char_EteledA*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_eteleda_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_EteledA_Tick(Character *character)
{
	Char_EteledA *this = (Char_EteledA*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_EteledA_SetFrame);
	Character_Draw(character, &this->tex, &char_eteleda_frame[this->frame]);
}

void Char_EteledA_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_EteledA_Free(Character *character)
{
	Char_EteledA *this = (Char_EteledA*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_EteledA_New(fixed_t x, fixed_t y)
{
	//Allocate eteleda object
	Char_EteledA *this = Mem_Alloc(sizeof(Char_EteledA));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_EteledA_New] Failed to allocate eteleda object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_EteledA_Tick;
	this->character.set_anim = Char_EteledA_SetAnim;
	this->character.free = Char_EteledA_Free;
	
	Animatable_Init(&this->character.animatable, char_eteleda_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\ETELEDA.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"hit0.tim", 
		"hit1.tim", 
		"hit2.tim", 
		"hit3.tim", 
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
