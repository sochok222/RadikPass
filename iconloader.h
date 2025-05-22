#ifndef ICONLOADER_H
#define ICONLOADER_H

#include <QIcon>

enum class Icon {
    add,
    bitcoin,
    card,
    close,
    color,
    copy,
    create,
    crypto,
    duplicate,
    edit,
    entry,
    game,
    house,
    key,
    language,
    link,
    money,
    net,
    office,
    open,
    openBrowser,
    pc,
    programming,
    settings,
    trash,
    user,
    save
};

class IconLoader
{
public:
    static QIcon getIcon(const Icon &ico, const QString theme);
};

#endif // ICONLOADER_H
