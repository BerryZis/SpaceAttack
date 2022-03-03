#include "windows.h"
#include "toolbox.h"
#include <cvirte.h>		
#include <userint.h>
#include "SpaceAttack.h"
#include "bass.h"
HSTREAM Shoot_SND;   
HSTREAM Music_SND;
HSTREAM Bonus_SND;
HSTREAM Win_SND;
HSTREAM Loose_SND;

static int panelHandle,panelHandleMain,panelHandleTop,panelHandleAbout;
double dt=0.015;
int tick_counter=0,bonus=0,randBonus;
int ene_num,ene_active=0,ene_create_flag=1,boss_active=0,meteor_active=0,bonus_active=0;
int hero_shoot,bmp_win,bmp_loose,sndflag = 1;
int hero_shoot_num=0,ene_shoot_num=0;
int height=600,width=600 ,bmp_hero[3]={0}, bmp_bg[9]={0}, bmp_enemy[10]={0},bmp_shoot[5]={0},bmp_boss[2]={0},bmp_bonus[3]={0},bmp_lvl,LEVEL;
char line[100],filename[50]="LeaderBoard.csv";
FILE *fp;

//Keyboard-------------------------------------------------------------------
	
				//variables for keyboard monitoring
				int Callback_Data;	
				int Posting_Handle; 
				int Keys_Down[256]={0}; 

				//Receives information from windows regarding key presses
				int CVICALLBACK Key_DownFunc(int panel, int message, unsigned int* wParam, unsigned int* lParam, void* callbackData)
				{
					unsigned int Key = *wParam;
					Keys_Down[Key] = 1;
					return 0;
				}

				//Receives information from windows regarding key releases
				int CVICALLBACK Key_UpFunc(int panel, int message, unsigned int* wParam, unsigned int* lParam, void* callbackData)
				{
					unsigned int Key = *wParam;
					Keys_Down[Key] = 0;
					return 0;
				}

				//asking windows to send keyboard press and release events to our software 
				//Specifing 'Key_DownFunc' and 'Key_UpFunc' as the functions to call
				void Connect_To_Windows_Events(void)
				{
					InstallWinMsgCallback(panelHandle,WM_KEYDOWN,Key_DownFunc,VAL_MODE_IN_QUEUE,&Callback_Data,&Posting_Handle);
					InstallWinMsgCallback(panelHandle,WM_KEYUP,Key_UpFunc,VAL_MODE_IN_QUEUE,&Callback_Data,&Posting_Handle);
				}
//Structs-----------------------------------------------------------------

typedef struct{
	char Name[21];
	int Life;
	int Ammo;
	int Level;
	int Exp;
}PLAYER;

PLAYER Player;
PLAYER Leaders[11];

typedef struct{
double x;
double y;
double vx;
double vy;
int hight;
int width;
int life;
int bmp;
int s_bmp;
}OBJECT;

OBJECT Hero;
OBJECT Hero_Fire[200];
OBJECT Enemy[100];
OBJECT Enemy_Fire[2000];
OBJECT Boss;
OBJECT Meteor[50];
OBJECT Bonus[3];

void LoadBoard()
{
	//read & load board from file
	fp=fopen(filename,"r");
	int i=1;
	while(fgets(line,99,fp) != NULL)
	{
		sscanf(line,"%d,%[^,],%d\n",&Leaders[i-1].Level,Leaders[i-1].Name,&Leaders[i-1].Exp);
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(1,i) ,Leaders[i-1].Level );
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(2,i) ,Leaders[i-1].Name );
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(3,i) ,Leaders[i-1].Exp );
		i++;
	}

   fclose(fp);
  
}

void BoardUpdate()
{
	int ind=0,db_name=0,db_ind=0,sort=0;
   //Update and Sort the leaders array-----------------------------------------------------
   
   //check for same name
   for(int i=0;i<10;i++)
   {
	   if(strcmp(Player.Name,Leaders[i].Name)==0)
	   {
		   db_name=1;
		   db_ind=i;
		   if(Player.Exp > Leaders[i].Exp)
		   {
			   sort=1;
			   break;
		   }
	   }
   }
   
  //Condition to sort leaderboard if same name played and got better score--------------------------- 
  if(sort)
  {
	  for(int i=9; i>db_ind; i--) //delete old
		Leaders[i-1] = Leaders[i];
	  for(int i=9;i>=0;i--)
   	{
	   if(Player.Exp < Leaders[i].Exp)	//find the index for the new player
	   {
		   ind = i+1;
		   break;
	   }
  	 }
  	 for(int i=9;i>=ind;i--)					//move the array
	   Leaders[i+1] = Leaders[i];
 	 Leaders[ind] = Player;				//insert new player
	 //update the leaderboard
	for(int i=1; i<11; i++)
	{
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(1,i) ,Leaders[i-1].Level );
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(2,i) ,Leaders[i-1].Name );
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(3,i) ,Leaders[i-1].Exp );
	}
	sort=0;  
  }
   
 //Condition to Update the leaderboard for new name-----------------------------------------------  
   if((Player.Exp > Leaders[9].Exp) && db_name==0) 
   {
   	for(int i=9;i>=0;i--)
   	{
	   if(Player.Exp < Leaders[i].Exp)	//find the index for the new player
	   {
		   ind = i+1;
		   break;
	   }
  	 }
  	 for(int i=9;i>=ind;i--)					//move the array
	   Leaders[i+1] = Leaders[i];
 	 Leaders[ind] = Player;				//insert new player
   
   //save new data
   fp=fopen(filename,"w");
	for(int i=0; i<10; i++)
	{
		 fprintf(fp,"%d,%s,%d\n",Leaders[i].Level,Leaders[i].Name,Leaders[i].Exp); 
	}
	fclose(fp);
	
	//update the leaderboard
	for(int i=1; i<11; i++)
	{
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(1,i) ,Leaders[i-1].Level );
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(2,i) ,Leaders[i-1].Name );
		SetTableCellVal (panelHandleTop, PANEL_3_TABLE,MakePoint(3,i) ,Leaders[i-1].Exp );
	}
   }
}

