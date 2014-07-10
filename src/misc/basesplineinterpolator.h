/****************************************************************************
**
** Copyright (C) 2014 Deif Lou
**
** This file is part of Anitools
**
** Anitools is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef ANITOOLS_MISC_BASESPLINEINTERPOLATOR_H
#define ANITOOLS_MISC_BASESPLINEINTERPOLATOR_H

#include "splineinterpolator.h"

namespace anitools {
namespace misc {

class BaseSplineInterpolator : public SplineInterpolator
{

public:
    BaseSplineInterpolator();
    ~BaseSplineInterpolator();

    virtual const SplineInterpolatorKnots &knots();
    virtual const SplineInterpolatorKnot &knot(int i);
    virtual int size();
    virtual bool setKnots(const SplineInterpolatorKnots &k);
    virtual bool setKnot(int i, const SplineInterpolatorKnot &k);
    virtual bool setKnot(int i, double nx, double ny);
    virtual bool setKnot(double x, const SplineInterpolatorKnot &k);
    virtual bool setKnot(double x, double nx, double ny);
    virtual bool addKnot(const SplineInterpolatorKnot &k, bool replace = false);
    virtual bool addKnot(double nx, double ny, bool replace = false);
    virtual bool removeKnot(double x);
    virtual bool removeKnot(int i);

    virtual double f(double x) = 0;

    virtual ExtrapolationMode floorExtrapolationMode();
    virtual ExtrapolationMode ceilExtrapolationMode();
    virtual void setExtrapolationMode(ExtrapolationMode f, ExtrapolationMode c);

protected:
    SplineInterpolatorKnots mKnots;
    ExtrapolationMode mFloorExtrapolationMode;
    ExtrapolationMode mCeilExtrapolationMode;
};

}}
#endif // ANITOOLS_MISC_BASESPLINEINTERPOLATOR_H