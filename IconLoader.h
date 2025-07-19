#ifndef ICONLOADER_H
#define ICONLOADER_H
#include <QIcon>

enum class Theme {
    Dark, Light
};

// enumeration that allows access icons comfortably through Icon::
enum class Icon {
    Add,
    Bitcoin,
    Card,
    Close,
    Color,
    Copy,
    Create,
    Crypto,
    Duplicate,
    Edit,
    Entry,
    Game,
    House,
    Key,
    Language,
    Link,
    Money,
    Net,
    Office,
    Open,
    OpenBrowser,
    Pc,
    Programming,
    Settings,
    Trash,
    User,
    Save,
    Dice,
    Eye,
    EyeClosed
};

class IconLoader {
public:
static QString getIconName(const Icon &ico) {
    QString type;

    switch(ico) {
    case(Icon::Add):
        type = "add";
        break;
    case(Icon::Card):
        type = "card";
        break;
    case(Icon::Close):
        type = "close";
        break;
    case(Icon::Color):
        type = "color";
        break;
    case(Icon::Copy):
        type = "copy";
        break;
    case(Icon::Create):
        type = "create";
        break;
    case(Icon::Crypto):
        type = "crypto";
        break;
    case(Icon::Duplicate):
        type = "duplicate";
        break;
    case(Icon::Edit):
        type = "edit";
        break;
    case(Icon::Entry):
        type = "entry";
        break;
    case(Icon::Game):
        type = "game";
        break;
    case(Icon::House):
        type = "house";
        break;
    case(Icon::Key):
        type = "key";
        break;
    case(Icon::Language):
        type = "language";
        break;
    case(Icon::Link):
        type = "link";
        break;
    case(Icon::Money):
        type = "money";
        break;
    case(Icon::Net):
        type = "net";
        break;
    case(Icon::Office):
        type = "office";
        break;
    case(Icon::Open):
        type = "open";
        break;
    case(Icon::OpenBrowser):
        type = "openBrowser";
        break;
    case(Icon::Pc):
        type = "pc";
        break;
    case(Icon::Programming):
        type = "programming";
        break;
    case(Icon::Settings):
        type = "settings";
        break;
    case(Icon::Trash):
        type = "trash";
        break;
    case(Icon::User):
        type = "user";
        break;
    case(Icon::Save):
        type = "save";
        break;
    case(Icon::Dice):
        type = "dice";
        break;
    default:
        break;
    }

    return type;
}

static QIcon getIcon(const Icon &ico, const Theme theme) {
    QString type;

    switch(ico) {
    case(Icon::Add):
        type = "add";
        break;
    case(Icon::Card):
        type = "card";
        break;
    case(Icon::Close):
        type = "close";
        break;
    case(Icon::Color):
        type = "color";
        break;
    case(Icon::Copy):
        type = "copy";
        break;
    case(Icon::Create):
        type = "create";
        break;
    case(Icon::Crypto):
        type = "crypto";
        break;
    case(Icon::Duplicate):
        type = "duplicate";
        break;
    case(Icon::Edit):
        type = "edit";
        break;
    case(Icon::Entry):
        type = "entry";
        break;
    case(Icon::Game):
        type = "game";
        break;
    case(Icon::House):
        type = "house";
        break;
    case(Icon::Key):
        type = "key";
        break;
    case(Icon::Language):
        type = "language";
        break;
    case(Icon::Link):
        type = "link";
        break;
    case(Icon::Money):
        type = "money";
        break;
    case(Icon::Net):
        type = "net";
        break;
    case(Icon::Office):
        type = "office";
        break;
    case(Icon::Open):
        type = "open";
        break;
    case(Icon::OpenBrowser):
        type = "openBrowser";
        break;
    case(Icon::Pc):
        type = "pc";
        break;
    case(Icon::Programming):
        type = "programming";
        break;
    case(Icon::Settings):
        type = "settings";
        break;
    case(Icon::Trash):
        type = "trash";
        break;
    case(Icon::User):
        type = "user";
        break;
    case(Icon::Save):
        type = "save";
        break;
    case(Icon::Dice):
        type = "dice";
        break;
    default:
        break;
    }
    if (theme == Theme::Dark) return QIcon(":/icons/dark/resources/icons/dark/"+type+".png");
    return QIcon(":/icons/light/resources/icons/light/"+type+".png");
}
};

#endif // ICONLOADER_H