void initiate()
{
	//Sound-------------------------------------------------
	BASS_Init( -1,44100, 0,0,NULL);
	Shoot_SND = BASS_StreamCreateFile(FALSE,"Sound\\shoot.ogg",0,0,0);
	Music_SND = BASS_StreamCreateFile(FALSE,"Sound\\music.mp3",0,0,0);
	Bonus_SND = BASS_StreamCreateFile(FALSE,"Sound\\bonus.ogg",0,0,0);
	Win_SND = BASS_StreamCreateFile(FALSE,"Sound\\win.wav",0,0,0);
	Loose_SND = BASS_StreamCreateFile(FALSE,"Sound\\loose.ogg",0,0,0);
	BASS_ChannelSetAttribute(Shoot_SND, BASS_ATTRIB_VOL, 0.7);
	BASS_ChannelPlay(Music_SND,TRUE);
	
	srand(time(0));
	LEVEL = 1;
	
	LoadBoard();
	
	SetPanelAttribute (panelHandle, ATTR_HEIGHT, 680);
	SetPanelAttribute (panelHandle, ATTR_WIDTH, 600);
	
	SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_HEIGHT, height+80); //+80 for Hero Panel
	SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_WIDTH, width);
	
	Hero.width = 45; 
	Hero.hight = 41;
	Hero.x = width/2-Hero.width/2;
	Hero.y = height-41;
	Hero.vx = 270*dt;
	Hero.vy = 270*dt;
	
	Player.Ammo = 500;
	Player.Life = 100;
	Player.Exp = 10;
	

//Bitmaps Loading----------------------------------------------------------------
	//Backgrounds---------------------------------------------------------------------
	GetBitmapFromFile ("Pics\\bg_1.png", &bmp_bg[0]);
	GetBitmapFromFile ("Pics\\bg_2.png", &bmp_bg[1]);
	GetBitmapFromFile ("Pics\\bg_boss_1.png", &bmp_bg[2]);//boss
	GetBitmapFromFile ("Pics\\bg_3.png", &bmp_bg[3]);
	GetBitmapFromFile ("Pics\\bg_4.png", &bmp_bg[4]);
	GetBitmapFromFile ("Pics\\bg_boss_2.png", &bmp_bg[5]);//boss
	GetBitmapFromFile ("Pics\\Stars_bg.png", &bmp_bg[6]);
	GetBitmapFromFile ("Pics\\meteor_Brown.png", &bmp_bg[7]);
	GetBitmapFromFile ("Pics\\meteor_Grey.png", &bmp_bg[8]);
	GetBitmapFromFile ("Pics\\bg_win.png", &bmp_win);
	GetBitmapFromFile ("Pics\\bg_loose.png", &bmp_loose);
	GetBitmapFromFile ("Pics\\lvlup.png", &bmp_lvl);
	//Player---------------------------------------------------------------------
	GetBitmapFromFile ("Pics\\Player_1.png", &bmp_hero[0]);
	GetBitmapFromFile ("Pics\\Player_2.png", &bmp_hero[1]);
	//Bonus----------------------------------------------------------------------
	GetBitmapFromFile ("Pics\\Life_bonus.png", &bmp_bonus[0]);
	GetBitmapFromFile ("Pics\\Ammo_bonus.png", &bmp_bonus[1]);
	GetBitmapFromFile ("Pics\\Exp_bonus.png", &bmp_bonus[2]);
	//Enemies--------------------------------------------------------------------
	GetBitmapFromFile ("Pics\\enemyRed.png", &bmp_enemy[0]);
	GetBitmapFromFile ("Pics\\enemyGreen.png", &bmp_enemy[1]);
	GetBitmapFromFile ("Pics\\enemyBlue.png", &bmp_enemy[2]);
	GetBitmapFromFile ("Pics\\enemyBlack.png", &bmp_enemy[3]);
	GetBitmapFromFile ("Pics\\UFO_1.png", &bmp_enemy[4]);
	GetBitmapFromFile ("Pics\\UFO_2.png", &bmp_enemy[5]);
	GetBitmapFromFile ("Pics\\UFO_3.png", &bmp_enemy[6]);
	GetBitmapFromFile ("Pics\\UFO_4.png", &bmp_enemy[7]);
	GetBitmapFromFile ("Pics\\Missile.png", &bmp_enemy[8]);
	//Shoots---------------------------------------------------------------------
	GetBitmapFromFile("Pics\\shoot_1.png", &bmp_shoot[0]);
	GetBitmapFromFile("Pics\\Ene_shoot.png", &bmp_shoot[1]);
	GetBitmapFromFile("Pics\\Boss_1_shoot.png", &bmp_shoot[2]);
	GetBitmapFromFile("Pics\\Boss_2_shoot.png", &bmp_shoot[3]);
	GetBitmapFromFile("Pics\\UFO_shoot.png", &bmp_shoot[4]);
	//Bosses---------------------------------------------------------------------
	GetBitmapFromFile("Pics\\Boss_1.png", &bmp_boss[0]);
	GetBitmapFromFile("Pics\\Boss_2.png", &bmp_boss[1]);
}

