#ifndef ANT_H
#define ANT_H

#include <hgevector.h>
#include <hgeCurvedAni.h>
#include "common/aimentity.h"

/* @class Ant
 * @note ant
 */
class Ant : public AimEntity
{
public:
    enum DamageType
    {
        DT_Normal, // 普通攻击
        DT_Impact, // 冲击
        DT_Frozen, // 冰冻
        DT_Fire,   // 火焰
        DT_Poison, // 中毒

        NumDamageType,
    };

    Ant(cAni::iAnimResManager &arm);
    virtual ~Ant();
    virtual void render(int time);
    virtual void step(float deltaTime);
    virtual AimType getAimType() const
    {
        return AT_Ant;
    }

    void initAnt(const hgeVector &spawnPos, int level);
    int getHp() const
    {
        return hp;
    }
    int getMaxHp() const
    {
        return int(4 * pow(1.1, level) - 1);
    }
    int getLevel() const
    {
        return level;
    }
    float getSpeed() const;
    void applyDamage(DamageType damageType, int damage);
    int refCount; // do not delete the ant if refCount is not 0, this is used by missile like bullets

    bool hasCake() const
    {
        return carryCake;
    }
protected:
    void applyDamageEffect();

    int hp;
    int level;

    cAni::iAnimation *anim;
    cAni::iAnimation *hpAnim;
	cAni::iAnimation *cakeAnim;
    float angle;

    hgeVector dest;

    int damageEffect[NumDamageType];

    float moveMeter; // 移动路程

	bool carryCake; // 是否携带蛋糕
};

/*
1、蚂蚁的速度是1.2，负重后速度降为1.0，冰冻后最低速度0.3左右。

2、第一个蚂蚁的血量为4，设蚂蚁的血量为Y，则蚂蚁的血量计算公式为“Yx=4*1.1x-1”。 
3、杀死蚂蚁后获得的积分和金钱与蚂蚁的级别相同。

4、屏幕上最多同时会有6只蚂蚁。

5、蚂蚁的AI比看起来的低，经常无视蛋糕四处乱跑。

6、蚂蚁抗上蛋糕的时候会恢复一半的血。
*/

#endif//ANT_H