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

#include "filter.h"
#include "filterwidget.h"
#include "../imgproc/lut.h"
#include "../imgproc/types.h"
#include "../imgproc/colorconversion.h"
#include "../misc/util.h"

Filter::Filter() :
    mImage()
{
}

Filter::~Filter()
{

}

ImageFilter *Filter::clone()
{
    Filter * f = new Filter();
    f->mImage = mImage;
    return f;
}

extern "C" QHash<QString, QString> getAnitoolsPluginInfo();
QHash<QString, QString> Filter::info()
{
    return getAnitoolsPluginInfo();
}

QImage Filter::process(const QImage &inputImage)
{
    if (inputImage.isNull() || inputImage.format() != QImage::Format_ARGB32)
        return inputImage;

    if (mImage.isNull())
        return inputImage;

    QImage bg = mImage.scaled(inputImage.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    register int w = inputImage.width(), h = inputImage.height(), mean = 0, totalPixels = w * h;
    register HSL * bitsHSL = (HSL *)malloc(w * h * sizeof(HSL)), * bitsHSLsl;
    register HSL * bitsHSLbg = (HSL *)malloc(w * h * sizeof(HSL)), * bitsHSLbgsl;

    convertBGRToHSL(inputImage.bits(), (unsigned char *)bitsHSL, w * h);
    convertBGRToHSL(bg.bits(), (unsigned char *)bitsHSLbg, w * h);

    bitsHSLbgsl = bitsHSLbg;
    while (totalPixels--)
    {
        mean += bitsHSLbgsl->l;
        bitsHSLbgsl++;
    }
    mean /= w * h;

    bitsHSLsl = bitsHSL;
    bitsHSLbgsl = bitsHSLbg;
    totalPixels = w * h;
    while (totalPixels--)
    {
        bitsHSLsl->l = AT_clamp(0, lut02[bitsHSLsl->l][bitsHSLbgsl->l] * mean / 255, 255);;
        bitsHSLsl++;
        bitsHSLbgsl++;
    }

    QImage i = inputImage.copy();
    convertHSLToBGR((unsigned char *)bitsHSL, i.bits(), w * h);
    free(bitsHSL);
    free(bitsHSLbg);

    return i;
}

bool Filter::loadParameters(QSettings &s)
{
    QVariant v;

    v = s.value("image", QImage());
    if (!v.isValid() || !v.canConvert<QImage>())
        return false;

    setImage(v.value<QImage>());

    return true;
}

bool Filter::saveParameters(QSettings &s)
{
    s.setValue("image", mImage);
    return true;
}

QWidget *Filter::widget(QWidget *parent)
{
    FilterWidget * fw = new FilterWidget(parent);

    fw->setImage(mImage);
    connect(this, SIGNAL(imageChanged(QImage)), fw, SLOT(setImage(QImage)));
    connect(fw, SIGNAL(imageChanged(QImage)), this, SLOT(setImage(QImage)));
    return fw;
}

void Filter::setImage(const QImage &i)
{
    if (i == mImage)
        return;
    if (i.format() != QImage::Format_ARGB32)
        mImage = i.convertToFormat(QImage::Format_ARGB32);
    else
        mImage = i;
    emit imageChanged(i);
    emit parametersChanged();
}