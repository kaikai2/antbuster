#include <cmath>
#include <cassert>
#include <cctype>
#include <algorithm>

#include <hgefont.h>
#include <hgesprite.h>
#include <hgeguictrls.h>
#include <hgecolor.h>

#include "game/maingamestate.h"
#include "common/entity.h"
#include "common/ui/hgeGUIAnimButton.h"
#include "entity/ant.h"
#include "entity/bullet.h"
#include "entity/cannon.h"
#include "entity/map.h"

using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(a) if (!a);else {delete a; a = 0;}
#endif

enum GUI_ID
{
    /// GUI main
    GID_BtnAddCannon = 1,
    GID_BtnMute,
    GID_BtnPause,
    GID_TxtPoints,
    GID_TxtPointsValue,
    GID_TxtMoney,
    GID_TxtMoneyValue,
    GID_TxtLevel,

    /// GUI Cannon
    GID_BtnCannonUpgradeA,
    GID_BtnCannonUpgradeB,
    GID_BtnCannonUpgradeC,
    GID_BtnCannonDegrade,
    GID_BtnCannonCancel,
    GID_TxtCannonName,
    // cannon info ...

    /// GUI Ant
    GID_BtnAntCancel,
    GID_TxtAntLevel,
    // ant info ...

    NumGUIId,
};
MainGameState *MainGameState::s_Instance = 0;
MainGameState::~MainGameState()
{
    assert(!cursor);
    assert(!gui);
    assert(s_Instance == this);
    assert(!animResManager && !system);
    assert(!hge);
    assert(ants.empty());
    assert(cannons.empty());
    s_Instance = 0;
}

void MainGameState::OnEnter()
{
    hge = hgeCreate(HGE_VERSION);
    assert(!animResManager && !system);
    assert(CannonInit());
    system = new hgeCurvedAniSystem;
    animResManager = iSystem::GetInstance()->createAnimResManager();

    curAntLevel = 1;
    points = 0;
    money = 200;
	restCake = 8;

    antLairPos = hgeVector(184, 83);
    cakePos = hgeVector(620, 417);

    lastSpawnAntTime = hge->Timer_GetTime();

    mouseLButtonDown = hge->Input_GetKeyState(HGEK_LBUTTON);
    bAddNewCannon = false;

    aimEntityHead = new AimEntity(*animResManager);

    font = new hgeFont("data/font.fnt");
    font->SetColor(ARGB(255, 0, 255, 0));
    
    gui = new hgeGUI;
    // gui->AddCtrl(new hgeGUIButton(1, 100, 520, 10, 10,));
    this->texGui = hge->Texture_Load("data/ui.png");
    cursor = new hgeSprite(this->texGui, 0, 120, 32, 32);
    cursorWithCannon = new hgeSprite(this->texGui, 32, 120, 32, 32);
    cursorWithCannon->SetHotSpot(8, 7);
    gui->AddCtrl(new hgeGUIButton(GID_BtnAddCannon, 170, 465, 50, 51, this->texGui, 0, 0));
    gui->AddCtrl(new hgeGUIButton(GID_BtnMute, 544, 503, 27, 28, this->texGui, 0, 52));
    gui->AddCtrl(new hgeGUIButton(GID_BtnPause, 514, 503, 27, 28, this->texGui, 0, 85));
    ((hgeGUIButton*)gui->GetCtrl(GID_BtnMute))->SetMode(true);
    ((hgeGUIButton*)gui->GetCtrl(GID_BtnPause))->SetMode(true);

    // add cannon ui
    hgeGUIAnimButton *animBtn;
    gui->AddCtrl(animBtn = new hgeGUIAnimButton(GID_BtnCannonUpgradeA, 236, 481, 40, 40)); // ));
    animBtn->SetMode(false);
    gui->AddCtrl(animBtn = new hgeGUIAnimButton(GID_BtnCannonUpgradeB, 236 + 40, 481, 40, 40));
    animBtn->SetMode(false);
    gui->AddCtrl(animBtn = new hgeGUIAnimButton(GID_BtnCannonUpgradeC, 236 + 80, 481, 40, 40));
    animBtn->SetMode(false);
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonDegrade, 236 + 120, 481, 40, 40, texGui, 0, 177));
    //animBtn->SetMode(false);
    gui->AddCtrl(new hgeGUIButton(GID_BtnCannonCancel, 236 + 243 , 481 + 24, 32, 32, texGui, 0, 177));
    //animBtn->SetMode(false);
    gui->AddCtrl(new hgeGUIText(GID_TxtCannonName, 236, 481 - 40, 100, 40, this->font));
    hgeGUIText *cannonName = (hgeGUIText *)gui->GetCtrl(GID_TxtCannonName);
    cannonName->SetColor(hgeColorRGB(0, .8f, 0, 1).GetHWColor());
    cannonName->SetMode(HGETEXT_LEFT);
    this->HideCannonUi();

    // add ant ui
    gui->AddCtrl(new hgeGUIButton(GID_BtnAntCancel, 236 + 243 , 481 + 24, 32, 32, texGui, 0, 177));
    this->HideAntUi();


    gui->SetCursor(cursor);
    gui->Enter();


    cake = iSystem::GetInstance()->createAnimation();
    cake->setAnimData(this->animResManager->getAnimData("data/cake.xml"), 0);

    picker = iSystem::GetInstance()->createAnimation(2);
    picker->setAnimData(this->animResManager->getAnimData("data/pickant.xml"), 0);
    picker->setAnimData(this->animResManager->getAnimData("data/pickcannon.xml"), 1);
    
    range = iSystem::GetInstance()->createAnimation();
    range->setAnimData(this->animResManager->getAnimData("data/range.xml"), 0);

    singleCake = iSystem::GetInstance()->createAnimation();
    singleCake->setAnimData(this->animResManager->getAnimData("data/singlecake.xml"), 0);

    this->map = new Map(*this->animResManager);
}

