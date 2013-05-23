/*
���ò�ֵ��
�����Լ������µĲ�ֵ��������sin���ߺ�����ֵ��,curvedataset�ڱ������߲�����������Ȼʹ��ʱ��-״̬���
��ʱ���ɱ�ʾ����ʱ��ı仯��sin���ߵĲ���Ҳ�ڱ仯����ģ�������񶯡�

*/

#ifndef CANI_INTERPOLATERS_H
#define CANI_INTERPOLATERS_H

#include "curvedani.h"
#include "caImage.h"
#include "caBspline.h"

#pragma warning(push)
#pragma warning(disable:4554)
namespace cAni
{
    // ��ֵ��
    /// Խ���Ͳ�ֵ��
    template<class T>
    class StepInterpolater : public iCurveInterpolater
    {
    public:
        virtual const char *getTypeName() const;
        virtual bool getSample(StateId sid, const iCurveDataSet &cds, int time, void *result) const
        {
            sid;

            size_t lo = 0, hi = cds.getDataCount();
            if (hi <= 0)
                return false;

            if (time <= cds.getTime(0))
            {
                *(T*)result = *(T*)cds.getData(0);
                return true;
            }
            if (time >= cds.getTime(hi - 1))
            {
                *(T*)result = *(T*)cds.getData(hi - 1);
                return true;
            }
            while(lo + 1 < hi)
            {
                size_t id = lo + hi >> 1;
                if (cds.getTime(id) <= time)
                    lo = id;
                else
                    hi = id;
            }
            assert(lo + 1 == hi);
            *(T*)result = *(T*)cds.getData(lo);
            return true;
        }
    };
    /// ���Բ�ֵ��
    template<class T>
    class LinearInterpolater : public iCurveInterpolater
    {
    public:
        virtual const char *getTypeName() const;
        virtual bool getSample(StateId sid, const iCurveDataSet &cds, int time, void *result) const
        {
            sid;

            size_t lo = 0, hi = cds.getDataCount();
            if (hi <= 0)
                return false;

            if (time <= cds.getTime(0))
            {
                *(T*)result = *(T*)cds.getData(0);
                return true;
            }
            if (time >= cds.getTime(hi - 1))
            {
                *(T*)result = *(T*)cds.getData(hi - 1);
                return true;
            }
            assert(hi >= 2);
            while(lo + 1 < hi)
            {
                size_t id = lo + hi >> 1;
                if (cds.getTime(id) <= time)
                    lo = id;
                else
                    hi = id;
            }
            assert(lo + 1 == hi && lo + 1 != cds.getDataCount());
            const T &t1 = *(const T*)cds.getData(lo);
            const T &t2 = *(const T*)cds.getData(lo + 1);
            float d = float(cds.getTime(lo + 1) - cds.getTime(lo));
            d = (time - cds.getTime(lo)) / d;
            *(T*)result = t1 + (t2 - t1) * d;
            return true;
        }
    };

    template<class T>
    class BsplineInterpolater : public iCurveInterpolater
    {
    public:
        virtual const char *getTypeName() const;
        virtual bool getSample(StateId sid, const iCurveDataSet &cds, int time, void *result) const
        {
            sid;

            size_t lo = 0, hi = cds.getDataCount();
            if (hi <= 3)
                return false;

            int time1 = cds.getTime(1);
            int time2 = cds.getTime(hi - 2);
            if (time <= time1)
            {
                lo = 2;
                time = time1;
            }
            else if (time >= time2)
            {
                lo = hi - 2;
                time = time2;
            }
            else
            {
                while(lo + 1 < hi)
                {
                    size_t id = lo + hi >> 1;
                    if (cds.getTime(id) <= time)
                        lo = id;
                    else
                        hi = id;
                }
                if (lo > hi - 2)
                    return false;
            }

            float dd = (float)cds.getTime(lo - 1);
            const float (&d4)[4] = spline::bspline_d((time - dd) / (cds.getTime(lo) - dd));

            const T &a = *(const T*)cds.getData(lo - 2);
            const T &b = *(const T*)cds.getData(lo - 1);
            const T &c = *(const T*)cds.getData(lo);
            const T &d = *(const T*)cds.getData(lo + 1);

            *(T*)result = T(a * d4[0] + b * d4[1] + c * d4[2] + d * d4[3]);
            return true;
        }
    };
};
#pragma warning(pop)

#endif//CANI_INTERPOLATERS_H