#ifndef CELLDELEGATE_H
#define CELLDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class CellDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        Q_UNUSED(locale);
        QString text = value.toString();
        return QString("*").repeated(text.length());
    }
};




#endif // CELLDELEGATE_H
