#include <cassert>
#include <cmath>

#include "hgeSprite.h"
#include "hgefont.h"
#include "hgegui.h"

#include "common/menuitem.h"
#include "menu/mainmenustate.h"

void MainMenuState::OnEnter()
{
    //ÔØÈëÖ÷»­Ãæ
    hge = hgeCreate(HGE_VERSION);

    // Load sound and textures
    quad.tex=hge->Texture_Load("data\\tkt.png");
    // tex=hge->Texture_Load("cursor.png");
    snd=hge->Effect_Load("se\\menu.wav");
    if(!quad.tex || !snd)
    {
        // If one of the data files is not found, display
        // an error message and shutdown.
        MessageBox(NULL, "Can't load BG.PNG or MENU.WAV", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
        RequestState("exit");
        return;
    }

    // Set up the quad we will use for background animation
    quad.blend=BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_NOZWRITE;

    for(int i=0;i<4;i++)
    {
        // Set up z-coordinate of vertices
        quad.v[i].z=0.5f;
        // Set up color. The format of DWORD col is 0xAARRGGBB
        quad.v[i].col=0xFFFFFFFF;
    }

    quad.v[0].x=0; quad.v[0].y=0; 
    quad.v[1].x=800; quad.v[1].y=0; 
    quad.v[2].x=800; quad.v[2].y=600; 
    quad.v[3].x=0; quad.v[3].y=600; 

    quad.v[0].tx=0; quad.v[0].ty=0; 
    quad.v[1].tx=640.f/1024; quad.v[1].ty=0; 
    quad.v[2].tx=640.f/1024; quad.v[2].ty=480.f/512; 
    quad.v[3].tx=0; quad.v[3].ty=480.f/512; 

    // Load the font, create the cursor sprite
    fnt=new hgeFont("data/font1.fnt");
    //spr=new hgeSprite(tex,0,0,32,32);

    // Create and initialize the GUI
    gui=new hgeGUI();

    gui->AddCtrl(new hgeGUIMenuItem(1,fnt,snd,400,340,0.0f,"Start"));
    gui->AddCtrl(new hgeGUIMenuItem(2,fnt,snd,400,360,0.0f,"Exit"));
		gui->AddCtrl(new hgeGUIMenuItem(3,fnt,snd,400,380,0.0f,"Replay"));

    gui->SetNavMode(HGEGUI_UPDOWN);// | HGEGUI_CYCLED);
    //gui->SetCursor(spr);
    gui->SetFocus(1);
    gui->Enter();
}

void MainMenuState::OnLeave()
{
    // Delete created objects and free loaded resources
    delete gui;
    delete fnt;
    //delete spr;
    hge->Effect_Free(snd);
    // hge->Texture_Free(tex);
    hge->Texture_Free(quad.tex);
    hge->Release();
    hge = 0;
}

void MainMenuState::OnFrame()
{
    float dt=hge->Timer_GetDelta();
    static float t=0.0f;
    int id;
    static int lastid=0;

    // If ESCAPE was pressed, tell the GUI to finish
    // if(hge->Input_GetKeyState(HGEK_ESCAPE)) { lastid=5; gui->Leave(); }

    // We update the GUI and take an action if
    // one of the menu items was selected
    id=gui->Update(dt);
    if(id == -1)
    {
        switch(lastid)
        {
        case 1:
            RequestState("maingame");
            break;
        case 2:
            RequestState("exit");
            break;
        }
    }
    else if(id)
    {
        lastid = id;
        gui->Leave();
    }
}

void MainMenuState::OnRender()
{
    hge->Gfx_BeginScene();
    hge->Gfx_RenderQuad(&quad);
    gui->Render();
    fnt->SetColor(0xFFFFFFFF);
    fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d", hge->Timer_GetDelta(), hge->Timer_GetFPS());
    hge->Gfx_EndScene();
}
