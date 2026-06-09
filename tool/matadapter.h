#ifndef MATADAPTER_H
#define MATADAPTER_H

#include <QJsonArray>
#include <QString>
#include <QVariantMap>
#include <QVector>

class MatAdapter
{
public:
    /*
     * 使用方法：
     * 1. 表格显示前调用 split(backendJson)。
     *    如果后台数据是 { productSN, materials: [...] } 复合结构，会展开成表格行。
     *    如果后台数据已经是平铺行，也会按 productSN 分组并补充行元数据。
     *
     * 2. 表格提交前优先调用 combine(originalBackendJson, tableRows)。
     *    该接口会以后台原始数据为底，把表格里修改或补充的数据合并回去，
     *    适合保留后台原有 ID、状态、时间等字段。
     *
     * 3. 如果没有原始后台数据，也可以调用 combine(tableRows)。
     *    表格行会重新组合成后台需要的复合 QJsonArray：
     *    [
     *      { "productSN": "...", "materials": [ {...}, {...} ] }
     *    ]
     *
     * 4. 上传前可调用 isCompound(uploadJson) 做格式校验。
     *    用来确认数据是否已经是带 materials 数组的复合结构。
     */
    static bool isCompound(const QJsonArray& data);
    static bool isCompound(const QVector<QVariantMap>& data);

    static QVector<QVariantMap> split(const QJsonArray& compoundData);
    static QVector<QVariantMap> split(const QVector<QVariantMap>& compoundData);

    static QJsonArray combine(const QVector<QVariantMap>& splitRows);
    static QJsonArray combine(const QJsonArray& originalData,
                              const QVector<QVariantMap>& splitRows);

private:
    struct Opt;

    static Opt opt();
    static bool hasItems(const QVariantMap& row, const Opt& opt);
    static QVector<QVariantMap> expandNestedRows(const QVector<QVariantMap>& backendRows,
                                                 const Opt& opt);
    static QVector<QVariantMap> expandFlatRows(const QVector<QVariantMap>& backendRows,
                                               const Opt& opt);
    static void appendGroup(QVector<QVariantMap>* out,
                            const QVector<QVariantMap>& groupRows,
                            const QString& groupKey,
                            int groupIndex,
                            const Opt& opt);
    static QVariantMap itemFromRow(const QVariantMap& row, const Opt& opt);
    static QVariantMap stripMeta(const QVariantMap& row, const Opt& opt);
};

#endif // MATADAPTER_H