void terminate()
{
	for(int i=0;i<9;i++){
		DiscardBitmap (bmp_bg[i]);
		DiscardBitmap (bmp_enemy[i]);
	}
	
	DiscardBitmap (bmp_win);
	DiscardBitmap (bmp_loose);
	DiscardBitmap (bmp_lvl);
	
	for(int i=0;i<2;i++){
		DiscardBitmap (bmp_hero[i]);
		DiscardBitmap (bmp_boss[i]);
	}
	
	for(int i=0;i<3;i++)
		DiscardBitmap (bmp_bonus[i]);
	for(int i=0;i<5;i++)
		DiscardBitmap (bmp_shoot[i]);
	
	DiscardPanel (panelHandleAbout);
	DiscardPanel (panelHandleTop);
	DiscardPanel (panelHandleMain);
	DiscardPanel (panelHandle);
	
	BASS_StreamFree(Shoot_SND);
	BASS_StreamFree(Music_SND);
	BASS_StreamFree(Loose_SND);
	BASS_StreamFree(Win_SND);
	BASS_StreamFree(Bonus_SND);

}

void reset()
{
	srand(time(0));
	LEVEL = 1;
	
	SetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON, 0);
	
	ene_active=0;
	boss_active=0;
	meteor_active=0;
	hero_shoot_num=0;
	ene_shoot_num=0;
	ene_create_flag=1;
	bonus_active=0;
	
	Hero.width = 45; 
	Hero.hight = 41;
	Hero.x = width/2-Hero.width/2;
	Hero.y = height-41;
	Hero.vx = 270*dt;
	Hero.vy = 270*dt;
	
	Player.Ammo = 500;
	Player.Life = 100;
	Player.Exp = 10;	
}

void Win()
{
	BASS_ChannelPlay(Win_SND,TRUE);
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_win, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
	SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
	BoardUpdate();
}

void Loose()
{
	BASS_ChannelPlay(Loose_SND,TRUE);
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_loose, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
	SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
	BoardUpdate();
}

double EnemyIntersec(OBJECT Enemy1,OBJECT Enemy2) //Checks intersec in enemy creation
{
	double x_new=Enemy2.x;
	int flag = RectIntersection(MakeRect(Enemy1.y,Enemy1.x,Enemy1.hight,Enemy1.width), MakeRect(Enemy2.y,Enemy2.x,Enemy2.hight,Enemy2.width), NULL);
	
	while(flag)
	{
		x_new=rand()%560;
		if(RectIntersection(MakeRect(Enemy1.y,Enemy1.x,Enemy1.hight,Enemy1.width), MakeRect(Enemy2.y,x_new,Enemy2.hight,Enemy2.width), NULL))
			flag=1;
		else
			flag=0;
	}
	return x_new;
}

void HeroMovement()
{
	if(Keys_Down[38] && Hero.y >= 0) //Up
		Hero.y = Hero.y - Hero.vy;	
	if(Keys_Down[40] && Hero.y < height-Hero.hight) //Down
		Hero.y = Hero.y + Hero.vy;
	if(Keys_Down[39] && Hero.x <= width-Hero.width) //Right
		Hero.x = Hero.x + Hero.vx;
	if(Keys_Down[37] && Hero.x >= 0) //Left
		Hero.x = Hero.x - Hero.vx;
	
	//Leveling ---------------------------------------------
	if(Hero.y <= 0 && LEVEL<6 && ene_active==0)
	{
		LEVEL++;  //new map new enemy
		Player.Level = LEVEL;
		Hero.y=height-Hero.hight;
		ene_create_flag=1;
		bonus=1;
	}
}

