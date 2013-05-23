#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "common/gamestate.h"

#include <hge.h>
class hgeSprite;
class hgeGUI;
class hgeFont;

class MainMenuState : public GameState
{
public:
    MainMenuState() : hge(0), gui(0), fnt(0)//, spr(0)
    {

    }
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();

protected:
    HGE *hge;
    hgeGUI *gui;
    hgeFont *fnt;
   // hgeSprite *spr;
    // Some resource handles
    HEFFECT snd;
    HTEXTURE tex;
    hgeQuad quad;
};

#endif