void MainGameState::OnLeave()
{
    SAFE_DELETE(map);
    SAFE_DELETE(cake);
    SAFE_DELETE(picker);
    SAFE_DELETE(range);
    SAFE_DELETE(singleCake);
    SAFE_DELETE(gui);
    SAFE_DELETE(cursor);
    SAFE_DELETE(cursorWithCannon);
    SAFE_DELETE(font);
    if (this->texGui)
    {
        hge->Texture_Free(this->texGui);
        this->texGui = 0;
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        delete *cannon;
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
    {
        delete *bullet;
    }
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        delete *ant;
    }
    ants.clear();
    cannons.clear();
    bullets.clear();
    SAFE_DELETE(animResManager);
    SAFE_DELETE(system);
    assert(!aimEntityHead->getNext());
    SAFE_DELETE(aimEntityHead);
    hge->Release();
    hge = 0;
}

void MainGameState::addCannon(BaseCannon::CannonId cannonid, float x, float y)
{
    assert(cannonid == BaseCannon::CI_Cannon);
    assert(cannonid >= 0 && cannonid < BaseCannon::NumCannonId);
    int cannonPrice = getCannonPrice();
    if (money < cannonPrice)
        return;
    money -= cannonPrice;
    BaseCannon *cannon = g_cannonData[cannonid].createInstance(*animResManager);
    cannon->setPos(hgeVector(x, y));
    map->setOccupied(x, y);
    cannons.push_back(cannon);
    assert(aimEntityHead);
    aimEntityHead->insertAfter(*cannon);
    UpdateUi();
}

AimEntity *MainGameState::findAimedEntity(float x, float y) const
{
    hgeVector ap(x, y);
    assert(aimEntityHead);
    float minLen = 10;
    AimEntity *minAe = 0;
    for (AimEntity *ae = aimEntityHead->getNext(); ae; ae = ae->getNext())
    {
        float len = (ae->getPos() - ap).Length();
        if (len < minLen)
        {
            minLen = len;
            minAe = ae;
        }
    }
    return minAe;
}

void MainGameState::SetPick(AimEntity *newPick)
{
    if (this->curPick)
    {
        switch(this->curPick->getAimType())
        {
        case AimEntity::AT_Ant:
            this->HideAntUi();
            break;
        case AimEntity::AT_Cannon:
            this->HideCannonUi();
            break;
        }
    }
    this->curPick = newPick;
    if (newPick == 0)
        return;
    switch(this->curPick->getAimType())
    {
    case AimEntity::AT_Ant:
        this->picker->startAnim(-1, 0);
        this->ShowAntUi();
        break;
    case AimEntity::AT_Cannon:
        this->picker->startAnim(-1, 1);
        this->ShowCannonUi();
        break;
    default:
        assert(0);
    }
}

