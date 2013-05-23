#ifndef BULLET_H
#define BULLET_H

#include <hgeVector.h>
#include <hgeCurvedAni.h>
#include "common/entity.h"

struct BulletData;
class Ant;
class Bullet : public Entity
{
public:
    enum BulletId
    {
        BI_CannonA, // NOTE they should have some more significative names
        BI_CannonB,
        BI_CannonC,
        BI_CannonD,
        BI_Missile1,
        BI_Missile2,

        NumBulletId,
    };
    Bullet(cAni::iAnimResManager &arm, const BulletData *data);
    virtual ~Bullet();
    virtual void render(int time);
    virtual void step(float deltaTime);

    void setPos(const hgeVector &pos)
    {
        this->pos = pos;
    }
    void setTarget(const hgeVector &target)
    {
        this->target = target;
    }
    virtual void setTarget(Ant &ant);
    cAni::iAnimation *anim;

    const BulletData * const data;
    hgeVector pos;
    hgeVector target;
};

class Missile : public Bullet
{
public:
    Missile(cAni::iAnimResManager &arm, const BulletData *data) : Bullet(arm, data), targetAnt(0)
    {
    }
    ~Missile();
    virtual void step(float deltaTime);
    virtual void setTarget(Ant &ant);

    Ant *targetAnt; //target ant
};

struct BulletData
{
    Bullet::BulletId bulletId;
    string name;
    float bulletSpeed; // �ӵ��ٶ�
    int damage; // �˺�
    string ad_fire;  // ����
    string ad_move;  // ����
    string ad_explode; // ��ը
    const cAni::AnimData *getAd_fire() const;  // ����
    const cAni::AnimData *getAd_move() const;  // ����
    const cAni::AnimData *getAd_explode() const; // ��ը
    Bullet *createInstance(cAni::iAnimResManager &arm) const; // ����һ���µı������ӵ�
    void releaseInstance(Bullet *bullet) const; // �ͷŸ��ӵ�
    void applyDamage(Ant &ant) const;
};

extern BulletData g_bulletData[Bullet::NumBulletId];

#endif//BULLET_H