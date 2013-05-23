#ifndef __LOCUS_H_
#define __LOCUS_H_

#include <cassert>
#include <vector>
#include <algorithm>

#include "curvedani.h"

namespace cAni
{

/// Locus
/// @note   轨迹，一个描述时间-状态的函数
class Locus
{
public:
    struct StreamData
    {
        size_t dsid;
        size_t ipid;
        StateId sid;
    };
    iCurveDataSet *dataset;
    iCurveInterpolater *interpolator;
    StateId sid;
    bool getSample(iClipState &cs, int time) const
    {
        return interpolator->getSample(sid, *dataset, time, cs.getStateAddr(sid));
    }
};


} // cAni
#endif // __LOCUS_H_