void MainGameState::OnFrame()
{
    const float deltaTime = hge->Timer_GetDelta();
    if (hge->Input_GetKeyState(HGEK_ESCAPE))
    {
        // RequestState("mainmenu");
        RequestState("exit");
    }

    if (hge->Input_GetKeyState(HGEK_LBUTTON))
    {
        if (!mouseLButtonDown)
        {
            float x, y;
            hge->Input_GetMousePos(&x, &y);
            if (bAddNewCannon)
            {
                if (map->checkCannonPos(x, y))
                    addCannon(BaseCannon::CI_Cannon, x, y);

                bAddNewCannon = false;
                gui->SetCursor(cursor);
                map->setShowHighLightClip(false);
            }
            else
            {
                AimEntity *newPick = this->findAimedEntity(x, y);
                if (newPick)
                {
                    SetPick(newPick);
                }
            }
        }
        mouseLButtonDown = true;
    }
    else
        mouseLButtonDown = false;

    if (curPick)
    {
        pickerCurPos += (curPick->getPos() - pickerCurPos) * 0.1f;
    }

    int id = gui->Update(deltaTime);
    switch(id)
    {
    case GID_BtnAddCannon:
        bAddNewCannon = true;
        gui->SetCursor(cursorWithCannon);
        map->setShowHighLightClip(true);
        break;
    case GID_BtnPause:
		// if (restCake > 0) restCake--;
        break;
    case GID_BtnMute:
		// if (restCake < 8) restCake++;
        break;
    case GID_BtnCannonUpgradeA:
    case GID_BtnCannonUpgradeB:
    case GID_BtnCannonUpgradeC:
        {
            assert(curPick);
            assert(curPick->getAimType() == AimEntity::AT_Cannon);
            BaseCannon *cannon = (BaseCannon *)curPick;
            BaseCannon::CannonId nextCannonId = cannon->getData().evolution[id - GID_BtnCannonUpgradeA];
            if (money >= g_cannonData[nextCannonId].price)
            {
                money -= g_cannonData[nextCannonId].price;
                BaseCannon *newCannon = cannon->upgrade(id - GID_BtnCannonUpgradeA);
                if (newCannon)
                {
                    replace(this->cannons.begin(), this->cannons.end(), cannon, newCannon);
                    SetPick(newCannon);
                    newCannon->setPos(cannon->getPos());
                    assert(aimEntityHead);
                    aimEntityHead->insertAfter(*newCannon);
                    delete cannon;
                }
                UpdateUi();
            }
        }
        break;
    case GID_BtnCannonDegrade:
        {
            assert(curPick);
            assert(curPick->getAimType() == AimEntity::AT_Cannon);
            BaseCannon *cannon = (BaseCannon *)curPick;
            this->money += cannon->getData().price / 2;
            BaseCannon *newCannon = cannon->degrade();
            if (newCannon)
            {
                replace(this->cannons.begin(), this->cannons.end(), cannon, newCannon);
                SetPick(newCannon);
                newCannon->setPos(cannon->getPos());
                assert(aimEntityHead);
                aimEntityHead->insertAfter(*newCannon);
            }
            else
            {
                this->cannons.erase(remove(this->cannons.begin(), this->cannons.end(), cannon), this->cannons.end());
                SetPick(0);
                map->clearOccupied(cannon->getPos().x, cannon->getPos().y);
            }
            delete cannon;
            UpdateUi();
        }
        break;

    case GID_BtnAntCancel:
    case GID_BtnCannonCancel:
        SetPick(0);
        break;
    }
    list<Ant *>::iterator ant;
    for (ant = antsDelList.begin(); ant != antsDelList.end();)
    {
        if ((*ant)->refCount == 0)
        {
            delete *ant;
            ant = antsDelList.erase(ant);
        }
        else
            ++ant;
    }
    for (ant = ants.begin(); ant != ants.end();)
    {
        (*ant)->step(deltaTime);
        if (!(*ant)->isActive())
        {
            points += (*ant)->getLevel();
            money += (*ant)->getLevel();
            UpdateUi();
            if (this->curPick == *ant)
            {
                SetPick(0);
            }
            if ((*ant)->hasCake())
            {
                this->cakeBack.push_back((*ant)->getPos());
            }
            if ((*ant)->refCount > 0)
                antsDelList.push_back(*ant);
            else
                delete *ant;
            ant = ants.erase(ant);
        }
        else
            ++ant;
    }
    if (ants.size() < 6)
    {
        if (hge->Timer_GetTime() - lastSpawnAntTime > 1)
        {
            lastSpawnAntTime = hge->Timer_GetTime();
            Ant *ant = new Ant(*animResManager);
            ant->initAnt(this->getAntLairPos(), this->curAntLevel++);
            ants.push_back(ant);
            assert(aimEntityHead);
            aimEntityHead->insertAfter(*ant);
        }
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->step(deltaTime);
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end();)
    {
        (*bullet)->step(deltaTime);
        if (!(*bullet)->isActive())
        {
            (*bullet)->data->releaseInstance(*bullet);
            bullet = bullets.erase(bullet);
        }
        else
            ++bullet;
    }
    assert(this->restCake + cakeBack.size() <= 8);
    for (vector<hgeVector>::iterator cake = cakeBack.begin(); cake != cakeBack.end();)
    {
        *cake = getCakePos() * 0.1f + *cake * 0.9f;
        if ((*cake - getCakePos()).Length() < 16)
        {
            cake = cakeBack.erase(cake);
            assert(this->restCake < 8);
            this->restCake++;
        }
        else
            ++cake;
    }
}