void Hit()
{
	//Hero Hit by Enemy --------------------------------------------------------------
	for(int i=0; i<ene_active; i++)
	{
		if(RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Enemy[i].y,Enemy[i].x,Enemy[i].hight,Enemy[i].width), NULL))
		{
			 Player.Life = Player.Life - 1;
		}
	}
	
	//Hero Hit by Enemy Fire----------------------------------------------------------
	for(int i=0; i<ene_shoot_num; i++)
	{
		if(RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Enemy_Fire[i].y,Enemy_Fire[i].x,Enemy_Fire[i].hight,Enemy_Fire[i].width), NULL))
		{
			Enemy_Fire[i] = Enemy_Fire[ene_shoot_num-1];
			ene_shoot_num--;
			if(boss_active!=0)
				 Player.Life = Player.Life - LEVEL - 2;
			else
				Player.Life = Player.Life - LEVEL;
		}
	}
	
	//Enemy Hit by Hero Fire----------------------------------------------------------
	for(int i=0; i<ene_active; i++){
		for(int j=0; j<hero_shoot_num; j++)
		{
			if(RectIntersection (MakeRect(Enemy[i].y,Enemy[i].x,Enemy[i].hight,Enemy[i].width),MakeRect(Hero_Fire[j].y,Hero_Fire[j].x,Hero_Fire[j].hight,Hero_Fire[j].width),  NULL))
			{
				if(Enemy[i].life > 0)
					Enemy[i].life--;
				else
				{
					if(ene_active>0)
						Enemy[i] = Enemy[ene_active-1];  //puts the hitted enemy last in the array
					ene_active--;					 //deletes/kill the last enemy in the array (the hitted one)
					Player.Exp = Player.Exp + 2;
				}
				Hero_Fire[j] = Hero_Fire[hero_shoot_num-1]; //deletes the shoot that hitted the enemy
				hero_shoot_num--;
			}
			
		}
	}
	//Enemy Pass the Hero and out of canvas--------------------------------------------
	for(int i=0; i<ene_active; i++)
	{
		if(Enemy[i].y >= height)
		{
			Enemy[i] = Enemy[ene_active-1];
			Player.Exp--;
			ene_active--;
			
		}
	}
	//Boss Hit by Hero-------------------------------------------------------------
	for(int i=0; i<boss_active; i++)
		for(int j=0; j<hero_shoot_num; j++)
		{
			if(RectIntersection (MakeRect(Boss.y,Boss.x,Boss.hight,Boss.width),MakeRect(Hero_Fire[j].y,Hero_Fire[j].x,Hero_Fire[j].hight,Hero_Fire[j].width),  NULL))
			{
				if(Boss.life > 0)
					Boss.life--;
				else
				{
					boss_active=0;						//deletes/kill the Boss
					Player.Exp = Player.Exp + LEVEL*5 ;
				}
				Hero_Fire[j] = Hero_Fire[hero_shoot_num-1]; //deletes the shoot that hitted the enemy
				hero_shoot_num--;
			}
		}
	//Hero Hit by Boss --------------------------------------------------------------
	for(int i=0; i<boss_active; i++)
	{
		if(RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Boss.y,Boss.x,Boss.hight,Boss.width), NULL))
		{
			 Player.Life = Player.Life - 2;
		}
	}
	//Meteor pass the canvas
	for(int i=0; i<meteor_active; i++)
	{
		if(Meteor[i].y >= height)
		{
			Meteor[i]=Meteor[meteor_active-1];
			meteor_active--;
		}
	}
	//Hero Hit by Meteor
	for(int i=0; i<meteor_active; i++)
	{
		if(RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Meteor[i].y,Meteor[i].x,Meteor[i].hight,Meteor[i].width), NULL))
		{
			 Player.Life = Player.Life - 1;
		}
	}
	//Hero Hit Bonus
	for(int i=0; i<bonus_active; i++)
	{
		if(randBonus == 0 && RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Bonus[0].y,Bonus[0].x,Bonus[0].hight,Bonus[0].width), NULL))
		{
			BASS_ChannelPlay(Bonus_SND,TRUE);
			if(Player.Life<=60) 
				Player.Life = Player.Life + 40;
			else
				Player.Life = 100;
			 bonus_active=0;
		}
		else if(randBonus == 1 && RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Bonus[1].y,Bonus[1].x,Bonus[1].hight,Bonus[1].width), NULL))
		{
			BASS_ChannelPlay(Bonus_SND,TRUE);
			if(Player.Ammo<=800) 
				Player.Ammo = Player.Ammo + 200;
			else
				Player.Ammo = 1000;
			 bonus_active=0;
		}
		else if(randBonus == 2 && RectIntersection (MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width), MakeRect(Bonus[2].y,Bonus[2].x,Bonus[2].hight,Bonus[2].width), NULL))
		{
			BASS_ChannelPlay(Bonus_SND,TRUE); 
			Player.Exp = Player.Exp + 15;
			bonus_active=0;
		}
		
	}
}

