#ifndef MAINGAMESTATE_H
#define MAINGAMESTATE_H

#include <list>

#include <hgegui.h>
#include <hgeCurvedAni.h>
#include "common/gamestate.h"
#include "entity/cannon.h"

using namespace std;

class Ant;
class Bullet;
struct BulletData;
class hgeFont;
class hgeSprite;
class Map;

class MainGameState : public GameState
{
public:
    MainGameState() : hge(0), animResManager(0), system(0), picker(0), curPick(0), restCake(0)
    {
        assert(s_Instance == 0);
        s_Instance = this;
    }
    virtual ~MainGameState();

    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnFrame();
    virtual void OnRender();

    static MainGameState *GetInstance()
    {
        return s_Instance;
    }
    cAni::iAnimResManager *GetAnimResManager()
    {
        return animResManager;
    }
    Ant *getTargetAnt(const hgeVector &pos, float maxRange);
    void fire(const hgeVector &pos, Ant &targetAnt, const BulletData &bulletData);
    void fire(const hgeVector &pos, const hgeVector &targetDirection, const BulletData &bulletData);
    void getNearestAnts(vector<Ant *> &hitAnts, const hgeVector &pos, float maxRange);

    // �����õ��⣬����õ�������true
    bool getCake(const hgeVector &pos);
    // �����͵��⵽��Ѩ������ʹ����true
    bool putCake(const hgeVector &pos);
    
    const hgeVector &getCakePos() const
    {
        return cakePos;
    }
    const hgeVector &getAntLairPos() const
    {
        return antLairPos;
    }
protected:
    int getCannonPrice() const;
    void addCannon(BaseCannon::CannonId cannonid, float x, float y);
    AimEntity *findAimedEntity(float x, float y) const;

    void SetPick(AimEntity *newPick);
    void ShowCannonUi();
    void HideCannonUi();
    void ShowAntUi();
    void HideAntUi();

    // Ǯ�����仯ʱ���ñ����̵�����ǰ��ʹ�õ�������ť
    void UpdateUi();

    HGE *hge;

    list<Ant *> ants, antsDelList;
    vector<BaseCannon *> cannons;
    list<Bullet *> bullets;
    vector<hgeVector> cakeBack;
    cAni::iAnimResManager *animResManager;
    hgeCurvedAniSystem *system;
    HTEXTURE texGui;

    int points;
    int money;
    int curAntLevel;
    float lastSpawnAntTime;
	int restCake; // ���µĵ�������
    
    hgeVector antLairPos;
    hgeVector cakePos;

    bool bAddNewCannon;

    Map *map;

    bool mouseLButtonDown;
    cAni::iAnimation *cake;
    hgeSprite *cursor, *cursorWithCannon;
    hgeGUI *gui;
    hgeFont *font;
    static MainGameState *s_Instance;

    cAni::iAnimation *picker;
    cAni::iAnimation *range;
    cAni::iAnimation *singleCake;
    hgeVector pickerCurPos;

    AimEntity *aimEntityHead;
    AimEntity *curPick;
};

#endif
