/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: QuitGamePanel */
#define  PANEL_SPLITTER                   2       /* control type: splitter, callback function: (none) */
#define  PANEL_CANVAS                     3       /* control type: canvas, callback function: (none) */
#define  PANEL_NUMERIC_LIFE               4       /* control type: scale, callback function: (none) */
#define  PANEL_NUMERIC_EXP                5       /* control type: scale, callback function: (none) */
#define  PANEL_NUMERIC_AMMO               6       /* control type: scale, callback function: (none) */
#define  PANEL_LIFE_PICTURE               7       /* control type: picture, callback function: (none) */
#define  PANEL_AMMO_PICTURE               8       /* control type: picture, callback function: (none) */
#define  PANEL_EXP_PICTURE                9       /* control type: picture, callback function: (none) */
#define  PANEL_TOGGLEBUTTON               10      /* control type: textButton, callback function: PAUSE */
#define  PANEL_RESET                      11      /* control type: picture, callback function: ResetGame */
#define  PANEL_SNDPIC                     12      /* control type: picture, callback function: (none) */
#define  PANEL_SOUNDONOFF                 13      /* control type: radioButton, callback function: SoundControl */
#define  PANEL_TIMER                      14      /* control type: timer, callback function: Tick */

#define  PANEL_2                          2       /* callback function: QuitMenuPanel */
#define  PANEL_2_STRING                   2       /* control type: string, callback function: (none) */
#define  PANEL_2_START                    3       /* control type: picture, callback function: StartGame */
#define  PANEL_2_ABOUT                    4       /* control type: picture, callback function: About */
#define  PANEL_2_LEADERBOARD              5       /* control type: picture, callback function: Board */
#define  PANEL_2_QUITGAME                 6       /* control type: picture, callback function: QuitGame */
#define  PANEL_2_BACKGROUND               7       /* control type: picture, callback function: (none) */
#define  PANEL_2_SNDPIC                   8       /* control type: picture, callback function: (none) */
#define  PANEL_2_SOUNDONOFF               9       /* control type: radioButton, callback function: SoundControlMain */

#define  PANEL_3                          3       /* callback function: QuitTopChart */
#define  PANEL_3_TABLE                    2       /* control type: table, callback function: (none) */
#define  PANEL_3_TOPBG                    3       /* control type: picture, callback function: (none) */
#define  PANEL_3_STAR                     4       /* control type: picture, callback function: (none) */

#define  PANEL_4                          4       /* callback function: QuitAbout */
#define  PANEL_4_ABOUTBG                  2       /* control type: picture, callback function: (none) */
#define  PANEL_4_MEPIC                    3       /* control type: picture, callback function: (none) */
#define  PANEL_4_TEXTMSG                  4       /* control type: textMsg, callback function: (none) */
#define  PANEL_4_TEXTMSG_2                5       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_DEMO                     2       /* callback function: DemoVid */
#define  MENUBAR_HELP                     3       /* callback function: HelpDoc */


     /* Callback Prototypes: */

int  CVICALLBACK About(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Board(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK DemoVid(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK HelpDoc(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PAUSE(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitAbout(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitGame(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitGamePanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitMenuPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitTopChart(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ResetGame(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SoundControl(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SoundControlMain(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartGame(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Tick(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
