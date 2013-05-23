#include "entity/map.h"
#include "game/bgdata.h"

const cAni::Rect border(150 + 16, 650 - 8, 50 + 16, 448 - 8);

Map::Map(cAni::iAnimResManager &arm) : Entity(arm)
{
    bg = iSystem::GetInstance()->createAnimation(2);
    bg->setAnimData(this->animResManager.getAnimData("data/bg0.xml"), 0); // 
    bg->setAnimData(this->animResManager.getAnimData("data/bg1.xml"), 1);
    hlc = iSystem::GetInstance()->createAnimation(2);
    hlc->setAnimData(this->animResManager.getAnimData("data/highlight0.xml"), 0); // 可以安放
    hlc->setAnimData(this->animResManager.getAnimData("data/highlight1.xml"), 1); // 禁止摆放

    HGE *hge = hgeCreate(HGE_VERSION);
    pos = hgeVector(hge->System_GetState(HGE_SCREENWIDTH) / 2.f, hge->System_GetState(HGE_SCREENHEIGHT) / 2.f);
    bg->startAnim(int(60 * hge->Timer_GetTime()), 0);
    hge->Release();
    
    w = (border.GetWidth() + 15) / 16;
    h = (border.GetHeight() + 15) / 16;
    nodes = new Node[w * h];

    showHlc = false;
    mouseHotPoint = hgeVector(8, 7);
}
Map::~Map()
{
    if (nodes)
    {
        delete [] nodes;
        nodes = 0;
    }
    iSystem::GetInstance()->release(bg);
    iSystem::GetInstance()->release(hlc);
}
void Map::render(int time)
{
    HGE* hge = hgeCreate(HGE_VERSION);
    hge->Gfx_SetTransform(0, 0, (float)(int)pos.x, (float)(int)pos.y, 0, 1, 1);
    bg->render(time, 0);
    if (showHlc)
    {
        float x, y;
        hge->Input_GetMousePos(&x, &y);
        if (isInMapRange(x, y))
        {
            if (checkCannonPos(x, y))
            {
                hlc->startAnim(0, 0);
            }
            else
            {
                hlc->startAnim(0, 1);
            }
            hge->Gfx_SetTransform(0, 0, (float)(int)x, (float)(int)y, 0, 1, 1);
            hlc->render(time, 0);
        }
    }
#if 0
    for (int x = 0; x < 800; x += 16)
        for (int y = 0; y < 600; y += 16)
        {
            float fx = x, fy = y;
            if (isInMapRange(fx, fy))
            {
                if (checkCannonPos(fx, fy))
                {
                    hlc->startAnim(0, 0);
                }
                else
                {
                    hlc->startAnim(0, 1);
                }
                hge->Gfx_SetTransform(0, 0, (float)(int)fx, (float)(int)fy, 0, 1, 1);
                hlc->render(time, 0);
            }
        }
#endif
    hge->Release();
}

void Map::step(float deltaTime)
{
    deltaTime;
}
bool Map::isInMapRange(float x, float y) const
{
    x += mouseHotPoint.x;
    y += mouseHotPoint.y;
    return x >= border.left && x < border.right && y >= border.top && y < border.bottom + 8;
}
bool Map::checkCannonPos(float &x, float &y) const
{
    if (!isInMapRange(x, y))
        return false;

    x += mouseHotPoint.x;
    y += mouseHotPoint.y;
    int bx = int((x - border.left) / 16);
    int by = int((y - border.top) / 16);
    x = float(bx * 16 + border.left);
    y = float(by * 16 + border.top);
    if (bx >= 0 && bx < BGV_BLOCK_MAX &&
        by >= 0 && by < BGH_BLOCK_MAX &&
        bgData[by][bx] == '1')
    {
        if (bx < w && by < h)
        {
            int xlo = bx > 0 ? bx - 1 : 0, xhi = bx < w - 1 ? bx + 1 : w - 1;
            int ylo = by > 0 ? by - 1 : 0, yhi = by < h - 1 ? by + 1 : h - 1;
            for (int i = xlo; i <= xhi; i++)
                for (int j = ylo; j <= yhi; j++)
                    if (nodes[i + j * w].occupied)
                        return false;
            return true;
        }
    }
    return false;
}

void Map::setOccupied(float x, float y)
{
    int bx = int((x - border.left) / 16);
    int by = int((y - border.top) / 16);
    if (bx >= 0 && bx < w && by >= 0 && by < h)
    {
        nodes[bx + by * w].occupied = true;
    }
}

void Map::clearOccupied(float x, float y)
{
    int bx = int((x - border.left) / 16);
    int by = int((y - border.top) / 16);
    if (bx >= 0 && bx < w && by >= 0 && by < h)
    {
        nodes[bx + by * w].occupied = false;
    }
}