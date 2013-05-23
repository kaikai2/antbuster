#ifndef HGEGUIANIMBUTTON_H
#define HGEGUIANIMBUTTON_H

#include <cassert>
#include "hgeguictrls.h"
#include <hgeCurvedAni.h>

/*
** hgeGUIAnimButton
*/
class hgeGUIAnimButton : public hgeGUIObject
{
public:
    hgeGUIAnimButton(int id, float x, float y, float w, float h);
	virtual			~hgeGUIAnimButton();

    enum AnimId
    {
        ButtonUp,
        ButtonDown,
        TriggedUp,
        TriggedDown,

        NumAnimId,
    };
    void SetAnim(AnimId animId, const cAni::AnimData *animData)
    {
        assert(anim);
        anim->setAnimData(animData, animId);
    }

	void			SetMode(bool _bTrigger) { bTrigger=_bTrigger; }
	void			SetState(bool _bPressed) { bPressed=_bPressed; }
	bool			GetState() const { return bPressed; }

	virtual void	Render();
	virtual bool	MouseLButton(bool bDown);

private:
	bool			bTrigger;
	bool			bPressed;
	bool			bOldState;
    cAni::iAnimation *anim;
};

#endif