#include "entity/cannon.h"
#include "entity/bullet.h"
#include "entity/ant.h"
#include "game/maingamestate.h"

BaseCannon::BaseCannon(cAni::iAnimResManager &arm, const CannonData *_data) : AimEntity(arm), data(_data)
{
    assert(data);
    pos;
    targetPos;
    anim_base;
    anim_tower;
    lastFireTime = 0;

    anim_base = iSystem::GetInstance()->createAnimation();
    anim_base->setAnimData(data->getAd_base(this->animResManager), 0);
    anim_tower = iSystem::GetInstance()->createAnimation();
    anim_tower->setAnimData(data->getAd_tower(this->animResManager), 0);
    anim_tower->setAnimLoop(false);
}

BaseCannon::~BaseCannon()
{
    iSystem::GetInstance()->release(anim_base);
    iSystem::GetInstance()->release(anim_tower);
}

void BaseCannon::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (float)(int)pos.x, (float)(int)pos.y, 0, 1, 1);
    anim_base->render(time, 0);

    hgeVector dir = targetPos - pos;
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
    hge->Gfx_SetTransform(0, 0, (float)(int)pos.x, (float)(int)pos.y, angle, 1, 1);
    anim_tower->render(time, 0);
    hge->Release();
}

void BaseCannon::step(float deltaTime)
{
    deltaTime;

    Ant *ant = MainGameState::GetInstance()->getTargetAnt(this->pos, this->data->range);
    if (ant)
    {
        hgeVector tp = ant->getPos();
        HGE* hge = hgeCreate(HGE_VERSION);
        float curtime = hge->Timer_GetTime();
        if ((curtime - lastFireTime) * this->data->freq > 1)
        {
            this->fire(tp);
            this->lastFireTime = curtime;
            anim_tower->startAnim(int(curtime * 60));
        }
        hge->Release();
    }
}
void BaseCannon::fire(const hgeVector &targetPos)
{
    this->targetPos = targetPos;
    hgeVector dir = targetPos - this->pos;
    dir.Normalize();
    dir *= this->data->range;
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
}
void DoubleCannon::fire(const hgeVector &targetPos)
{
    this->targetPos = targetPos;
    hgeVector dir = targetPos - this->pos;
    dir.Normalize();
    dir *= this->data->range;
    dir.Rotate(0.2f);
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
    dir.Rotate(-0.4f);
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
}

void TripleCannon::fire(const hgeVector &targetPos)
{
    this->targetPos = targetPos;
    hgeVector dir = targetPos - this->pos;
    dir.Normalize();
    dir *= this->data->range;
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
    dir.Rotate(0.15f);
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
    dir.Rotate(-0.3f);
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
}

void MachineGun::fire(const hgeVector &targetPos)
{
    this->targetPos = targetPos;
    hgeVector dir = targetPos - this->pos;
    dir.Normalize();
    dir *= this->data->range;
    dir.Rotate(((float) rand() / RAND_MAX - 0.5) * 0.5);
    MainGameState::GetInstance()->fire(this->pos, dir, this->data->getBulletData());
}
void MissileLauncher::step(float deltaTime)
{
    deltaTime;

    Ant *ant = MainGameState::GetInstance()->getTargetAnt(this->pos, this->data->range);
    if (ant)
    {
        hgeVector tp = ant->getPos();
        HGE* hge = hgeCreate(HGE_VERSION);
        float curtime = hge->Timer_GetTime();
        if ((curtime - lastFireTime) * this->data->freq > 1)
        {
            this->targetPos = tp;
            MainGameState::GetInstance()->fire(this->pos, *ant, this->data->getBulletData());
            this->lastFireTime = curtime;
            anim_tower->startAnim(int(curtime * 60));
        }
        hge->Release();
    }
}
BaseCannon *BaseCannon::upgrade(size_t index) const
{
    assert(index < 3);
    BaseCannon::CannonId new_cid = data->evolution[index];
    if (new_cid == BaseCannon::CI_NULL)
        return 0;

    return g_cannonData[new_cid].createInstance(this->animResManager);
}

BaseCannon *BaseCannon::degrade() const
{
    BaseCannon::CannonId new_cid = data->parent;
    if (new_cid == BaseCannon::CI_NULL)
        return 0;

    return g_cannonData[new_cid].createInstance(this->animResManager);
}

