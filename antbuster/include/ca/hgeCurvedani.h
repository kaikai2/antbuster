#ifndef HGE_CURVEDANI_H
#define HGE_CURVEDANI_H

#include "hge.h"
#include "curvedani.h"
#include "caImage.h"
#include "hgevector.h"
#include "caPoint3d.h"
#include "caRect.h"
#include "caInterpolaters.h"

#include <vector>
using namespace std;

using cAni::StateId;
using cAni::iTexture;
using cAni::iCurveDataSet;
using cAni::iCurveInterpolater;
using cAni::iClipState;
using cAni::iRenderer;
using cAni::iBitStream;
using cAni::iSystem;
using cAni::Image;
using cAni::Point3f;
using cAni::Rect;
using cAni::StepInterpolater;
using cAni::LinearInterpolater;
using cAni::BsplineInterpolater;


enum hgeCurvedAniStateId
{
    hgeSI_Image,    // size_t
    hgeSI_Position, // vector2
    hgeSI_Scale,    // vector2
    hgeSI_Angle,    // float
    hgeSI_Alpha,    // float
    hgeSI_Color,    // vector3
};

/// 实现下列接口
/// 纹理提供者
class hgeTexture : public iTexture
{
public:
    hgeTexture() : tex(0)
    {
        hge = hgeCreate(HGE_VERSION);
    }
    virtual ~hgeTexture()
    {
        if (tex)
        {
            hge->Texture_Free(tex);
        }
        hge->Release();
    }
    virtual size_t getWidth() const
    {
        return size_t(hge->Texture_GetWidth(tex));
    }
    virtual size_t getHeight() const
    {
        return size_t(hge->Texture_GetHeight(tex));
    }
    HTEXTURE getHgeTexture()
    {
        if (!tex)
        {
            loadTexture(this->getFilename());
        }
        return tex;
    }
    void loadTexture(const char *filename)
    {
        if (tex)
        {
            hge->Texture_Free(tex);
        }
        tex = hge->Texture_Load(filename);
    }
protected:
    HTEXTURE tex;
    static HGE *hge;
};

// 数据集
template<class T>
class hgeCurveDataSet : public iCurveDataSet
{
public:
    virtual const char *getTypeName() const;
    virtual bool loadFromBitStream(iBitStream *bs)
    {
        return iCurveDataSet::loadFromBitStream(bs);
    }
    virtual bool saveToBitStream(iBitStream *bs) const
    {
        return iCurveDataSet::saveToBitStream(bs);
    }
    virtual size_t getDataCount() const
    {
        return data.size();
    }
    virtual void resize(size_t index)
    {
        data.resize(index);
    }
    virtual void setData(size_t index, const void *value, int time)
    {
        assert(index < data.size());
        data[index].data = *(T*)value;
        data[index].time = time;
    }
    virtual void setData(size_t index, const void *value)
    {
        assert(index < data.size());
        data[index].data = *(T*)value;
    }
    virtual void setTime(size_t index, int time)
    {
        assert(index < data.size());
        data[index].time = time;
    }
    virtual int getTime(size_t index) const
    {
        assert(index < data.size());
        return data[index].time;
    }
    virtual const void *getData(size_t index) const
    {
        assert(index < data.size());
        return &data[index].data;
    }
    virtual size_t getDataSize() const
    {
        return sizeof(T);
    }
    virtual void setDataString(size_t index, const char *valueStr);
    virtual const char *getDataString(size_t index) const;
    struct DataNode
    {
        int time;
        T data;
    };
    vector<DataNode> data;
};

/// 参数
class hgeClipState : public iClipState
{
public:
    hgeClipState() : image(), position(0, 0), scale(1, 1), angle(0), alpha(1), color(1, 1, 1)
    {
        
    }
    virtual void reset()
    {
        image = Image();
        position = hgeVector(0, 0);
        scale = hgeVector(1, 1);
        angle = 0;
        alpha = 1;
        color = Point3f(1, 1, 1);
    }