void EnemyCreation()
{
	int i,j=0;
	switch (LEVEL)
	{
		//Map 1 / Level 1------------------------------------------------------------------------------------------------------------
		case 1:
			meteor_active=6;
			for(i=0; i<meteor_active; i++)
			{
				Meteor[i].hight = 40;
				Meteor[i].width = 45;
				Meteor[i].x = (600-Meteor[i].width)*(double)rand()/RAND_MAX;
				Meteor[i].y = -100 - i*200;
				Meteor[i].vy = 48*dt;
				Meteor[i].bmp = 7+rand()%2;
			}
			ene_num = 4;
			for(i=0; i<ene_num ; i++)
			{
		 		Enemy[i].hight = 27;
				Enemy[i].width = 33;
				Enemy[i].x = 560*(double)rand()/RAND_MAX;					  
				Enemy[i].y = -100 - 100*(double)rand()/RAND_MAX;
				Enemy[i].vx = 0;
				Enemy[i].vy = 35*dt;											  //First Iteration
				Enemy[i].bmp = rand()%4;
				Enemy[i].life = LEVEL;
				Enemy[i].s_bmp = 1;
				if(i>0)
					Enemy[i].x = EnemyIntersec(Enemy[i-1],Enemy[i]);
			}
			for(i; i<2*ene_num ; i++)
			{
		 		Enemy[i].hight = 27;											  //Second Iteration
				Enemy[i].width = 33;
				Enemy[i].x = 560*(double)rand()/RAND_MAX;					 
				Enemy[i].y = -300 - 100*(double)rand()/RAND_MAX;
				Enemy[i].vx = 0;
				Enemy[i].vy = 35*dt;
				Enemy[i].bmp = rand()%4;
				Enemy[i].s_bmp = 1;
				Enemy[i].life = LEVEL;
				if(i>0)
					Enemy[i].x = EnemyIntersec(Enemy[i-1],Enemy[i]);
			}
			
			ene_active=ene_num*2; //enemy created
	  		ene_create_flag=0;
			break;
		
		//Map 2 / Level 2-----------------------------------------------------------------------------------------------------------
		case 2:
			meteor_active=10;
			for(i=0; i<meteor_active; i++)
			{
				Meteor[i].hight = 40;
				Meteor[i].width = 45;
				Meteor[i].x = (600-Meteor[i].width)*(double)rand()/RAND_MAX;
				Meteor[i].y = -100-i*200;
				Meteor[i].vy = 48*dt;
				Meteor[i].bmp = 7+rand()%2;
			}
			ene_num = 8;
			for(i=0; i<ene_num ; i++)
			{
		 
				Enemy[i].x = -10-i*50;					 
				Enemy[i].y = -10 - 20*(double)rand()/RAND_MAX;
				Enemy[i].vx = 70*dt;
				Enemy[i].vy = 38*dt;
				Enemy[i].hight = 27;
				Enemy[i].width = 33;
				Enemy[i].bmp = rand()%4;
				Enemy[i].life = LEVEL;
				Enemy[i].s_bmp = 1;
				
			}
	 		
				for(i; i<2*ene_num ; i++)
				{
		 
					Enemy[i].x = 650+j*100;					
					Enemy[i].y = -50 - 20*(double)rand()/RAND_MAX;
					Enemy[i].vx = -70*dt;
					Enemy[i].vy = 38*dt;
					Enemy[i].hight = 27;
					Enemy[i].width = 33;
					Enemy[i].bmp = rand()%4;
					Enemy[i].life = LEVEL;
					Enemy[i].s_bmp = 1;
					j++;
				}
				ene_active=ene_num*2; //enemy created
	  			ene_create_flag=0;
				break;
		//Boss 1-----------------------------------------------------------------------------------------------------------------
		case 3:
			meteor_active=15;
			for(i=0; i<meteor_active; i++)
			{
				Meteor[i].hight = 40;
				Meteor[i].width = 45;
				Meteor[i].x = (600-Meteor[i].width)*(double)rand()/RAND_MAX;
				Meteor[i].y = -100-i*200;
				Meteor[i].vy = 55*dt;
				Meteor[i].bmp = 7+rand()%2;
			}
			 ene_active=0;
			 Boss.hight = 108;
			 Boss.width = 126;
			 Boss.x = (600-Boss.width)*(double)rand()/RAND_MAX;
			 Boss.y = -100;
			 Boss.vx=45*dt;
			 Boss.vy= 35*dt;							  
			 Boss.bmp =0;
			 Boss.life = LEVEL*20;
			 Boss.s_bmp = 2;
			 boss_active=1;
			 ene_create_flag=0;
			break;
		//Map 4// Level 4------------------------------------------------------------------------------------------------------
		case 4: 
			meteor_active=15;
			for(i=0; i<meteor_active; i++)
			{
				Meteor[i].hight = 40;
				Meteor[i].width = 45;
				Meteor[i].x = (600-Meteor[i].width)*(double)rand()/RAND_MAX;
				Meteor[i].y = -100 - i*200;
				Meteor[i].vy = 45*dt;
				Meteor[i].bmp = 7+rand()%2;
			}
			ene_num = 8;
			for(i=0; i<ene_num ; i++)
			{
		 	  	Enemy[i].hight = 27;
				Enemy[i].width = 33;
				Enemy[i].x = 560*(double)rand()/RAND_MAX;					  
				Enemy[i].y = -200 - 300*(double)rand()/RAND_MAX; 
				Enemy[i].vx = 25*dt;
				Enemy[i].vy = 40*dt;											 
				Enemy[i].bmp = rand()%4;
				Enemy[i].s_bmp = 1;
				Enemy[i].life = LEVEL;
				if(i>0)
					Enemy[i].x = EnemyIntersec(Enemy[i-1],Enemy[i]);
				
			}
			 
			for(i; i<ene_num*2 ; i++)
			{
		 	   	Enemy[i].hight = 40;
				Enemy[i].width = 40;
				Enemy[i].x = 560*(double)rand()/RAND_MAX;					  
				Enemy[i].y = -600 - 500*(double)rand()/RAND_MAX; 
				Enemy[i].vx = 35*dt;
				Enemy[i].vy = 40*dt;											
				Enemy[i].bmp = 4 + rand()%4;
				Enemy[i].s_bmp = 4;								 
				Enemy[i].life = LEVEL;
				if(i>0)
					Enemy[i].x = EnemyIntersec(Enemy[i-1],Enemy[i]);
			}
			ene_active=ene_num*2; //enemy created
	  		ene_create_flag=0;
			break;
		//Map 5// Level 5-----------------------------------------------------------------------------------------------
		case 5:
			meteor_active=10;
			for(i=0; i<meteor_active; i++)
			{
				Meteor[i].hight = 40;
				Meteor[i].width = 45;
				Meteor[i].x = (600-Meteor[i].width)*(double)rand()/RAND_MAX;
				Meteor[i].y = -100 - i*200;
				Meteor[i].vy = 45*dt;
				Meteor[i].bmp = 7+rand()%2;
			}
			ene_num = 5;
			for(i=0; i<ene_num ; i++)
			{
		 	  	Enemy[i].hight = 40;
				Enemy[i].width = 40;
				Enemy[i].x = 560*(double)rand()/RAND_MAX;				  
				Enemy[i].y = -200 - 400*(double)rand()/RAND_MAX; 
				Enemy[i].vx = 40*dt;
				Enemy[i].vy = 50*dt;										
				Enemy[i].bmp = 4 + rand()%4;
				Enemy[i].s_bmp = 4;
				Enemy[i].life = LEVEL;
				if(i>0)
					Enemy[i].x = EnemyIntersec(Enemy[i-1],Enemy[i]);
			}
			 
			for(i; i<ene_num*2 ; i++)
			{
		 	   	Enemy[i].hight = 40;
				Enemy[i].width = 40;
				Enemy[i].x = 560*(double)rand()/RAND_MAX;					  
				Enemy[i].y = -600 - 500*(double)rand()/RAND_MAX; 
				Enemy[i].vx = 40*dt;
				Enemy[i].vy = 50*dt;											
				Enemy[i].bmp = 4 + rand()%4;
				Enemy[i].s_bmp = 4;
				Enemy[i].life = LEVEL;
				if(i>0)
					Enemy[i].x = EnemyIntersec(Enemy[i-1],Enemy[i]);

			}
			 
			for(i; i<ene_num*3 ; i++)
			{
		 	   	Enemy[i].hight = 27;
				Enemy[i].width = 33;
				Enemy[i].x = -10 - i*50;					  
				Enemy[i].y = -200 - 100*(double)rand()/RAND_MAX;
				Enemy[i].vx = 55*dt;
				Enemy[i].vy = 35*dt;											  
				Enemy[i].bmp = rand()%4;
				Enemy[i].s_bmp = 1;
				Enemy[i].life = LEVEL-2;
			}
			ene_active=ene_num*3;
	  		ene_create_flag=0;
			break;
		//Final Boss-------------------------------------------------------------------------------------------------									  
		case 6: 
			 meteor_active=7;
			for(i=0; i<meteor_active; i++)
			{
				Meteor[i].hight = 40;
				Meteor[i].width = 45;
				Meteor[i].x =(600-Meteor[i].width)*(double)rand()/RAND_MAX;
				Meteor[i].y = -100 - i*200;
				Meteor[i].vy = 45*dt;
				Meteor[i].bmp = 7+rand()%2;
			}
			 Boss.hight = 126;
			 Boss.width = 108;
			 Boss.x = (600-Boss.width)*(double)rand()/RAND_MAX;
			 Boss.y = -300;
			 Boss.vx = 30*dt;
			 Boss.vy = 20*dt;							 
			 Boss.bmp =1;
			 Boss.life = LEVEL*15;
			 Boss.s_bmp = 3;
			 boss_active=1;
			 
			 ene_num = 2;					  //Missiles
			for(i=0; i<ene_num ; i++)
			{
		 		Enemy[i].hight = 267;
				Enemy[i].width = 100;
				Enemy[i].x = 10 + 480*i;					  
				Enemy[i].y = -600 - 100*(double)rand()/RAND_MAX;
				Enemy[i].vx = 0;
				Enemy[i].vy = 25*dt;											 
				Enemy[i].bmp = 8;
				Enemy[i].life = LEVEL*3;
				
			}
			ene_active=ene_num;
			ene_create_flag=0;
			break;
	}
	
}

