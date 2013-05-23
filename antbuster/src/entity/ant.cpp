#include <hge.h>
#include "caRect.h"
#include "entity/ant.h"
#include "game/maingamestate.h"

const cAni::Rect border(150 + 16, 650 - 16, 50 + 16, 448 - 16);
const float dlen = 20;
const int dMaxlen = 50;

Ant::Ant(cAni::iAnimResManager &arm) : AimEntity(arm), moveMeter(0), carryCake(false)
{
    this->dest.x = this->pos.x = float(rand() % border.GetWidth() + border.left);
    this->dest.y = this->pos.y = float(rand() % border.GetHeight() + border.top);
    this->angle = (float)rand() / RAND_MAX * 3.1415926f;
    this->level = 1;
    this->hp = this->getMaxHp();
    this->refCount = 0;
    memset(this->damageEffect, 0, sizeof(this->damageEffect));

    this->anim = iSystem::GetInstance()->createAnimation();
    this->anim->setAnimData(animResManager.getAnimData("data/ant.xml"), 0);
    this->hpAnim = iSystem::GetInstance()->createAnimation();
    this->hpAnim->setAnimData(animResManager.getAnimData("data/anthp.xml"), 0);
	this->cakeAnim = iSystem::GetInstance()->createAnimation();
	this->cakeAnim->setAnimData(animResManager.getAnimData("data/antcake.xml"), 0);
}

Ant::~Ant()
{
    iSystem::GetInstance()->release(anim);
	iSystem::GetInstance()->release(hpAnim);
	iSystem::GetInstance()->release(cakeAnim);
    assert(refCount == 0);
}

void Ant::initAnt(const hgeVector &spawnPos, int level)
{
    this->dest = this->pos = spawnPos;
    this->level = level;
    this->hp = this->getMaxHp();
    memset(this->damageEffect, 0, sizeof(this->damageEffect));
}

void Ant::render(int time)
{
    time;

    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (float)(int) this->pos.x, (float)(int) this->pos.y, -this->angle, 1, 1);
	this->anim->render((int) this->moveMeter, 0);
	if (this->carryCake)
	{
		this->cakeAnim->render(time, 0);
	}
    hge->Gfx_SetTransform(0, 0, (float)(int) this->pos.x, (float)(int) this->pos.y, 0, 1, 1);
    this->hpAnim->render(int(100.0f * this->hp / this->getMaxHp()), 0);
    hge->Gfx_SetTransform();
    float alpha = (dest - pos).Length() / 50;
    DWORD color = int(255 * alpha);
    if (color > 255)
        color = 255;
    color = color << 24 | 0xffffff;
    hge->Gfx_RenderLine(this->pos.x, this->pos.y, this->dest.x, this->dest.y, color);
	
    hge->Release();
}

void Ant::applyDamage(DamageType damageType, int damage)
{
    assert(damageType >= 0 && damageType < NumDamageType);
    assert(damage > 0);
    this->damageEffect[damageType] += damage;
}
void Ant::applyDamageEffect()
{
    this->hp -= this->damageEffect[DT_Normal]; // ÆÕÍ¨¹¥»÷
    this->damageEffect[DT_Normal] = 0;   // Á¢¼´ÉúÐ§

    this->hp -= this->damageEffect[DT_Impact]; // ³å»÷
    this->damageEffect[DT_Impact] /= 2;  // ÓÐË¥¼õµÄÐ§¹û

    if (this->damageEffect[DT_Frozen] > 0) // ±ù¶³
        this->hp--, this->damageEffect[DT_Frozen]--; 

    if (this->damageEffect[DT_Fire] > 0) // »ðÑæ
        this->hp--, this->damageEffect[DT_Fire]--; 

    if (this->damageEffect[DT_Poison] > 0) // ÖÐ¶¾
        this->hp--, this->damageEffect[DT_Poison]--; 

    if (this->hp < 0)
    {
        this->hp = 0;
        this->active = false;
    }
}

float Ant::getSpeed() const
{
    float speed = 1.2f;
    if (this->carryCake)
        speed = 1.0f;
    speed -= 0.1f * this->damageEffect[DT_Frozen];
    if (speed < 0.3f)
        speed = 0.3f;
    return speed;
}

void Ant::step(float deltaTime)
{
    if ((this->dest - this->pos).Length() < dlen * 0.1f)
    {
#if 0
        int dx = rand() % (dMaxlen * 2 + 1) - dMaxlen;
        int dy = rand() % (dMaxlen * 2 + 1) - dMaxlen;
        if (dx > 0) dx += (int)dlen; else dx -= (int)dlen;
        if (dy > 0) dy += (int)dlen; else dy -= (int)dlen;
        this->dest = hgeVector(pos.x + dx, pos.y + dy);
#else
        int x = int((pos.x - border.left) / dlen + 0.5) * dlen + border.left;
        int y = int((pos.y - border.top) / dlen + 0.5) * dlen + border.top;
        hgeVector curDestPos = this->carryCake ? MainGameState::GetInstance()->getAntLairPos() : MainGameState::GetInstance()->getCakePos();
        //curDestPos.x > x
        if (rand() % 3)
        {
            int r = rand() % 3;
            if (r & 1)
            {
                if (curDestPos.x > x)
                    x += dlen;
                else if (curDestPos.x < x)
                    x -= dlen;
            }
            if (r & 2)
            {
                if (curDestPos.y > y)
                    y += dlen;
                else if (curDestPos.y < y)
                    y -= dlen;
            }
        }
        else
        {
            int r = rand() % 8;
            if (r >= 4) r ++;
            if (r % 3 == 0)
                x -= dlen;
            else if (r % 3 == 2)
                x += dlen;
            if (r / 3 == 0)
                y -= dlen;
            else if (r / 3 == 2)
                y += dlen;
        }
        this->dest = hgeVector(x, y);
#endif
        if (this->dest.x < border.left)
            this->dest.x = border.left;
        if (this->dest.x > border.right)
            this->dest.x = border.right;
        if (this->dest.y < border.top)
            this->dest.y = border.top;
        if (this->dest.y > border.bottom)
            this->dest.y = border.bottom;
    }

    if (this->carryCake)
    {
        // ÓÐµ°¸â£¬ËÍ»ØÒÏÑ¨
        if (MainGameState::GetInstance()->putCake(this->pos))
        {
            this->carryCake = false;
        }
    }
    else
    {
        // È¡µÃµ°¸â
        if (MainGameState::GetInstance()->getCake(this->pos))
        {
            this->carryCake = true;
            this->hp = min(this->getMaxHp(), this->getMaxHp() / 2 + this->hp);
        }
    }

    hgeVector curdir(0, -1);
    hgeVector oldPos = this->pos; 
    curdir.Rotate(this->angle);
    hgeVector destdir = *(this->dest - this->pos).Normalize();
    float offangle = curdir.Angle(&destdir);
    if (curdir.x * destdir.y - curdir.y * destdir.x < 0)
        offangle = -offangle;
    this->angle += offangle * 0.05f;
    this->pos += 60 * deltaTime * curdir * this->getSpeed() * min(1, 0.1 / abs(offangle));// * (1 - 2 * sqrt(abs(offangle)));

    if (this->pos.x < border.left)
        this->pos.x = border.left;
    if (this->pos.x > border.right)
        this->pos.x = border.right;
    if (this->pos.y < border.top)
        this->pos.y = border.top;
    if (this->pos.y > border.bottom)
        this->pos.y = border.bottom;

    this->moveMeter += (oldPos - this->pos).Length();
    this->applyDamageEffect();
}