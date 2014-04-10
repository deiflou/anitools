#include <opencv2/imgproc.hpp>
#include <QDebug>

#include "filter.h"
#include "filterwidget.h"
#include "../imgproc/lut.h"
#include "../imgproc/types.h"

Filter::Filter() :
    mMode(0),
    mImage()
{
}

Filter::~Filter()
{

}

ImageFilter *Filter::clone()
{
    Filter * f = new Filter();
    f->mMode = mMode;
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

    QImage i, bg;
    cv::Mat msrc, mdst;
    register BGRA * bits1, * bits2, *bits3;
    register int totalPixels;

    switch (mMode)
    {
    case 0:
        bg = QImage(inputImage.width(), inputImage.height(), QImage::Format_ARGB32);
        msrc = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4, (void*)inputImage.bits());
        mdst = cv::Mat(bg.height(), bg.width(), CV_8UC4, bg.bits());
        cv::morphologyEx(msrc, mdst, cv::MORPH_CLOSE,
                         cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(10, 10)));
        break;
    case 1:
        if (mImage.isNull())
            return inputImage;
        bg = mImage.scaled(inputImage.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        break;
    default:
        return inputImage;
    }

    i = QImage(inputImage.width(), inputImage.height(), QImage::Format_ARGB32);
    totalPixels = bg.width() * bg.height();
    bits1 = (BGRA*)bg.bits();
    bits2 = (BGRA*)inputImage.bits();
    bits3 = (BGRA*)i.bits();
    while (totalPixels--)
    {
        bits3->r = qMin<int>(lut02[bits2->r][bits1->r], 255);
        bits3->g = qMin<int>(lut02[bits2->g][bits1->g], 255);
        bits3->b = qMin<int>(lut02[bits2->b][bits1->b], 255);
        bits3->a = bits2->a;
        bits1++;
        bits2++;
        bits3++;
    }

    return i;
}

bool Filter::loadParameters(QSettings &s)
{
    QString m;
    int mode;
    QVariant v;

    m = s.value("mode", "auto").toString();
    if (m == "auto")
        mode = 0;
    else if (m == "fromimage")
        mode = 1;
    else
        return false;

    if (mode == 1)
    {
        v = s.value("image", QImage());
        if (!v.isValid() || !v.canConvert<QImage>())
            return false;
    }

    setMode(mode);
    if (mode == 1)
        setImage(v.value<QImage>());

    return true;
}

bool Filter::saveParameters(QSettings &s)
{
    s.setValue("mode", mMode == 0 ? "auto" : "fromimage");
    if (mMode == 1)
        s.setValue("image", mImage);
    return true;
}

QWidget *Filter::widget(QWidget *parent)
{
    FilterWidget * fw = new FilterWidget(parent);

    fw->setMode(mMode);
    fw->setImage(mImage);
    connect(this, SIGNAL(modeChanged(int)), fw, SLOT(setMode(int)));
    connect(this, SIGNAL(imageChanged(QImage)), fw, SLOT(setImage(QImage)));
    connect(fw, SIGNAL(modeChanged(int)), this, SLOT(setMode(int)));
    connect(fw, SIGNAL(imageChanged(QImage)), this, SLOT(setImage(QImage)));
    return fw;
}

void Filter::setMode(int m)
{
    if (m == mMode)
        return;
    mMode = m;
    emit modeChanged(m);
    emit parametersChanged();
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