void EnemyDraw()
{
//Meteor-------------------------------------------------------
	for(int i=0; i<meteor_active; i++)
	{
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[Meteor[i].bmp], VAL_ENTIRE_OBJECT, MakeRect(Meteor[i].y,Meteor[i].x,Meteor[i].hight,Meteor[i].width));
		Meteor[i].y = Meteor[i].y + Meteor[i].vy; 
	}


//Normal Enemies---------------------------------------------

	for(int i=0; i<ene_active; i++)
		 {
			 CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_enemy[Enemy[i].bmp], VAL_ENTIRE_OBJECT, MakeRect(Enemy[i].y,Enemy[i].x,Enemy[i].hight,Enemy[i].width));
		 	 
			 if(Enemy[i].vx > 0 && Enemy[i].x >= width-Enemy[i].width) //Right Wall
			 	 Enemy[i].vx=-Enemy[i].vx;
			 if(Enemy[i].vx < 0 && Enemy[i].x <= 0) //Left Wall
				 Enemy[i].vx=-Enemy[i].vx;
		 
			 Enemy[i].x = Enemy[i].x + Enemy[i].vx;
			 
				 
			 if(Enemy[i].x + Enemy[i].width >=0)
			 	Enemy[i].y = Enemy[i].y + Enemy[i].vy;
		 }
//Boss ------------------------------------------------------
	if(boss_active)
	{
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_boss[Boss.bmp], VAL_ENTIRE_OBJECT, MakeRect(Boss.y,Boss.x,Boss.hight,Boss.width));
		if(Boss.x <= 0 || Boss.x+Boss.width >= width)//Walls
			 Boss.vx = -Boss.vx;
		 
			 Boss.x = Boss.x + Boss.vx;
			 Boss.y = Boss.y + Boss.vy;
		
	}
}

