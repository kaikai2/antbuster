#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <vector>
#include <hge.h>

using namespace std;

class GameState
{
public:
    GameState()
    {
    }
    virtual void OnEnter();
    virtual void OnLeave();

    virtual void OnFrame() {}
    virtual void OnRender() {}

    void SetName(const string &name);
    const string &GetName() const
    {
        return name;
    }

protected:
    void RequestState(const string &name) const;

    string name;
};

class GameStateManager
{
public:
    GameStateManager();
    virtual ~GameStateManager();

    void RegisterState(GameState *state);
    void RequestState(const string &name);
    GameState *FindState(const string &name) const;

    static GameStateManager *Instance();
    static void ReleaseInstance();

protected:
    bool OnFrame();
    static GameStateManager *sInstance;

    friend static bool FrameFunc();
    friend static bool RenderFunc();

    vector<GameState *> states;
    string requestState;
    GameState *curState;
};

inline
void
GameState::RequestState(const string &name) const
{
    GameStateManager::Instance()->RequestState(name);
}

#endif