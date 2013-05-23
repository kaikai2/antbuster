#ifndef CANNON_H
#define CANNON_H

#include <hgevector.h>
#include <hgeCurvedAni.h>
#include "common/aimentity.h"

struct BulletData;
struct CannonData;

class BaseCannon : public AimEntity
{
public:
    enum CannonId
    {
        CI_NULL = -1,

        CI_Cannon = 0,
        CI_HeavyCannon1,        CI_HeavyCannon2, CI_HeavyCannon3,
        CI_MissileLauncher1,    CI_MissileLauncher2,
        CI_ImpactCannon1,       CI_ImpactCannon2,
        CI_IceCannon1,          CI_IceCannon2,
        CI_DoubleHeavyCannon1,  CI_DoubleHeavyCannon2,
        CI_SonicPulse1,         CI_SonicPulse2,
        CI_QuickCannon1,        CI_QuickCannon2, CI_QuickCannon3,
        CI_FlameThrower1,
        CI_SniperCanon1,        CI_SniperCanon2,
        CI_Laser1,              CI_Laser2,
        CI_LongRangeCannon1,    CI_LongRangeCannon2,
        CI_EletricCannon1,      CI_EletricCannon2,
        CI_DoubleCannon1,       CI_DoubleCannon2, CI_DoubleCannon3,
        CI_Boomerang,
        CI_DoubleQuickCannon1,  CI_DoubleQuickCannon2,
        CI_MachineGun,
        CI_TripleCannon1,       CI_TripleCannon2,
        CI_PoisonSpray1,        CI_PoisonSpray2,

        NumCannonId,
    };
    BaseCannon(cAni::iAnimResManager &arm, const CannonData *data);
    virtual ~BaseCannon();
    virtual void render(int time);
    virtual void step(float deltaTime);
    virtual AimType getAimType() const
    {
        return AT_Cannon;
    }
    // 成功升级，则返回升级后的地址，否则返回0。如果成功则需要删除原BaseCannon
    BaseCannon *upgrade(size_t index) const;
    // 降级，同升级
    BaseCannon *degrade() const;

    inline float getRange() const;
    const CannonData &getData() const
    {
        return *data;
    }
protected:
    virtual void fire(const hgeVector &targetPos);
    friend struct CannonData;
    const CannonData *const data;

    cAni::iAnimation *anim_base;
    cAni::iAnimation *anim_tower;

    hgeVector targetPos;

    float lastFireTime;
};
class DoubleCannon : public BaseCannon
{
public:
    DoubleCannon(cAni::iAnimResManager &arm, const CannonData *data) : BaseCannon(arm, data)
    {
    }
protected:
    virtual void fire(const hgeVector &targetPos);
};
class TripleCannon : public BaseCannon
{
public:
    TripleCannon(cAni::iAnimResManager &arm, const CannonData *data) : BaseCannon(arm, data)
    {
    }
protected:
    virtual void fire(const hgeVector &targetPos);
};
class MachineGun : public BaseCannon
{
public:
    MachineGun(cAni::iAnimResManager &arm, const CannonData *data) : BaseCannon(arm, data)
    {
    }
protected:
    virtual void fire(const hgeVector &targetPos);
};
class MissileLauncher : public BaseCannon
{
public:
    MissileLauncher(cAni::iAnimResManager &arm, const CannonData *data) : BaseCannon(arm, data)
    {
    }
    virtual void step(float deltaTime);
protected:
};
struct CannonData
{
    BaseCannon::CannonId id;
    BaseCannon::CannonId parent; // -1 表示无祖先
    BaseCannon::CannonId evolution[3]; // 进化的下一级，显示调用的CannonInit内根据parent检验它的正确性。

    string name;
    float freq; // 攻击频率
    size_t bulletId;
    const BulletData &getBulletData() const;
    float range; // 射程
    int price; // 建造费用

    string ad_base;  // 底座部分（固定部分）
    string ad_tower; // 炮塔部分（可旋转的部分）
    string ad_button; // 升级按钮
    const cAni::AnimData *getAd_base(cAni::iAnimResManager &arm) const; 
    const cAni::AnimData *getAd_tower(cAni::iAnimResManager &arm) const;
    const cAni::AnimData *getAd_buttonUp(cAni::iAnimResManager &arm) const;
    const cAni::AnimData *getAd_buttonDown(cAni::iAnimResManager &arm) const;

    BaseCannon *createInstance(cAni::iAnimResManager &arm) const;
    void releaseInstance(BaseCannon *) const;
};

inline
float BaseCannon::getRange() const
{
    assert(data);
    return data->range;
}

extern CannonData g_cannonData[BaseCannon::NumCannonId];
extern bool CannonInit();
#endif