void Shoot()
{
	//Hero Fire Creation ---------------------------------------------------------------------------
	if(hero_shoot==1 && tick_counter%10==0)
	{
		Hero_Fire[hero_shoot_num].x = Hero.x + 20;
		Hero_Fire[hero_shoot_num].y = Hero.y - Hero.hight/2;
		Hero_Fire[hero_shoot_num].vy = 250*dt;
		Hero_Fire[hero_shoot_num].hight = 15;
		Hero_Fire[hero_shoot_num].width = 4;
		hero_shoot_num++;
		Player.Ammo--;
		if(sndflag)
			BASS_ChannelPlay(Shoot_SND,TRUE);
		hero_shoot=0;
	}
	//Hero Fire Draw -------------------------------------------------------------------------------
	for(int i=0; i<hero_shoot_num; i++)
	{
		if(Hero_Fire[i].y + Hero_Fire[i].hight < 0) //shoot out of canvas
		{
			hero_shoot_num--;
			Hero_Fire[i] = Hero_Fire[hero_shoot_num];  //Use shoots that passed the canvas again
		}
		
		Hero_Fire[i].y = Hero_Fire[i].y - Hero_Fire[i].vy; //Shoot Movement
		
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_shoot[0], VAL_ENTIRE_OBJECT,
						  MakeRect(Hero_Fire[i].y,Hero_Fire[i].x,Hero_Fire[i].hight,Hero_Fire[i].width));
	}
	
	//Enemy Fire Creation --------------------------------------------------------------------------------
	for(int i=0; i<ene_active; i++)
	{
		double delta_ene = Hero.x - Enemy[i].x;
		
		//Enemy-----------------------------------------------------------------------------------------------------
		if(delta_ene<=25 && delta_ene>= -25  && (Hero.y > Enemy[i].y + Enemy[i].hight) && tick_counter%(30-LEVEL)==0 && Enemy[i].y>0 && LEVEL!=6)
		{
			Enemy_Fire[ene_shoot_num].x = Enemy[i].x + Enemy[i].width/2;
			Enemy_Fire[ene_shoot_num].y = Enemy[i].y + Enemy[i].hight/2;
			Enemy_Fire[ene_shoot_num].vy = 4 + LEVEL; 					//the fire rate
			Enemy_Fire[ene_shoot_num].width = 5;
			Enemy_Fire[ene_shoot_num].hight = 19;
			ene_shoot_num++;
		}
		
	}
	//Enemy Fire Draw----------------------------------------------------------------------------------------------
	for(int i=0; i<ene_shoot_num; i++)
	{
		if(Enemy_Fire[i].y >= 600) //shoot out of canvas
		{
			ene_shoot_num--;
			Enemy_Fire[i] = Enemy_Fire[ene_shoot_num];  //Use shoots that passed the canvas again
		}
		
		Enemy_Fire[i].y = Enemy_Fire[i].y + Enemy_Fire[i].vy; //Shoot Movement
		
		if(boss_active){
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_shoot[Boss.s_bmp], VAL_ENTIRE_OBJECT,
						  MakeRect(Enemy_Fire[i].y,Enemy_Fire[i].x,Enemy_Fire[i].hight,Enemy_Fire[i].width));
		}
		else
		{
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_shoot[Enemy[i].s_bmp], VAL_ENTIRE_OBJECT,
						  MakeRect(Enemy_Fire[i].y,Enemy_Fire[i].x,Enemy_Fire[i].hight,Enemy_Fire[i].width));
		}
	}
	//Boss Fire Creation
	for(int i=0; i<boss_active; i++)
	{
		//Boss-----------------------------------------------------------------------------------------------------
		if(LEVEL==3 && tick_counter%(20-LEVEL)==0 && (Boss.y + Boss.hight/2)>0)
		{
			Enemy_Fire[ene_shoot_num].x = Boss.x + 58;
			Enemy_Fire[ene_shoot_num].y = Boss.y + 64;
			Enemy_Fire[ene_shoot_num].vy = 5; 					//the fire rate
			Enemy_Fire[ene_shoot_num].width = 8;
			Enemy_Fire[ene_shoot_num].hight = 23;
			ene_shoot_num++;	
		}
		if(LEVEL==6 && tick_counter%(30-LEVEL)==0 && (Boss.y + Boss.hight/2)>0)
		{
			Enemy_Fire[ene_shoot_num].x = Boss.x + 50;
			Enemy_Fire[ene_shoot_num].y = Boss.y + 64;
			Enemy_Fire[ene_shoot_num].vy = 5; 					//the fire rate
			Enemy_Fire[ene_shoot_num].width = 10;
			Enemy_Fire[ene_shoot_num].hight = 42;
			ene_shoot_num++;	
		}
	}
}

void BonusesCreation(int i)
{
	switch(i)
	{
		//Life Bonus---------------------------------------
		case 0:
			Bonus[0].bmp = bmp_bonus[0];
			Bonus[0].hight = 21; 
			Bonus[0].width = 22;
			Bonus[0].x = rand()%(width-Bonus[0].width);
			Bonus[0].y = rand()%(height-100);
		break;
		//Ammo Bonus---------------------------------------
		case 1:
			Bonus[1].bmp = bmp_bonus[1];
			Bonus[1].hight = 33; 
			Bonus[1].width = 34;
			Bonus[1].x = rand()%(width-Bonus[1].width);
			Bonus[1].y = 50 + rand()%(height-100);
		break;
		//Exp Bonus---------------------------------------
		case 2:
			Bonus[2].bmp = bmp_bonus[2];
			Bonus[2].hight = 33; 
			Bonus[2].width = 34;
			Bonus[2].x = rand()%(width-Bonus[2].width);
			Bonus[2].y = rand()%(height-100);
		break;
	}
	bonus_active=1;
	bonus=0; //flag
}	

void BonusesDraw(int j)
{
	while(bonus_active==1)
	{
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, Bonus[j].bmp, VAL_ENTIRE_OBJECT, MakeRect(Bonus[j].y,Bonus[j].x,Bonus[j].hight,Bonus[j].width));
	break;
	}
}

