/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "eteledb.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//EteledB character structure
enum
{
	EteledB_ArcMain_Idle0,
	EteledB_ArcMain_Idle1,
	EteledB_ArcMain_Idle2,
	EteledB_ArcMain_Idle3,
	EteledB_ArcMain_Left0,
	EteledB_ArcMain_Left1,
	EteledB_ArcMain_Down,
	EteledB_ArcMain_Up0,
	EteledB_ArcMain_Up1,
	EteledB_ArcMain_Right,

	EteledB_ArcSpook_Idleb0,
	EteledB_ArcSpook_Idleb1,
	EteledB_ArcSpook_Idleb2,
	EteledB_ArcSpook_Idleb3,
	EteledB_ArcSpook_Leftb0,
	EteledB_ArcSpook_Leftb1,
	EteledB_ArcSpook_Downb,
	EteledB_ArcSpook_Upb0,
	EteledB_ArcSpook_Upb1,
	EteledB_ArcSpook_Rightb,
	
	EteledB_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_spook;
	IO_Data arc_ptr[EteledB_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_EteledB;

//EteledB character definitions
static const CharFrame char_eteledb_frame[] = {
	{EteledB_ArcMain_Idle0, {  0,   0, 159, 142}, { 42, 183}}, //0 idle 1
	{EteledB_ArcMain_Idle1, {  0,   0, 157, 144}, { 42, 185}}, //1 idle 2
	{EteledB_ArcMain_Idle2, {  0,   0, 155, 145}, { 41, 186}}, //2 idle 3
	{EteledB_ArcMain_Idle3, {  0,   0, 155, 145}, { 41, 186}}, //3 idle 4
	 
	{EteledB_ArcMain_Left0, {  0,   0, 137, 145}, { 94, 186}}, //4 left 1
	{EteledB_ArcMain_Left1, {  0,   0, 140, 145}, { 99, 186}}, //5 left 2
	 
	{EteledB_ArcMain_Down,  {  0,   0, 139, 119}, { 42, 158}}, //6 down 1
	{EteledB_ArcMain_Down,  {  0, 119, 133, 119}, { 42, 159}}, //7 down 2
	 
	{EteledB_ArcMain_Up0,   {  0,   0, 138, 161}, { 47, 202}}, //8 up 1
	{EteledB_ArcMain_Up1,   {  0,   0, 136, 162}, { 46, 203}}, //9 up 2
	 
	{EteledB_ArcMain_Right, {  0,   0, 128, 123}, { 37, 164}}, //10 right 1
	{EteledB_ArcMain_Right, {  0, 123, 130, 123}, { 37, 164}}, //11 right 2

	{EteledB_ArcSpook_Idleb0, {  0,   0, 159, 142}, { 42, 183}}, //12 idle 1
	{EteledB_ArcSpook_Idleb1, {  0,   0, 157, 144}, { 42, 185}}, //13 idle 2
	{EteledB_ArcSpook_Idleb2, {  0,   0, 155, 145}, { 41, 186}}, //14 idle 3
	{EteledB_ArcSpook_Idleb3, {  0,   0, 155, 145}, { 41, 186}}, //15 idle 4
	 
	{EteledB_ArcSpook_Leftb0, {  0,   0, 137, 145}, { 94, 186}}, //16 left 1
	{EteledB_ArcSpook_Leftb1, {  0,   0, 140, 145}, { 99, 186}}, //17 left 2
	 
	{EteledB_ArcSpook_Downb,  {  0,   0, 139, 119}, { 42, 158}}, //18 down 1
	{EteledB_ArcSpook_Downb,  {  0, 119, 133, 119}, { 42, 159}}, //19 down 2
	 
	{EteledB_ArcSpook_Upb0,   {  0,   0, 138, 161}, { 47, 202}}, //20 up 1
	{EteledB_ArcSpook_Upb1,   {  0,   0, 136, 162}, { 46, 203}}, //21 up 2
	 
	{EteledB_ArcSpook_Rightb, {  0,   0, 128, 123}, { 37, 164}}, //22 right 1
	{EteledB_ArcSpook_Rightb, {  0, 123, 130, 123}, { 37, 164}}, //23 right 2
};

static const Animation char_eteledb_anim[CharAnim_Max] = {
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

static const Animation char_eteledb_anim2[CharAnim_Max] = {
	{2, (const u8[]){ 12, 13,  14,  15,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 16,  17, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 18,  19, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 20,  21, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){22, 23, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//EteledB character functions
void Char_EteledB_SetFrame(void *user, u8 frame)
{
	Char_EteledB *this = (Char_EteledB*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_eteledb_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_EteledB_Tick(Character *character)
{
	Char_EteledB *this = (Char_EteledB*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	if (stage.spook == 0)
	{
	Animatable_Animate(&character->animatable, (void*)this, Char_EteledB_SetFrame);
	Character_Draw(character, &this->tex, &char_eteledb_frame[this->frame]);
	}

	else if (stage.spook == 1)
	{
		Animatable_Animate(&character->animatable2, (void*)this, Char_EteledB_SetFrame);
	    Character_Draw(character, &this->tex, &char_eteledb_frame[this->frame]);
	}
}

void Char_EteledB_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Animatable_SetAnim(&character->animatable2, anim);
	Character_CheckStartSing(character);
}

void Char_EteledB_Free(Character *character)
{
	Char_EteledB *this = (Char_EteledB*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_spook);
}

Character *Char_EteledB_New(fixed_t x, fixed_t y)
{
	//Allocate eteledb object
	Char_EteledB *this = Mem_Alloc(sizeof(Char_EteledB));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_EteledB_New] Failed to allocate eteledb object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_EteledB_Tick;
	this->character.set_anim = Char_EteledB_SetAnim;
	this->character.free = Char_EteledB_Free;
	
	Animatable_Init(&this->character.animatable, char_eteledb_anim);
	Animatable_Init(&this->character.animatable2, char_eteledb_anim2);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\ETELEDB.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
        "idle1.tim",
        "idle2.tim",
        "idle3.tim",
        "left0.tim",
        "left1.tim",
        "down.tim",
        "up0.tim",
        "up1.tim",
        "right.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Load scene art
	switch (stage.stage_id)
	{
		case StageId_1_2: //Diagraphephobia
		{
			//Load "Spook" art
			this->arc_spook = IO_Read("\\CHAR\\SPOOK.ARC;1");
			
			const char **pathp = (const char *[]){
				"idleb0.tim",
                "idleb1.tim",
                "idleb2.tim",
                "idleb3.tim",
                "leftb0.tim",
                "leftb1.tim",
                "downb.tim",
                "upb0.tim",
                "upb1.tim",
                "rightb.tim",
				NULL
			};
			IO_Data *arc_ptr = &this->arc_ptr[EteledB_ArcSpook_Idleb0];
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_spook, *pathp);
			break;
		}
		default:
		{
			this->arc_spook = NULL;
			break;
		}
	}
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