bool MainGameState::getCake(const hgeVector &pos)
{
    if (this->restCake == 0)
        return false;

    if ((pos - this->cakePos).Length() > 25)
        return false;

    --this->restCake;
    return true;
}

// 蚂蚁送蛋糕到蚁穴，如果送达，返回true
bool MainGameState::putCake(const hgeVector &pos)
{
    assert(this->restCake < 8); // 肯定已经被取走蛋糕了
    if ((pos - this->antLairPos).Length() > 25)
        return false;

    return true;
}

void MainGameState::ShowCannonUi()
{
    assert(gui);
    assert(curPick && curPick->getAimType() == AimEntity::AT_Cannon);
    BaseCannon *cannon = (BaseCannon *) curPick;
    const CannonData &data = cannon->getData();

    for (int i = 0; i < 3; i++)
    {
        BaseCannon::CannonId cid = data.evolution[i];
        if (cid != BaseCannon::CI_NULL)
        {
            assert(cid < BaseCannon::NumCannonId);
            hgeGUIAnimButton *animBtn = (hgeGUIAnimButton *) gui->GetCtrl(GID_BtnCannonUpgradeA + i);
            animBtn->SetAnim(hgeGUIAnimButton::ButtonUp, g_cannonData[cid].getAd_buttonUp(*this->animResManager));
            animBtn->SetAnim(hgeGUIAnimButton::ButtonDown, g_cannonData[cid].getAd_buttonDown(*this->animResManager));
            gui->ShowCtrl(GID_BtnCannonUpgradeA + i, true);
            gui->EnableCtrl(GID_BtnCannonUpgradeA + i, true);
        }
    }
    gui->ShowCtrl(GID_BtnCannonDegrade, true);
    gui->EnableCtrl(GID_BtnCannonDegrade, true);

    gui->ShowCtrl(GID_BtnCannonCancel, true);
    gui->EnableCtrl(GID_BtnCannonCancel, true);

    gui->ShowCtrl(GID_TxtCannonName, true);
    hgeGUIText *cannonName = (hgeGUIText *)gui->GetCtrl(GID_TxtCannonName);
    assert(cannonName);
    cannonName->SetText(data.name.c_str());
}

void MainGameState::HideCannonUi()
{
    assert(gui);

    gui->ShowCtrl(GID_BtnCannonUpgradeA, false);
    gui->ShowCtrl(GID_BtnCannonUpgradeB, false);
    gui->ShowCtrl(GID_BtnCannonUpgradeC, false);
    gui->ShowCtrl(GID_BtnCannonDegrade, false);
    gui->ShowCtrl(GID_BtnCannonCancel, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeA, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeB, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeC, false);
    gui->EnableCtrl(GID_BtnCannonDegrade, false);
    gui->EnableCtrl(GID_BtnCannonCancel, false);
    gui->ShowCtrl(GID_TxtCannonName, false);
}

void MainGameState::ShowAntUi()
{
    assert(gui);
    assert(curPick && curPick->getAimType() == AimEntity::AT_Ant);

    gui->ShowCtrl(GID_BtnAntCancel, true);
    gui->EnableCtrl(GID_BtnAntCancel, true);
}

void MainGameState::HideAntUi()
{
    assert(gui);

    gui->ShowCtrl(GID_BtnAntCancel, false);
    gui->EnableCtrl(GID_BtnAntCancel, false);
}