void Draw()
{
		 
	
	   CanvasStartBatchDraw (panelHandle, PANEL_CANVAS);
	   CanvasClear (panelHandle, PANEL_CANVAS, VAL_ENTIRE_OBJECT);
	   
	   //Background ----------------------------------------------------------------
	   
	   switch(LEVEL)
	   {
		   case 1:CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[0], VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); break;//Background 1
		   case 2:CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[1], VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); break;//Background 2
		   case 3:CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[2], VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); break;//Boss 1
		   case 4:CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[3], VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); break;//Background 3
		   case 5:CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[4], VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); break;//Background 4
		   case 6:CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[5], VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT); break;//Boss 2
	   }
	   
	   //Stars------------------------------------------------------------------------
	   CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[6] , VAL_ENTIRE_OBJECT				
							  						,MakeRect((tick_counter)%600,0,600,600) );    
	   CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[6] , VAL_ENTIRE_OBJECT					   
							  						,MakeRect((tick_counter)%600-600,0,600,600) );
	   
	    //Bonus Items-----------------------------------------------------------------
		if(bonus)
		{
			
			if(Player.Life < 30 || Player.Ammo < 10)
				randBonus = rand()%2;
			 else randBonus = rand()%3;
			 BonusesCreation(randBonus);
		}
		BonusesDraw(randBonus);
		//Level arrow--------------------------------------------------------------------------------------------
		if(ene_active == 0 && boss_active == 0 && LEVEL != 6)
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_lvl, VAL_ENTIRE_OBJECT, MakeRect(10,250,135,100));
		//Hero -----------------------------------------------------------------------
		if(Keys_Down[38])
		   CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_hero[1], VAL_ENTIRE_OBJECT, MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width));
	   	   else
			   CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_hero[0], VAL_ENTIRE_OBJECT, MakeRect(Hero.y,Hero.x,Hero.hight,Hero.width));
		
		//Validate Hit ------------------------------------------------------------------------
		Hit();
	    //Enemy ----------------------------------------------------------------------
		if(ene_create_flag)
			EnemyCreation();
		EnemyDraw();
		 
		//Shoot------------------------------------------------------------------
			//Hero Shoot Condition
		if(Keys_Down[17] && Player.Ammo >0)
			hero_shoot=1;
			
		Shoot();
		
		
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, bmp_bg[LEVEL-1], MakeRect(0,0,80,600), MakeRect (600, 0, 80, 600)); //Background for panel
		CanvasEndBatchDraw (panelHandle, PANEL_CANVAS);
	   
}



int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "SpaceAttack.uir", PANEL)) < 0)
		return -1;
	if ((panelHandleMain = LoadPanel (0, "SpaceAttack.uir", PANEL_2)) < 0)
		return -1;
	if ((panelHandleTop = LoadPanel (0, "SpaceAttack.uir", PANEL_3)) < 0)
		return -1;
	if ((panelHandleAbout = LoadPanel (0, "SpaceAttack.uir", PANEL_4)) < 0)
		return -1;
	Connect_To_Windows_Events();
	initiate();
	DisplayPanel (panelHandleMain);
	RunUserInterface ();
	terminate();
	
	return 0;
}

int CVICALLBACK QuitGamePanel (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
				reset();
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				HidePanel (panelHandle);
			break;
	}
	return 0;
}

int CVICALLBACK Tick (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			HeroMovement();
			Draw();
			tick_counter++;
			SetCtrlVal (panelHandle, PANEL_NUMERIC_LIFE, Player.Life);
			SetCtrlVal (panelHandle, PANEL_NUMERIC_AMMO, Player.Ammo);
			SetCtrlVal (panelHandle, PANEL_NUMERIC_EXP, Player.Exp);
			
			//Win/Loose-------------------------------------------------------------
			if(Player.Life<=0 || Player.Exp <= 0)
					Loose();
			if(LEVEL==6 && boss_active==0)
					Win();
			break;
	}
	return 0;
}

int CVICALLBACK StartGame (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetCtrlVal (panelHandleMain, PANEL_2_STRING, Player.Name);
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1); //Start timer
			DisplayPanel(panelHandle);

			break;
	}
	return 0;
}

int CVICALLBACK QuitMenuPanel (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			  QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK QuitGame (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{   
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			  QuitUserInterface (0);
			break;
	}
	return 0;
}

void CVICALLBACK DemoVid (int menuBar, int menuItem, void *callbackData,
						  int panel)
{
	OpenDocumentInDefaultViewer ("Demo.mp4", VAL_NO_ZOOM);
}

void CVICALLBACK HelpDoc (int menuBar, int menuItem, void *callbackData,
						  int panel)
{
	OpenDocumentInDefaultViewer ("Help.pdf", VAL_NO_ZOOM);
}

int CVICALLBACK ResetGame (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			reset();
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1); //Start timer

			break;
	}
	return 0;
}

int CVICALLBACK PAUSE (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	int flag;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_TOGGLEBUTTON, &flag);
			if(flag)
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				
			else 
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			

			break;
	}
	return 0;
}

int CVICALLBACK About (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			DisplayPanel(panelHandleAbout);

			break;
	}
	return 0;
}

int CVICALLBACK Board (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			DisplayPanel(panelHandleTop);

			break;
	}
	return 0;
}

int CVICALLBACK QuitTopChart (int panel, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
				 HidePanel (panelHandleTop);
			break;
	}
	return 0;
}

int CVICALLBACK QuitAbout (int panel, int event, void *callbackData,
						   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
				 HidePanel (panelHandleAbout);
			break;
	}
	return 0;
}

int CVICALLBACK SoundControl (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			  GetCtrlVal (panelHandle, PANEL_SOUNDONOFF, &sndflag);
			  SetCtrlVal (panelHandleMain, PANEL_2_SOUNDONOFF, sndflag);
			  if(!sndflag)
				 BASS_ChannelStop(Music_SND);
			  else
				BASS_ChannelPlay(Music_SND,TRUE);
			  
			break;
	}
	return 0;
}

int CVICALLBACK SoundControlMain (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandleMain, PANEL_2_SOUNDONOFF, &sndflag);
			SetCtrlVal (panelHandle, PANEL_SOUNDONOFF, sndflag);
			  
			  if(!sndflag)
				 BASS_ChannelStop(Music_SND);
			  else
				BASS_ChannelPlay(Music_SND,TRUE);

			break;
	}
	return 0;
}
