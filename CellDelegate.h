#ifndef CELLDELEGATE_H
#define CELLDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTranslator>

// This is custom delegate that replaces text in hided coumns by * that repeated in text length
class CellDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        return QString("*").repeated(value.toString().size());
    }
};




#endif // CELLDELEGATE_H
