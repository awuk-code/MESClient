#ifndef APIDEFINITION_H
#define APIDEFINITION_H

#include <QString>
#include <QUrl>

class ApiDefinition
{
public:
    static QString baseUrl();
    static QUrl loginUrl();

private:
    static QUrl buildUrl(const QString& path);
};

#endif // APIDEFINITION_H
