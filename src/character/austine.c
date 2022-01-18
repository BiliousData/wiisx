/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "austine.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//AustinE character structure
enum
{
	AustinE_ArcMain_Idle0,
	AustinE_ArcMain_Idle1,
	AustinE_ArcMain_Idle2,
	AustinE_ArcMain_Idle3,
	AustinE_ArcMain_Hit0,
	AustinE_ArcMain_Left0,
	AustinE_ArcMain_Down0,
	AustinE_ArcMain_Down1,
	AustinE_ArcMain_Down2,
	AustinE_ArcMain_Up,
	AustinE_ArcMain_Right0,
	AustinE_ArcMain_Right1,
	AustinE_ArcMain_Right2,
	
	AustinE_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[AustinE_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_AustinE;

//AustinE character definitions
static const CharFrame char_austine_frame[] = {
	{AustinE_ArcMain_Idle0,  {  0,   0,141, 216}, { 56, 240}}, //0 idle 1
	{AustinE_ArcMain_Idle1,  {  0,   0,140, 218}, { 57, 242}}, //1 idle 2
	{AustinE_ArcMain_Idle2,  {  0,   0,137, 218}, { 56, 243}}, //2 idle 3
	{AustinE_ArcMain_Idle3,  {  0,   0,136, 219}, { 57, 243}}, //3 idle 4
 
	{AustinE_ArcMain_Left0,  {  0,   0,113, 223}, {115, 247}}, //4 Left
	{AustinE_ArcMain_Left0,  {113,   0,119, 223}, {121, 247}}, //5 Left
	{AustinE_ArcMain_Hit0,   {  0,   0,124, 223}, {127, 247}}, //6
 
	{AustinE_ArcMain_Down0,  {  0,   0,151, 138}, { 54, 162}}, //7
	{AustinE_ArcMain_Down1,  {  0,   0,153, 138}, { 55, 162}}, //8
	{AustinE_ArcMain_Down2,  {  0,   0,155, 138}, { 55, 162}}, //9
 
	{AustinE_ArcMain_Hit0,   {124,   0,123, 229}, {100, 253}}, //10
	{AustinE_ArcMain_Up,     {  0,   0,122, 229}, {100, 254}}, //11
	{AustinE_ArcMain_Up,     {122,   0,121, 230}, { 99, 254}}, //12

	{AustinE_ArcMain_Right0, {  0,   0,201, 177}, { 56, 201}}, //13
	{AustinE_ArcMain_Right1, {  0,   0,203, 176}, { 56, 200}}, //14
	{AustinE_ArcMain_Right2, {  0,   0,204, 175}, { 56, 199}}, //15
	
};

static const Animation char_austine_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1,  2,  3,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//AustinE character functions
void Char_AustinE_SetFrame(void *user, u8 frame)
{
	Char_AustinE *this = (Char_AustinE*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_austine_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_AustinE_Tick(Character *character)
{
	Char_AustinE *this = (Char_AustinE*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_AustinE_SetFrame);
	Character_Draw(character, &this->tex, &char_austine_frame[this->frame]);
}

void Char_AustinE_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_AustinE_Free(Character *character)
{
	Char_AustinE *this = (Char_AustinE*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_AustinE_New(fixed_t x, fixed_t y)
{
	//Allocate austine object
	Char_AustinE *this = Mem_Alloc(sizeof(Char_AustinE));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_AustinE_New] Failed to allocate austine object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_AustinE_Tick;
	this->character.set_anim = Char_AustinE_SetAnim;
	this->character.free = Char_AustinE_Free;
	
	Animatable_Init(&this->character.animatable, char_austine_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-145,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\AUSTINE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
        "idle1.tim",
        "idle2.tim",
        "idle3.tim",
        "hit0.tim",
        "left0.tim",
        "down0.tim",
        "down1.tim",
        "down2.tim",
        "up.tim",
        "right0.tim",
        "right1.tim",
        "right2.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
