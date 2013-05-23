
#include "common/gamestate.h"

#include <cassert>

void GameState::OnEnter()
{
}

void GameState::OnLeave()
{
}

void GameState::SetName(const string &name)
{
    if (!GameStateManager::Instance()->FindState(name))
    {
        this->name = name;
    }
}

GameStateManager *GameStateManager::sInstance = 0;

GameStateManager *GameStateManager::Instance()
{
    if (sInstance == 0)
    {
        sInstance = new GameStateManager;
    }
    return sInstance;
}

void GameStateManager::ReleaseInstance()
{
    if (sInstance)
    {
        if (sInstance->curState)
        {
            sInstance->curState->OnLeave();
            sInstance->curState = 0;
        }
        delete sInstance;
        sInstance = 0;
    }
}

GameStateManager::GameStateManager() : curState(0)
{
    HGE *hge = hgeCreate(HGE_VERSION);
    assert(hge);
    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->Release();
}
GameStateManager::~GameStateManager()
{
    assert(!curState);
}

GameState *GameStateManager::FindState(const string &name) const
{
    for (vector<GameState *>::const_iterator s = states.begin(); s != states.end(); ++s)
    {
        if ((*s)->GetName() == name)
            return *s;
    }
    return 0;
}

void GameStateManager::RegisterState(GameState *state)
{
    assert(state);
    assert(!state->GetName().empty());
    assert(0 == FindState(state->GetName()));
    states.push_back(state);
}

void GameStateManager::RequestState(const string &name)
{
    assert(requestState.empty()); // already requested
    requestState = name;
}

bool GameStateManager::OnFrame()
{
    if (!requestState.empty())
    {
        if (curState)
        {
            curState->OnLeave();
            curState = 0;
        }
        if (requestState == "exit")
            return true;
        curState = FindState(requestState);
        requestState.clear();
        if (curState)
            curState->OnEnter();
    }

    if (curState)
        curState->OnFrame();

    return false;
}

static bool FrameFunc()
{
    return GameStateManager::Instance()->OnFrame();
}

static bool RenderFunc()
{
    GameState *gs = GameStateManager::Instance()->curState;
    if (gs)
    {
        gs->OnRender();
    }
    return false;
}
