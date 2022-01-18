/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "austinp.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//AustinP character structure
enum
{
	AustinP_ArcMain_Idle0,
	AustinP_ArcMain_Idle1,
	AustinP_ArcMain_Idle2,
	AustinP_ArcMain_Idle3,
	AustinP_ArcMain_Hit0,
	AustinP_ArcMain_Left0,
	AustinP_ArcMain_Down0,
	AustinP_ArcMain_Down1,
	AustinP_ArcMain_Down2,
	AustinP_ArcMain_Up,
	AustinP_ArcMain_Right0,
	AustinP_ArcMain_Right1,
	AustinP_ArcMain_Right2,
	
	AustinP_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[AustinP_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_AustinP;

//AustinP character definitions
static const CharFrame char_austinp_frame[] = {
	{AustinP_ArcMain_Idle0,  {  0,   0,141, 216}, { 56, 240}}, //0 idle 1
	{AustinP_ArcMain_Idle1,  {  0,   0,140, 218}, { 54, 242}}, //1 idle 2
	{AustinP_ArcMain_Idle2,  {  0,   0,137, 218}, { 52, 243}}, //2 idle 3
	{AustinP_ArcMain_Idle3,  {  0,   0,136, 219}, { 51, 243}}, //3 idle 4

	{AustinP_ArcMain_Left0,  {  0,   0,113, 223}, {-31, 247}}, //4 Left
	{AustinP_ArcMain_Left0,  {113,   0,119, 223}, {-31, 247}}, //5 Left
	{AustinP_ArcMain_Hit0,   {  0,   0,124, 223}, {-32, 247}}, //6
 
	{AustinP_ArcMain_Down0,  {  0,   0,151, 138}, { 67, 162}}, //7
	{AustinP_ArcMain_Down1,  {  0,   0,153, 138}, { 69, 162}}, //8
	{AustinP_ArcMain_Down2,  {  0,   0,155, 138}, { 71, 162}}, //9
 
	{AustinP_ArcMain_Hit0,   {124,   0,123, 229}, { -5, 253}}, //10
	{AustinP_ArcMain_Up,     {  0,   0,122, 229}, { -6, 254}}, //11
	{AustinP_ArcMain_Up,     {122,   0,121, 230}, { -6, 254}}, //12

	{AustinP_ArcMain_Right0, {  0,   0,201, 177}, {117, 201}}, //13
	{AustinP_ArcMain_Right1, {  0,   0,203, 176}, {119, 200}}, //14
	{AustinP_ArcMain_Right2, {  0,   0,204, 175}, {120, 199}}, //15
	
};

static const Animation char_austinp_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1,  2,  3,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){4, 5, 6, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//AustinP character functions
void Char_AustinP_SetFrame(void *user, u8 frame)
{
	Char_AustinP *this = (Char_AustinP*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_austinp_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_AustinP_Tick(Character *character)
{
	Char_AustinP *this = (Char_AustinP*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_AustinP_SetFrame);
	Character_Draw(character, &this->tex, &char_austinp_frame[this->frame]);
}

void Char_AustinP_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_AustinP_Free(Character *character)
{
	Char_AustinP *this = (Char_AustinP*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_AustinP_New(fixed_t x, fixed_t y)
{
	//Allocate austinp object
	Char_AustinP *this = Mem_Alloc(sizeof(Char_AustinP));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_AustinP_New] Failed to allocate austinp object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_AustinP_Tick;
	this->character.set_anim = Char_AustinP_SetAnim;
	this->character.free = Char_AustinP_Free;
	
	Animatable_Init(&this->character.animatable, char_austinp_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(10,1);
	this->character.focus_y = FIXED_DEC(-145,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\AUSTINP.ARC;1");
	
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
