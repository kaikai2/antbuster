#include "entity/bullet.h"
#include "entity/ant.h"
#include "game/maingamestate.h"
Bullet::Bullet(cAni::iAnimResManager &arm, const BulletData *_data) : Entity(arm), data(_data)
{
    anim = iSystem::GetInstance()->createAnimation();
    anim->setAnimData(data->getAd_move(), 0);
    anim->startAnim(0, 0);
}

Bullet::~Bullet()
{
    iSystem::GetInstance()->release(anim);
}

void Bullet::setTarget(Ant &ant)
{
    this->target = ant.getPos();
}
void Bullet::render(int time)
{
    HGE *hge = hgeCreate(HGE_VERSION);
    hgeVector dir = target - pos;
    hgeVector up(0, -1);
    if (dir.Length() < 0.5f)
    {
        dir = up;
    }
    else
    {
        dir.Normalize();
    }
    float angle = dir.Angle(&up);
    if (dir.x > 0)
        angle = -angle;
    hge->Gfx_SetTransform(0, 0, pos.x, pos.y, angle, 1, 1);
    anim->render(time, 0);

    hge->Release();
}

void Bullet::step(float deltaTime)
{
    hgeVector dir = target - pos;
    float distance = dir.Length();
    float moveDistance = this->data->bulletSpeed * deltaTime;
    if (distance < moveDistance)
    {
        active = false;
    }
    else
    {
        dir.Normalize();
        dir *= moveDistance;
        pos += dir;

        vector<Ant *> hitAnts;
        MainGameState::GetInstance()->getNearestAnts(hitAnts, this->pos, 10.0f);
        if (!hitAnts.empty())
        {
            this->data->applyDamage(*hitAnts.front());
            active = false;
        }
    }
}

Missile::~Missile()
{
    if (this->targetAnt)
    {
        assert(this->targetAnt->refCount > 0);
        this->targetAnt->refCount--;
    }
}
void Missile::setTarget(Ant &ant)
{
    ant.refCount++;
    this->targetAnt = &ant;
}
void Missile::step(float deltaTime)
{
    if (targetAnt)
    {
        if (targetAnt->getHp() > 0)
        {
            this->target = targetAnt->getPos();
        }
        else
        {
            assert(this->targetAnt->refCount > 0);
            this->targetAnt->refCount--;
            this->targetAnt = 0;
            hgeVector dir = this->target - this->pos; 
            dir.Normalize();
            this->target = dir * 200 + this->pos;
        }
    }
    Bullet::step(deltaTime);
}


const cAni::AnimData *BulletData::getAd_fire() const  // 发射
{
    return ad_fire.empty() ? 0 : MainGameState::GetInstance()->GetAnimResManager()->getAnimData(ad_fire.c_str());
}
const cAni::AnimData *BulletData::getAd_move() const  // 飞行
{
    return ad_move.empty() ? 0 : MainGameState::GetInstance()->GetAnimResManager()->getAnimData(ad_move.c_str());
}
const cAni::AnimData *BulletData::getAd_explode() const // 爆炸
{
    return ad_explode.empty() ? 0 : MainGameState::GetInstance()->GetAnimResManager()->getAnimData(ad_explode.c_str());
}

Bullet *BulletData::createInstance(cAni::iAnimResManager &arm) const// 生成一个新的本类型子弹
{
    Bullet *bullet = 0;
    switch(this->bulletId)
    {
    case Bullet::BI_CannonA:
    case Bullet::BI_CannonB:
    case Bullet::BI_CannonC:
    case Bullet::BI_CannonD:
        bullet = new Bullet(arm, this);
        break;
    case Bullet::BI_Missile1:
        bullet = new Missile(arm, this);
        break;
    default:
        assert(0);
        break;
    }
    return bullet;
}
void BulletData::releaseInstance(Bullet *bullet) const // 释放该子弹
{
    assert(bullet && bullet->data == this);
    delete bullet;
}
void BulletData::applyDamage(Ant &ant) const
{
    switch(this->bulletId)
    {
    case Bullet::BI_CannonA:
        ant.applyDamage(Ant::DT_Normal, damage);
        break;
    case Bullet::BI_CannonB:
        ant.applyDamage(Ant::DT_Normal, damage / 2);
        ant.applyDamage(Ant::DT_Impact, damage / 2);
        break;
    case Bullet::BI_CannonC:
        ant.applyDamage(Ant::DT_Normal, damage);
        ant.applyDamage(Ant::DT_Impact, damage / 2);
        break;
    case Bullet::BI_CannonD:
        ant.applyDamage(Ant::DT_Normal, damage);
        ant.applyDamage(Ant::DT_Impact, damage);
        break;
    case Bullet::BI_Missile1:
        ant.applyDamage(Ant::DT_Normal, damage);
        ant.applyDamage(Ant::DT_Impact, damage);
        break;
    default:
        assert(0);
        break;
    }
}
BulletData g_bulletData[Bullet::NumBulletId] =
{
    {Bullet::BI_CannonA, "Cannon A", 50.0, 1, "", "data/bullet.xml", "", },
    {Bullet::BI_CannonB, "Cannon B", 60.0, 2, "data/bullet_fire.xml", "data/bulletb.xml", "data/bullet_explode.xml", },
    {Bullet::BI_CannonC, "Cannon C", 70.0, 4, "data/bullet_fire.xml", "data/bulletc.xml", "data/bullet_explode.xml", },
    {Bullet::BI_CannonD, "Cannon D", 100.0, 2, "data/bullet_fire.xml", "data/bulletc.xml", "data/bullet_explode.xml", },
    {Bullet::BI_Missile1, "Missile 1", 30.0, 8, "", "data/bullet.xml", "", },
};
