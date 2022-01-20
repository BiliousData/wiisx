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
	AustinP_ArcMain_MissL,
	AustinP_ArcMain_MissD,
	AustinP_ArcMain_Miss,

	AustinP_ArcMain_Die0,
	AustinP_ArcMain_Die1,
	AustinP_ArcMain_Die2,
	AustinP_ArcMain_Die3,
	AustinP_ArcMain_Die4,
	AustinP_ArcMain_Die5,
	AustinP_ArcMain_Die6,
	AustinP_ArcMain_Die7,
	AustinP_ArcMain_Die8,
	
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

	{AustinP_ArcMain_MissL, {  0,   0,213, 180}, {123, 205}}, //16
	{AustinP_ArcMain_MissD, {  0,   0,157, 149}, { 71, 173}}, //17
	{AustinP_ArcMain_Miss,  {  0,   0,122, 229}, { -7, 257}}, //18
	{AustinP_ArcMain_Miss,  {123,   0,120, 229}, {-38, 256}}, //19

	{AustinP_ArcMain_Die0,   {  0,  0,161,204}, { 65, 229}}, //20
	{AustinP_ArcMain_Die1,   {  0,  0,162,205}, { 61, 229}}, //21
	{AustinP_ArcMain_Die2,   {  0,  0,172,205}, { 61, 229}}, //22
	{AustinP_ArcMain_Die3,   {  0,  0,180,205}, { 61, 229}}, //23
	{AustinP_ArcMain_Die4,   {  0,  0,180,205}, { 60, 229}}, //24
	{AustinP_ArcMain_Die5,   {  0,  0,181,205}, { 61, 229}}, //25
	{AustinP_ArcMain_Die6,   {  0,  0,161,205}, { 62, 229}}, //26
	{AustinP_ArcMain_Die7,   {  0,  0,160,205}, { 61, 229}}, //27
	{AustinP_ArcMain_Die8,   {  0,  0,160,205}, { 61, 229}}, //28

	
	
};

static const Animation char_austinp_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0, 1,  2,  3,  ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 13, 14, 15, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, 9, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 10, 11, 12, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){4, 5, 6, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt

	{1, (const u8[]){ 13, 14, 16, ASCR_BACK, 1}},      //PlayerAnim_LeftMiss
	{1, (const u8[]){ 7, 8, 17, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){10, 11, 18, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){4, 5, 19, ASCR_BACK, 1}},     //PlayerAnim_RightMiss

	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},         //PlayerAnim_Peace
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      //PlayerAnim_Sweat

	{2, (const u8[]){20, 21, 22, 23, 24, 25, 26, 27, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{2, (const u8[]){27, ASCR_CHGANI, PlayerAnim_Dead2}},
	{20, (const u8[]){27, 27, 27, ASCR_CHGANI, PlayerAnim_Dead3}},
	{20, (const u8[]){28,  ASCR_BACK, 1}},
	{0, (const u8[]){ ASCR_CHGANI, PlayerAnim_Dead3}}, //CharAnim_Idle
	{0, (const u8[]){ ASCR_CHGANI, PlayerAnim_Dead3}}, //CharAnim_Idle
	{0, (const u8[]){ ASCR_CHGANI, PlayerAnim_Dead3}}, //CharAnim_Idle
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
	this->character.spec = CHAR_SPEC_MISSANIM;
	
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
		"missl.tim",
		"missd.tim",
		"miss.tim",
		"die0.tim",
		"die1.tim",
		"die2.tim",
		"die3.tim",
		"die4.tim",
		"die5.tim",
		"die6.tim",
		"die7.tim",
		"die8.tim",

		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