const BulletData &CannonData::getBulletData() const
{
    assert(this->bulletId < Bullet::NumBulletId);
    return g_bulletData[this->bulletId];
}

const cAni::AnimData *CannonData::getAd_base(cAni::iAnimResManager &arm) const
{
    return arm.getAnimData((this->ad_base + ".xml").c_str());
}

const cAni::AnimData *CannonData::getAd_tower(cAni::iAnimResManager &arm) const
{
    return arm.getAnimData((this->ad_tower + ".xml").c_str());
}

const cAni::AnimData *CannonData::getAd_buttonUp(cAni::iAnimResManager &arm) const
{
    return arm.getAnimData((this->ad_button + "_up.xml").c_str());
}

const cAni::AnimData *CannonData::getAd_buttonDown(cAni::iAnimResManager &arm) const
{
    return arm.getAnimData((this->ad_button + "_down.xml").c_str());
}
/*
第一个加农炮的训练费用为30，设加农炮的建造费用为Y，则加农炮的建造费用计算公式为“Yx=Yx-1*1.5n-1”，n为炮塔的序号，直接去除小数，因此每个炮塔的建造费用为：
加农炮1 30 45 67 100 150 225 337 505 757 1135 1702 
*/
CannonData g_cannonData[BaseCannon::NumCannonId] = 
{
    //id                          parent                       evolution[3]
    //   name             攻击频率      子弹          射程     建造费用
    //   炮座
    //   炮塔
    //   升级按钮
    {
        BaseCannon::CI_Cannon,       BaseCannon::CI_NULL,         {BaseCannon::CI_HeavyCannon1, BaseCannon::CI_QuickCannon1, BaseCannon::CI_DoubleCannon1,},
        "Cannon",           3.0f, Bullet::BI_CannonA, 110.0f, -1,
        "data/cannon/base/3",
        "data/cannon/tower/10",
        "data/cannon/button/0",
    },
    {
        BaseCannon::CI_HeavyCannon1, BaseCannon::CI_Cannon,       {BaseCannon::CI_HeavyCannon2, BaseCannon::CI_ImpactCannon1, BaseCannon::CI_DoubleHeavyCannon1,},
        "Heavy Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 60,
        "data/cannon/base/4",
        "data/cannon/tower/12",
        "data/cannon/button/0",
    },
    {
        BaseCannon::CI_HeavyCannon2, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_HeavyCannon3, BaseCannon::CI_MissileLauncher1, BaseCannon::CI_NULL,},
        "Heavy Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 120,
        "data/cannon/base/7",
        "data/cannon/tower/3",
        "data/cannon/button/5",
    },
    {
        BaseCannon::CI_HeavyCannon3, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Heavy Cannon 3",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/20",
    },
    {
        BaseCannon::CI_MissileLauncher1, BaseCannon::CI_HeavyCannon2, {BaseCannon::CI_MissileLauncher2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Missile Launcher 1",   0.2f, Bullet::BI_Missile1, 120.0f, 288,
        "data/cannon/base/29", //
        "data/cannon/tower/3", //
        "data/cannon/button/21",
    },
    {
        BaseCannon::CI_MissileLauncher2, BaseCannon::CI_MissileLauncher1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Missile Launcher 2",   0.2f, Bullet::BI_Missile1, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/21",
    },
    {
        BaseCannon::CI_ImpactCannon1, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_ImpactCannon2, BaseCannon::CI_IceCannon1, BaseCannon::CI_NULL,},
        "Impact Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/8",
        "data/cannon/tower/3",
        "data/cannon/button/6",
    },
    {
        BaseCannon::CI_ImpactCannon2, BaseCannon::CI_ImpactCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Impact Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/25",
    },
    {
        BaseCannon::CI_IceCannon1, BaseCannon::CI_ImpactCannon1, {BaseCannon::CI_IceCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Ice Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/26",
    },
    {
        BaseCannon::CI_IceCannon2, BaseCannon::CI_IceCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Ice Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/26",
    },
    {
        BaseCannon::CI_DoubleHeavyCannon1, BaseCannon::CI_HeavyCannon1, {BaseCannon::CI_DoubleHeavyCannon2, BaseCannon::CI_SonicPulse1, BaseCannon::CI_NULL,},
        "Double Heavy Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/9",
        "data/cannon/tower/3",
        "data/cannon/button/7",
    },
    {
        BaseCannon::CI_DoubleHeavyCannon2, BaseCannon::CI_DoubleHeavyCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Heavy Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/22",
    },
    {
        BaseCannon::CI_SonicPulse1, BaseCannon::CI_DoubleHeavyCannon1, {BaseCannon::CI_SonicPulse2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sonic Pulse 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/27",
    },
    {
        BaseCannon::CI_SonicPulse2, BaseCannon::CI_SonicPulse1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sonic Pulse 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/27",
    },
    {
        BaseCannon::CI_QuickCannon1, BaseCannon::CI_Cannon, {BaseCannon::CI_QuickCannon2, BaseCannon::CI_SniperCanon1, BaseCannon::CI_LongRangeCannon1,},
        "Quick Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/5",
        "data/cannon/tower/3",
        "data/cannon/button/1",
    },
    {
        BaseCannon::CI_QuickCannon2, BaseCannon::CI_QuickCannon1, {BaseCannon::CI_QuickCannon3, BaseCannon::CI_FlameThrower1, BaseCannon::CI_NULL,},
        "Quick Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/10",
    },
    {
        BaseCannon::CI_QuickCannon3, BaseCannon::CI_QuickCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Quick Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/3",
    },
    {
        BaseCannon::CI_FlameThrower1, BaseCannon::CI_QuickCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Flame Thrower 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/4",
    },
    {
        BaseCannon::CI_SniperCanon1, BaseCannon::CI_QuickCannon1, {BaseCannon::CI_SniperCanon2, BaseCannon::CI_Laser1, BaseCannon::CI_NULL,},
        "Sniper Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/11",
    },
    {
        BaseCannon::CI_SniperCanon2, BaseCannon::CI_SniperCanon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Sniper Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/8",
    },
    {
        BaseCannon::CI_Laser1, BaseCannon::CI_SniperCanon1, {BaseCannon::CI_Laser2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Laser 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/9",
    },
    {
        BaseCannon::CI_Laser2, BaseCannon::CI_Laser1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Laser 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/9",
    },
    {
        BaseCannon::CI_LongRangeCannon1, BaseCannon::CI_QuickCannon1, {BaseCannon::CI_LongRangeCannon2, BaseCannon::CI_EletricCannon1, BaseCannon::CI_NULL,},
        "Long Range Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/12",
    },
    {
        BaseCannon::CI_LongRangeCannon2, BaseCannon::CI_LongRangeCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Long Range Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/13",
    },

    {
        BaseCannon::CI_EletricCannon1, BaseCannon::CI_LongRangeCannon1, {BaseCannon::CI_EletricCannon2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Eletric Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/14",
    },
    {
        BaseCannon::CI_EletricCannon2, BaseCannon::CI_EletricCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Eletric Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/14",
    },

    {
        BaseCannon::CI_DoubleCannon1, BaseCannon::CI_Cannon, {BaseCannon::CI_DoubleCannon2, BaseCannon::CI_DoubleQuickCannon1, BaseCannon::CI_TripleCannon1,},
        "Double Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/6",
        "data/cannon/tower/14",
        "data/cannon/button/2",
    },
    {
        BaseCannon::CI_DoubleCannon2, BaseCannon::CI_DoubleCannon1, {BaseCannon::CI_DoubleCannon3, BaseCannon::CI_Boomerang, BaseCannon::CI_NULL,},
        "Double Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/15",
    },
    {
        BaseCannon::CI_DoubleCannon3, BaseCannon::CI_DoubleCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Cannon 3",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/18",
    },
    {
        BaseCannon::CI_Boomerang, BaseCannon::CI_DoubleCannon2, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Boomerang",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/19",
    },
    {
        BaseCannon::CI_DoubleQuickCannon1, BaseCannon::CI_DoubleCannon1, {BaseCannon::CI_DoubleQuickCannon2, BaseCannon::CI_MachineGun, BaseCannon::CI_NULL,},
        "Double Quick Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/16",
    },
    {
        BaseCannon::CI_DoubleQuickCannon2, BaseCannon::CI_DoubleQuickCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Double Quick Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/23",
    },
    {
        BaseCannon::CI_MachineGun, BaseCannon::CI_DoubleQuickCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Machine Gun",   12.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/20",
        "data/cannon/button/24",
    },
    {
        BaseCannon::CI_TripleCannon1, BaseCannon::CI_DoubleCannon1, {BaseCannon::CI_TripleCannon2, BaseCannon::CI_PoisonSpray1, BaseCannon::CI_NULL,},
        "Triple Cannon 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", // 
        "data/cannon/button/17",
    },
    {
        BaseCannon::CI_TripleCannon2, BaseCannon::CI_TripleCannon1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Triple Cannon 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/28",
    },
    {
        BaseCannon::CI_PoisonSpray1, BaseCannon::CI_TripleCannon1, {BaseCannon::CI_PoisonSpray2, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Poison Spray 1",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/29",
    },
    {
        BaseCannon::CI_PoisonSpray2, BaseCannon::CI_PoisonSpray1, {BaseCannon::CI_NULL, BaseCannon::CI_NULL, BaseCannon::CI_NULL,},
        "Poison Spray 2",   3.0f, Bullet::BI_CannonA, 120.0f, 288,
        "data/cannon/base/3", //
        "data/cannon/tower/3", //
        "data/cannon/button/29",
    },
};

BaseCannon *CannonData::createInstance(cAni::iAnimResManager &arm) const
{
    BaseCannon *cannon = 0;
    switch(this->id)
    {
    case BaseCannon::CI_Cannon:
    case BaseCannon::CI_HeavyCannon1:
    case BaseCannon::CI_HeavyCannon2:
    case BaseCannon::CI_HeavyCannon3:
    case BaseCannon::CI_ImpactCannon1:
    case BaseCannon::CI_ImpactCannon2:
    case BaseCannon::CI_IceCannon1:
    case BaseCannon::CI_IceCannon2:
    case BaseCannon::CI_SonicPulse1:
    case BaseCannon::CI_SonicPulse2:
    case BaseCannon::CI_QuickCannon1:
    case BaseCannon::CI_QuickCannon2:
    case BaseCannon::CI_QuickCannon3:
    case BaseCannon::CI_FlameThrower1:
    case BaseCannon::CI_SniperCanon1:
    case BaseCannon::CI_SniperCanon2:
    case BaseCannon::CI_Laser1:
    case BaseCannon::CI_Laser2:
    case BaseCannon::CI_LongRangeCannon1:
    case BaseCannon::CI_LongRangeCannon2:
    case BaseCannon::CI_EletricCannon1:
    case BaseCannon::CI_EletricCannon2:
    case BaseCannon::CI_Boomerang:
    case BaseCannon::CI_PoisonSpray1:
    case BaseCannon::CI_PoisonSpray2:
        cannon = new BaseCannon(arm, this);
        break;
    case BaseCannon::CI_DoubleHeavyCannon1:
    case BaseCannon::CI_DoubleHeavyCannon2:
    case BaseCannon::CI_DoubleCannon1:
    case BaseCannon::CI_DoubleCannon2:
    case BaseCannon::CI_DoubleCannon3:
    case BaseCannon::CI_DoubleQuickCannon1:
    case BaseCannon::CI_DoubleQuickCannon2:
        cannon = new DoubleCannon(arm, this);
        break;
    case BaseCannon::CI_TripleCannon1:
    case BaseCannon::CI_TripleCannon2:
        cannon = new TripleCannon(arm, this);
        break;
    case BaseCannon::CI_MachineGun:
        cannon = new MachineGun(arm, this);
        break;
    case BaseCannon::CI_MissileLauncher1:
    case BaseCannon::CI_MissileLauncher2:
        cannon = new MissileLauncher(arm, this);
        break;
    default:
        assert(0);
        break;
    }
    return cannon;
}

void CannonData::releaseInstance(BaseCannon *cannon) const
{
    assert(cannon && cannon->data == this);
    delete cannon;
}

bool CannonInit()
{
    for (int i = 0; i < BaseCannon::NumCannonId; i++)
    {
        const CannonData &cd = g_cannonData[i];
        assert(cd.id == i);
        for (int j = 0; j < 3; j++)
        {
            BaseCannon::CannonId e = cd.evolution[j];
            if (e == BaseCannon::CI_NULL)
                continue;
            assert(e < BaseCannon::NumCannonId);
            assert(g_cannonData[e].parent == cd.id);
        }
    }
    return true;
}