    virtual StateId StringToStateId(const char *name)
    {
        return name, 0;
    }
    virtual void *getStateAddr(StateId sid)
    {
        switch(sid)
        {
        case hgeSI_Image:    // size_t
            return &image;
        case hgeSI_Position: // vector2
            return &position;
        case hgeSI_Scale:    // vector2
            return &scale;
        case hgeSI_Angle:    // float
            return &angle;
        case hgeSI_Alpha:    // float
            return &alpha;
        case hgeSI_Color:    // vector3
            return &color;
        default:
            assert(0);
            break;
        }
        return 0;
    }
    virtual void setState(StateId sid, const void *value)
    {
        switch(sid)
        {
        case hgeSI_Image:    // size_t
            image = *(Image*)value;
            break;
        case hgeSI_Position: // vector2
            position = *(hgeVector*)value;
            break;
        case hgeSI_Scale:    // vector2
            scale = *(hgeVector*)value;
            break;
        case hgeSI_Angle:    // float
            angle = *(float*)value;
            break;
        case hgeSI_Alpha:    // float
            alpha = *(float*)value;
            break;
        case hgeSI_Color:    // vector3
            color = *(Point3f*)value;
            break;
        default:
            assert(0);
            break;
        }
    }
    virtual const void *getState(StateId sid) const
    {
        switch(sid)
        {
        case hgeSI_Image:    // size_t
            return &image;
        case hgeSI_Position: // vector2
            return &position;
        case hgeSI_Scale:    // vector2
            return &scale;
        case hgeSI_Angle:    // float
            return &angle;
        case hgeSI_Alpha:    // float
            return &alpha;
        case hgeSI_Color:    // vector3
            return &color;
        default:
            assert(0);
            break;
        }
        return 0;
    }

    Image image;
    hgeVector position;
    hgeVector scale;
    float angle;
    float alpha;
    Point3f color;
};

class hgeRenderer : public iRenderer
{
public:
    hgeRenderer()
    {
        hge = hgeCreate(HGE_VERSION);
    }
    virtual ~hgeRenderer()
    {
        hge->Release();
    }
    virtual void render(const iClipState &cs);
protected:
    static HGE *hge;
};
class hgeBitStream : public iBitStream
{
public:
    hgeBitStream() : data(0), size(0)
    {
        hge = hgeCreate(HGE_VERSION);
    }
    virtual ~hgeBitStream()
    {
        if (data)
        {
            hge->Resource_Free(data);
            size = 0;
        }
        hge->Release();
    }
    virtual bool write(const void *buf, size_t len)
    {
        buf, len;
        assert(0 && "not allowed to write");
        return false;
    }
    virtual bool read(void *buf, size_t len)
    {
        if (len > size - cpos)
            return false;
        if (len > 0)
        {
            memcpy(buf, data + cpos, len);
            cpos += (DWORD)len;
        }
        return true;
    }
    /// seek to position
    virtual void seek(size_t pos)
    {
        cpos = min(DWORD(pos), size);
    }
    /// seek with an offset
    virtual void seekOffset(int offset)
    {
        seek(cpos + offset);
    }
    virtual size_t length() const
    {
        return size;
    }
    virtual size_t tell() const
    {
        return cpos;
    }
    void loadBitStream(const char *name)
    {
        if (data)
        {
            hge->Resource_Free(data);
            size = 0;
        }
        data = (char *)hge->Resource_Load(name, &size);
        cpos = 0;
    }

protected:
    char *data;
    DWORD size;
    DWORD cpos;
    static HGE *hge;
};
class hgeCurvedAniSystem : public iSystem
{
public:
    hgeCurvedAniSystem()
    {
        hge = hgeCreate(HGE_VERSION);
    }
    virtual ~hgeCurvedAniSystem()
    {
        // MUST call this here
        iSystem::release();
        hge->Release();
    }
    virtual iBitStream *loadStream(const char *name)
    {
        hgeBitStream *bitStream = new hgeBitStream;
        bitStream->loadBitStream(name);
        return bitStream;
    }
    virtual iCurveDataSet *createCurveDataSet(const char *name);
    virtual iCurveInterpolater *createCurveInterpolater(const char *name);
    virtual void release(iBitStream *bs)
    {
        delete bs;
    }
    virtual void release(iCurveDataSet *cds)
    {
        delete cds;
    }
    virtual void release(iCurveInterpolater *ci)
    {
        delete ci;
    }
    virtual iRenderer *getRenderer()
    {
        return &renderer;
    }
    virtual float getTime()
    {
        return hge->Timer_GetTime();
    }

    static iSystem* GetInstance()
    {
        return s_Instance;
    }
protected:
    virtual iTexture *loadTexture(const char *name);
    virtual void release(cAni::iTexture *tex);
    virtual void release(cAni::iClipState *cs);
    virtual iClipState *createClipState();
    static HGE *hge;
    hgeRenderer renderer;
};

#endif
