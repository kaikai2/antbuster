#ifndef CANI_CURVED_ANI
#define CANI_CURVED_ANI

#include <cassert>
#include "caRect.h"

class TiXmlElement;
namespace cAni
{
    /// implement the following interfaces in your renderer

    class iTexture
    {
    public:
        iTexture();
        virtual ~iTexture();
        virtual size_t getWidth() const = 0;
        virtual size_t getHeight() const = 0;
        const char *getFilename() const;
    private:
        friend class iSystem;
        void *internalData;
    };
    class iBitStream
    {
    public:
        template<typename T> bool write(const T& var)
        {
            return write(&var, sizeof(T));
        }
        template<typename T> bool writeArray(const T var[], size_t n)
        {
            return write(var, sizeof(T) * n);
        }
        template<typename T> bool read(T& var)
        {
            return read(&var, sizeof(T));
        }
        template<typename T> bool readArray(T var[], size_t n)
        {
            return read(var, sizeof(T) * n);
        }

        virtual bool write(const void *buf, size_t len) = 0;
        virtual bool read(void *buf, size_t len) = 0;
        /// seek to head
        void rewind()
        {
            seek(0);
        }
        /// seek to position
        virtual void seek(size_t pos) = 0;
        /// seek with an offset
        virtual void seekOffset(int offset) = 0;
        virtual size_t length() const = 0;
        virtual size_t tell() const = 0;
    };
    typedef size_t StateId;
    // 数据集
    class iCurveDataSet
    {
    public:
        // unique type name, 
        virtual const char *getTypeName() const = 0;

        virtual size_t getDataCount() const = 0;
        virtual void resize(size_t index) = 0;
        virtual size_t getDataSize() const = 0;

        virtual void setTime(size_t index, int time) = 0;
        virtual void setData(size_t index, const void *value, int time) = 0;
        virtual void setData(size_t index, const void *value) = 0;
        virtual void setDataString(size_t index, const char *valueStr) = 0;

        virtual int getTime(size_t index) const = 0;
        virtual const void *getData(size_t index) const = 0;
        virtual const char *getDataString(size_t index) const = 0;

        virtual bool loadFromBitStream(iBitStream *bs);
        virtual bool saveToBitStream(iBitStream *bs) const;
        bool loadFromXml(const TiXmlElement *elem);
        bool saveToXml(TiXmlElement *elem) const;
    };
    // 插值器
    class iCurveInterpolater
    {
    public:
        // unique type name, 
        virtual const char *getTypeName() const = 0;

        virtual bool getSample(StateId sid, const iCurveDataSet &cds, int time, void *result) const = 0;
    };
    /// 参数
    class iClipState
    {
    public:
        virtual StateId StringToStateId(const char *name) = 0;
        virtual void *getStateAddr(StateId sid) = 0;
        virtual void setState(StateId sid, const void *value) = 0;
        virtual const void *getState(StateId sid) const = 0;
        virtual void reset() = 0;
    };
    class iRenderer
    {
    public:
        virtual void render(const iClipState &cs) = 0;
    };

    class AnimData;
    /// NO need to implement this interface
    class iAnimation
    {
    public:
        typedef size_t AnimId;
        enum
        {
            DefaultAnimId = ~0,
        };
        virtual void setAnimData(const AnimData *pAnimData, AnimId aniId) = 0;
        virtual bool startAnim(int curtime, AnimId aniId = DefaultAnimId) = 0;
        virtual bool checkEnd(int curtime) const = 0;
        virtual void setAnimLoop(bool bLoop) = 0;
        virtual AnimId getCurAnim() const = 0;
        // void setAnimTimer(int time);

        virtual void render(/*System &system, */int time, const Rect *cliprect) const = 0;
    };
    /// NO need to implement this interface
    class iAnimResManager
    {
    public:
        virtual const AnimData* getAnimData(const char *aniFileName) = 0;
        virtual bool saveToBitStream(const AnimData *pAnimData, iBitStream *bs) const = 0;
        virtual bool saveToXml(const AnimData *pAnimData, const char *filename) const = 0;
    };
    class iSystem
    {
    public:
        iSystem() : texmap(0), cs(0)
        {
            assert(s_Instance == 0);
            s_Instance = this;
        }
        virtual ~iSystem();
        iTexture *getTexture(const char *name);
        virtual iBitStream *loadStream(const char *name) = 0;
        virtual iCurveDataSet *createCurveDataSet(const char *name) = 0;
        virtual iCurveInterpolater *createCurveInterpolater(const char *name) = 0;
        iAnimation *createAnimation(size_t animCount = 1);
        iAnimResManager *createAnimResManager();
        iClipState *getClipState()
        {
            if (!cs)
            {
                cs = createClipState();
            }
            return cs;
        }
        virtual void release(iBitStream *bs) = 0;
        virtual void release(iCurveDataSet *cds) = 0;
        virtual void release(iCurveInterpolater *ci) = 0;
        void release(iAnimation *a);
        void release(iAnimResManager *arm);
        virtual iRenderer *getRenderer() = 0;
        virtual float getTime() = 0;

        static iSystem* GetInstance()
        {
            return s_Instance;
        }
    protected:
        void release();
        virtual void release(iTexture *bs) = 0;
        virtual void release(iClipState *cs) = 0;
        virtual iTexture *loadTexture(const char *name) = 0;
        virtual iClipState *createClipState() = 0;
        static iSystem *s_Instance;
        void *texmap;
        mutable iClipState *cs;
    };
}

#endif // CANI_CURVED_ANI