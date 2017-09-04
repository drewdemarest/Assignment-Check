#include "gm_i18nlocale.h"
#include "gm_organization.h"

GM_I18NLocale::GM_I18NLocale()
{
    qDebug() << key.getKey();
    qDebug() << key.getVariantValue();
    QJsonObject tobj;
    //tobj.insert(key.getKey(), key.getJsonValue());
    //tobj.insert(language.getKey(), testObj);

    digitalGrouping = JsonType<QString>{"digitalGrouping", testLoc};
    tobj.insert(digitalGrouping.getKey(), digitalGrouping.getJsonValue());
    qDebug() << QJsonDocument(tobj).toJson(QJsonDocument::Indented);

    shared_ptr<QString> digitalGroupingVal = make_shared<QString>(QString("New Val!"));
    QString digitalGroupingKey("New Key!");
    digitalGrouping.setKeyVal(digitalGroupingKey, digitalGroupingVal);

    tobj.insert(digitalGrouping.getKey(), digitalGrouping.getJsonValue());
    qDebug() << QJsonDocument(tobj).toJson(QJsonDocument::Indented);
}

GM_I18NLocale::~GM_I18NLocale()
{
}
