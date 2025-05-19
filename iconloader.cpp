#include "iconloader.h"

QIcon IconLoader::getIcon(const Icon &ico, const QString theme)
{
    QString type;

    switch(ico)
    {
    case(Icon::add):
        type = "add";
        break;
    case(Icon::card):
        type = "card";
        break;
    case(Icon::close):
        type = "close";
        break;
    case(Icon::colorScheme):
        type = "colorScheme";
        break;
    case(Icon::copy):
        type = "copy";
        break;
    case(Icon::create):
        type = "create";
        break;
    case(Icon::crypto):
        type = "crypto";
        break;
    case(Icon::duplicate):
        type = "duplicate";
        break;
    case(Icon::edit):
        type = "edit";
        break;
    case(Icon::entry):
        type = "entry";
        break;
    case(Icon::game):
        type = "game";
        break;
    case(Icon::house):
        type = "house";
        break;
    case(Icon::key):
        type = "key";
        break;
    case(Icon::language):
        type = "language";
        break;
    case(Icon::link):
        type = "link";
        break;
    case(Icon::money):
        type = "money";
        break;
    case(Icon::net):
        type = "net";
        break;
    case(Icon::office):
        type = "office";
        break;
    case(Icon::open):
        type = "open";
        break;
    case(Icon::openBrowser):
        type = "openBrowser";
        break;
    case(Icon::pc):
        type = "pc";
        break;
    case(Icon::programming):
        type = "programming";
        break;
    case(Icon::settings):
        type = "settings";
        break;
    case(Icon::trash):
        type = "trash";
        break;
    case(Icon::user):
        type = "user";
        break;
    case(Icon::save):
        type = "save";
        break;
    default:
        break;
    }
    return QIcon(":/icons/"+theme+"/resources/icons/"+theme+"/"+type+".png");
}