void MainGameState::OnRender()
{
    hge->Gfx_BeginScene(0);
    hge->Gfx_Clear(ARGB(255, 0x22, 0xbb, 0x33));//黑色背景
    int time = int(60 * hge->Timer_GetTime());
    map->render(time);
    hge->Gfx_SetTransform(0, 0, cakePos.x, cakePos.y, 0, 1, 1);
    cake->render(restCake, 0);

    // draw range before ants and cannons
    if (this->curPick && this->curPick->getAimType() == AimEntity::AT_Cannon)
    {
        BaseCannon *cannon = (BaseCannon *) this->curPick;
        hge->Gfx_SetTransform(0, 0, cannon->getPos().x, cannon->getPos().y, 0, 1, 1);
        this->range->render((int) cannon->getRange(), 0);
    }
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        (*ant)->render(time);
    }
    for (vector<BaseCannon *>::iterator cannon = cannons.begin(); cannon != cannons.end(); ++cannon)
    {
        (*cannon)->render(time);
    }
    for (list<Bullet *>::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
    {
        (*bullet)->render(time);
    }
    for (vector<hgeVector>::const_iterator cake = cakeBack.begin(); cake != cakeBack.end(); ++cake)
    {
        hge->Gfx_SetTransform(0, 0, cake->x, cake->y, 0, 1, 1);
        this->singleCake->render(time, 0);
    }
    if (this->curPick)
    {
        hge->Gfx_SetTransform(0, 0, pickerCurPos.x, pickerCurPos.y, 0, 1, 1);
        this->picker->render(time, 0);
    }
    hge->Gfx_SetTransform();
    gui->Render();

    font->printf(620 - 78, 480, HGETEXT_CENTER, "%d", this->points);
    font->printf(610, 480, HGETEXT_CENTER, "%d", this->money);
    font->printf(640 - 14, 505, HGETEXT_CENTER, "%d", this->curAntLevel);

    // 游戏状态信息
    hge->Gfx_EndScene();
}

Ant *MainGameState::getTargetAnt(const hgeVector &pos, float maxRange)
{
    Ant *bestAnt = 0;
    float distance = maxRange + 1;
    if (this->curPick && this->curPick->getAimType() == AimEntity::AT_Ant)
    {
        Ant *ant = (Ant *)this->curPick;
        if ((ant->getPos() - pos).Length() <= maxRange)
            return ant;
    }
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        float d = ((*ant)->getPos() - pos).Length();
        if (d < distance)
        {
            distance = d;
            bestAnt = *ant;
        }
    }
    if (distance > maxRange)
        bestAnt = 0;

    return bestAnt;
}

void MainGameState::fire(const hgeVector &pos, Ant &targetAnt, const BulletData &bulletData)
{
    assert(this->animResManager);
    Bullet *bullet = bulletData.createInstance(*this->animResManager);
    bullet->setPos(pos);
    bullet->setTarget(targetAnt);
    this->bullets.push_back(bullet);
}

void MainGameState::fire(const hgeVector &pos, const hgeVector &targetDirection, const BulletData &bulletData)
{
    assert(this->animResManager);
    Bullet *bullet = bulletData.createInstance(*this->animResManager);
    bullet->setPos(pos);
    bullet->setTarget(pos + targetDirection);
    this->bullets.push_back(bullet);
}

struct CmpNearestFrom
{
    hgeVector pos;
    CmpNearestFrom(const hgeVector &_pos) : pos(_pos)
    {
    }
    bool operator () (Ant *const a, Ant *const b) const
    {
        return (a->getPos() - pos).Length() < (b->getPos() - pos).Length();
    }
};

void MainGameState::getNearestAnts(vector<Ant *> &hitAnts, const hgeVector &pos, float maxRange)
{
    for (list<Ant *>::iterator ant = ants.begin(); ant != ants.end(); ++ant)
    {
        float d = ((*ant)->getPos() - pos).Length();
        if (d < maxRange)
        {
            hitAnts.push_back(*ant);
        }
    }
    sort(hitAnts.begin(), hitAnts.end(), CmpNearestFrom(pos));
}

// 钱发生变化时调用本过程调整当前能使用的升级按钮
void MainGameState::UpdateUi()
{
    gui->EnableCtrl(GID_BtnAddCannon, money >= getCannonPrice());
    gui->EnableCtrl(GID_BtnCannonUpgradeA, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeB, false);
    gui->EnableCtrl(GID_BtnCannonUpgradeC, false);
    if (this->curPick)
    {
        switch(this->curPick->getAimType())
        {
        case AimEntity::AT_Ant:
            break;
        case AimEntity::AT_Cannon:
            {
                BaseCannon *cannon = (BaseCannon *)curPick;
                for (int i = 0; i < 3; i++)
                {
                    BaseCannon::CannonId nextCannonId = cannon->getData().evolution[i];
                    if (nextCannonId == BaseCannon::CI_NULL)
                        continue;
                    if (money >= g_cannonData[nextCannonId].price)
                    {
                        gui->EnableCtrl(GID_BtnCannonUpgradeA + i, true);
                    }
                }
            }
            break;
        }
    }
}

int MainGameState::getCannonPrice() const
{
    size_t cannonCount = this->cannons.size();
    int price = 30;
    for (size_t i = 0; i < cannonCount; i++)
    {
        price = int(price * 1.5f);
    }
    return price;
}