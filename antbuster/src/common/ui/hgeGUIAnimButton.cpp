#include "common/ui/hgeGUIAnimButton.h"

/*
** hgeGUIAnimButton
*/

hgeGUIAnimButton::hgeGUIAnimButton(int _id, float x, float y, float w, float h)
{
    anim = iSystem::GetInstance()->createAnimation(NumAnimId);
	id = _id;
	bStatic = false;
	bVisible = true;
	bEnabled = true;
	rect.Set(x, y, x + w, y + h);

	bPressed = false;
	bTrigger = false;
}

hgeGUIAnimButton::~hgeGUIAnimButton()
{
    iSystem::GetInstance()->release(anim);
}

void hgeGUIAnimButton::Render()
{
    HGE *hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (rect.x1 + rect.x2) / 2, (rect.y1 + rect.y2) / 2, 0, 1, 1);
    anim->render((int) (hge->Timer_GetTime() * 60), 0);
    hge->Gfx_SetTransform();
    hge->Release();
}

bool hgeGUIAnimButton::MouseLButton(bool bDown)
{
	if (bDown)
	{
		bOldState = bPressed;
        bPressed = true;
        anim->startAnim(0, bTrigger && bOldState ? TriggedDown : ButtonDown);
		return false;
	}
	else
	{
		if (bTrigger)
        {
            bPressed = !bOldState;
            anim->startAnim(0, bPressed ? TriggedDown : TriggedUp);
        }
		else 
        {
            bPressed = false;
            anim->startAnim(0, ButtonUp);
        }
		return true; 
	}
}