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
        DT_Normal, // ��ͨ����
        DT_Impact, // ���
        DT_Frozen, // ����
        DT_Fire,   // ����
        DT_Poison, // �ж�

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

    float moveMeter; // �ƶ�·��

	bool carryCake; // �Ƿ�Я������
};

/*
1�����ϵ��ٶ���1.2�����غ��ٶȽ�Ϊ1.0������������ٶ�0.3���ҡ�

2����һ�����ϵ�Ѫ��Ϊ4�������ϵ�Ѫ��ΪY�������ϵ�Ѫ�����㹫ʽΪ��Yx=4*1.1x-1���� 
3��ɱ�����Ϻ��õĻ��ֺͽ�Ǯ�����ϵļ�����ͬ��

4����Ļ�����ͬʱ����6ֻ���ϡ�

5�����ϵ�AI�ȿ������ĵͣ��������ӵ����Ĵ����ܡ�

6�����Ͽ��ϵ����ʱ���ָ�һ���Ѫ��
*/

#endif//ANT_H