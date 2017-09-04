#include <QJsonObject>
#include <QtCore>
#include <memory>

using namespace std;

template <class T>
class JsonType
{
public:
    JsonType()
    {construct();}

    JsonType(const QString &qs):
        key(qs)
    {construct();}

    JsonType(const QString &qs, const T &t):
        key(qs), value(new T(t))
    {construct();}

    JsonType(const QString &qs, const QJsonValue &jv):
        key(qs), jsonValue(jv)
    {construct();}

    JsonType(const QString &qs, const QVariant &qv):
        key(qs), variantValue(qv)
    {construct();}

    //Special ctor, will iterate through a QJsonObject
    //and set values if it finds a key match.
    JsonType(const QString &qs, const QJsonObject &qjo):
        key(qs), jsonValue(qjo.value(qs))
    {construct();}

    inline void setKey(QString &qs);

    inline void setValue(shared_ptr<T> t);

    inline void setKeyVal(QString &qs, shared_ptr<T> t)
    {setKey(qs); setValue(t);}

    inline QString getKey();
    inline shared_ptr<T> getValue();
    inline QJsonValue getJsonValue();
    inline QVariant getVariantValue();

private:
    //Construct happisly exists within each constructor.
    //This can be overloaded or not because of its inline status.
    inline void construct();

    QString         key;
    shared_ptr<T>   value;
    QVariant        variantValue;
    QJsonValue      jsonValue;
};



template <class T>
inline void JsonType<T>::construct()
{
    if(!key.isEmpty())
    {
        //Determine which type is not null and populate other values.
        //Once everything is populated, return.
        if(value)
        {
            variantValue = QVariant(*value.get());
            jsonValue = jsonValue.fromVariant(variantValue);
            //qDebug() << key  <<  jsonValue << variantValue;
            if(variantValue.isNull())
                qDebug() << "Warning, 1 variantValue did not properly map";

            if(jsonValue.isNull())
                qDebug() << "Warning, 1 jsonValue did not properly map";
            return;
        }

        if(!variantValue.isNull())
        {
            value.reset(new T(variantValue.value<T>()));
            jsonValue = jsonValue.fromVariant(variantValue);

            if(!value)
                qDebug() << "Warning, 2 shared_ptr value did not properly map" << *value.get();

            if(jsonValue.isNull())
                qDebug() << "Warning, 2 jsonValue did not map!";

            return;
        }
        if(!jsonValue.isNull() && !jsonValue.isUndefined())
        {
            variantValue = jsonValue.toVariant().value<T>();
            value.reset(new T(variantValue.value<T>()));

            //An attempt to recast jsonValue with the correct type using
            //variant's ability to typecast.
            jsonValue = jsonValue.fromVariant(variantValue);

            if(!value)
                qDebug() << "Warning 3 shared_ptr value did not properly map" << *value.get();

            if(variantValue.isNull())
                qDebug() << "Warning 3 jsonValue did not map!";

            return;
        }
        else
        {
            qDebug() << "Null obj" << key;
            return;
        }

    }
    else
    {

    }
}

template <class T>
inline void JsonType<T>::setKey(QString &qs)
{
    key = qs;
}

template <class T>
inline void JsonType<T>::setValue(shared_ptr<T> t)
{
    if(t)
    {
        value = t;
        variantValue = QVariant(*value.get());
        jsonValue = jsonValue.fromVariant(variantValue);
    }
    if(!value)
        qDebug() << "Warning, value is null.";

    if(variantValue.isNull())
        qDebug() << "Warning, 1 variantValuse did not properly map";

    if(jsonValue.isNull())
        qDebug() << "Warning, 1 jsonValue did not properly map";
    return;
}

template <class T>
inline QString JsonType<T>::getKey()
{
    return key;
}

template <class T>
inline shared_ptr<T> JsonType<T>::getValue()
{
    return value;
}

template <class T>
inline QJsonValue JsonType<T>::getJsonValue()
{
    return jsonValue;
}

template <class T>
inline QVariant JsonType<T>::getVariantValue()
{
    return variantValue;
}
