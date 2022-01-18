/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "eteledc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//EteledC character structure
enum
{
	EteledC_ArcMain_Cut0,
	EteledC_ArcMain_Cut1,
	EteledC_ArcMain_Cut2,
	EteledC_ArcMain_Turn0,
	EteledC_ArcMain_Turn1,
	EteledC_ArcMain_Turn2,
	EteledC_ArcMain_Turn3,
	
	EteledC_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[EteledC_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_EteledC;

//EteledC character definitions
static const CharFrame char_eteledc_frame[] = {
	{EteledC_ArcMain_Cut0, {  0,   0, 67, 157}, { 42, 183}}, //0 idle

	{EteledC_ArcMain_Cut0, { 67,   0, 66, 157}, { 41, 183}}, //1 "Gottem." 1
	{EteledC_ArcMain_Cut0, {133,   0, 66, 157}, { 42, 183}}, //2 "Gottem." 2
	{EteledC_ArcMain_Cut1, {  0,   0, 66, 156}, { 42, 183}}, //3 "Gottem." 3
	{EteledC_ArcMain_Cut1, { 66,   0, 67, 156}, { 42, 182}}, //4 "Gottem." 4

	{EteledC_ArcMain_Cut1, {133,   0, 66, 157}, { 42, 184}}, //5 Spooked 1
	{EteledC_ArcMain_Cut2, {  0,   0, 66, 158}, { 42, 184}}, //6 Spooked 2
	{EteledC_ArcMain_Cut2, { 66,   0, 66, 158}, { 42, 185}}, //7 Spooked 3
	
	{EteledC_ArcMain_Turn0, {  0,   0, 66, 156}, { 43, 183}}, //8  Turn around
	{EteledC_ArcMain_Turn0, { 66,   0, 68, 157}, { 44, 184}}, //9  Turn around
	{EteledC_ArcMain_Turn0, {134,   0, 73, 156}, { 48, 183}}, //10 Turn around
	{EteledC_ArcMain_Turn1, {  0,   0, 89, 145}, { 60, 173}}, //11 Turn around
	{EteledC_ArcMain_Turn1, { 89,   0, 83, 147}, { 55, 175}}, //12 Turn around
	{EteledC_ArcMain_Turn1, {172,   0, 73, 147}, { 49, 175}}, //13 Turn around
	{EteledC_ArcMain_Turn2, {  0,   0, 73, 147}, { 49, 176}}, //14 Turn around
	{EteledC_ArcMain_Turn2, { 73,   0, 69, 147}, { 49, 176}}, //15 Turn around
	{EteledC_ArcMain_Turn2, {142,   0, 76, 148}, { 45, 177}}, //16 Turn around
	{EteledC_ArcMain_Turn3, {  0,   0, 74, 147}, { 45, 178}}, //17 Turn around
	{EteledC_ArcMain_Turn3, { 74,   0, 72, 148}, { 44, 179}}, //18 Turn around
};

static const Animation char_eteledc_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}}, //CharAnim_Idle
	{1, (const u8[]){8, 9, 9, 9, 10, 11, 12, 12, 12, 12, 13, 13, 14, 15, 16, 17, 18, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 1, 2, 3, 4, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 5, 6, 7, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//EteledC character functions
void Char_EteledC_SetFrame(void *user, u8 frame)
{
	Char_EteledC *this = (Char_EteledC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_eteledc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_EteledC_Tick(Character *character)
{
	Char_EteledC *this = (Char_EteledC*)character;
	
	//Perform idle dance
	if (stage.song_step <= 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_EteledC_SetFrame);
	Character_Draw(character, &this->tex, &char_eteledc_frame[this->frame]);

	if (stage.flag & STAGE_FLAG_JUST_STEP)
    {   //Stage specific animations
		if (stage.note_scroll >= 0)
		{
			switch (stage.stage_id)
			{
				case StageId_1_7: //Intro animations
				if ((stage.song_step) == 14) //"Gottem."
					    character->set_anim(character, CharAnim_Down);
				if ((stage.song_step) == 45) //Spooked
				        character->set_anim(character, CharAnim_Up);
				if ((stage.song_step) == 64) //Turn 1
				        character->set_anim(character, CharAnim_Left);
				default:
					break;
			}
		}
	}
}

void Char_EteledC_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_EteledC_Free(Character *character)
{
	Char_EteledC *this = (Char_EteledC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_EteledC_New(fixed_t x, fixed_t y)
{
	//Allocate eteledc object
	Char_EteledC *this = Mem_Alloc(sizeof(Char_EteledC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_EteledC_New] Failed to allocate eteledc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_EteledC_Tick;
	this->character.set_anim = Char_EteledC_SetAnim;
	this->character.free = Char_EteledC_Free;
	
	Animatable_Init(&this->character.animatable, char_eteledc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\INTRO.ARC;1");
	
	const char **pathp = (const char *[]){
		"cut0.tim",
        "cut1.tim",
        "cut2.tim",
        "turn0.tim",
        "turn1.tim",
        "turn2.tim",
        "turn3.tim", 
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
