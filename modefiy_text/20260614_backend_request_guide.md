# 后台数据对接说明

当前项目仍然以 `etc/` 目录下的 mock JSON 作为数据来源。后续接后台时，建议保持现有 UI、Model、字段结构不大改，只把“数据来源”从本地 JSON 替换成 `ApiService` 请求后台。

## 一、需要请求后台的位置

可以全局搜索：

```text
TODO(backend-request)
```

这些标记就是后续需要接后台接口的位置。

### 1. 列表数据

- `core/productiontaskmodel.cpp`
  - 生产任务列表
  - 当前 mock 文件：`etc/production_tasks.json`

- `core/repairstationmodel.cpp`
  - 维修站列表
  - 当前 mock 文件：`etc/repair_station_rows.json`

- `core/processstationmodel.cpp`
  - 工序站点右侧表格数据
  - 当前 mock 文件：
    - `process_station_material_check.json`
    - `process_station_route.json`
    - `process_station_materials.json`
    - `process_station_tool_equipment.json`
    - `process_station_replacement_materials.json`

### 2. 提交和操作数据

- `ui/productiontaskpage.cpp`
  - 点击“开工”后，需要请求后台开工接口

- `ui/processstationpage.cpp`
  - NG 异常上报，需要请求后台创建异常处理单

- `ui/processstationrightpanel.cpp`
  - 右侧工艺信息保存，需要请求后台保存接口

- `ui/repairjudgepage.cpp`
  - 维修判定草稿保存
  - 维修判定正式提交

- `core/processstationmodel.cpp`
  - 扫入物料标签码后，请求后台解析 EPR、批次号等信息
  - 扫入物料 SN 后，请求后台解析批次、序列号并校验是否可绑定

## 二、推荐接入流程

### 1. 在 `ApiDefinition` 增加接口地址

文件：

```text
network/apidefinition.h
network/apidefinition.cpp
```

示例：

```cpp
static QUrl productionTasksUrl();
```

```cpp
QUrl ApiDefinition::productionTasksUrl()
{
    return buildUrl("/api/production/tasks");
}
```

这样接口路径集中维护，后面后端路径变化时只改一处。

### 2. 在 `ApiService` 增加业务请求方法

文件：

```text
network/apiservice.h
network/apiservice.cpp
```

示例：

```cpp
void loadProductionTasks(int page,
                         int pageSize,
                         const QJsonObject& filters);
```

同时增加返回信号：

```cpp
signals:
    void productionTasksLoaded(bool success,
                               const QVector<QVariantMap>& rows,
                               int total,
                               const QString& message);
```

页面不要直接 new `QNetworkAccessManager`。页面只调用 `ApiService`，网络细节统一放在 `network/` 目录。

### 3. 在 `ApiService` 里发送请求

推荐请求体：

```json
{
  "page": 1,
  "pageSize": 50,
  "status": "未开工",
  "keyword": "MO202606"
}
```

推荐后台返回：

```json
{
  "success": true,
  "code": 0,
  "message": "ok",
  "data": {
    "total": 50,
    "page": 1,
    "pageSize": 50,
    "rows": []
  }
}
```

当前 `HttpClient::parseReply()` 已经按 `success/code/message/data` 这种结构解析。

### 4. 把 `data.rows` 转成 `QVector<QVariantMap>`

后台返回的每一行字段名，尽量和当前 mock JSON 保持一致。这样现有表格列配置不用大改。

如果后台字段名和前端不同，建议在 `ApiService` 里统一转换，不要把字段转换逻辑散落到 UI 页面里。

### 5. 页面收到数据后刷新表格

当前表格刷新入口仍然是：

```cpp
model->setRows(rows);
```

接后台分页后，`rows` 应该只包含当前页的数据，比如 50 条，而不是后台所有数据。

## 三、分页规则

当前已经有本地分页链路：

```text
BaseTableModel -> FieldFilterProxyModel -> PaginationProxyModel -> QTableView
```

现在是：

```text
一次读取全部 mock JSON -> 本地筛选 -> 本地分页
```

接后台后应该改成：

```text
点击页码 -> ApiService 请求 page/pageSize -> 后台返回当前页 rows -> setRows(rows)
```

注意：后端接入后不要再一次性请求全部数据回来做本地分页。

到真正接后台分页时，`PaginationProxyModel` 可以有两种处理方式：

- 改成“服务端分页模式”，按钮点击时发请求
- 或者后台分页表格绕过本地分页代理，只用分页按钮触发请求

## 四、建议接口清单

```text
POST /api/production/tasks
POST /api/production/tasks/start
POST /api/repair/station/rows
POST /api/process-station/material-check
POST /api/process-station/route
POST /api/process-station/materials
POST /api/process-station/tool-equipment
POST /api/process-station/replacement-materials
POST /api/process-station/material-label/resolve
POST /api/process-station/material-sn/resolve
POST /api/process-station/process-info/save
POST /api/process-station/ng
POST /api/repair/judge/draft/save
POST /api/repair/judge/submit
```

## 五、字段约定

第一版后台建议直接使用当前 mock JSON 的字段名。

生产任务字段示例：

```text
taskNo
productModel
erpCode
productName
lineNo
workCount
routeName
startTime
finishTime
priority
status
```

维修站字段示例：

```text
exceptionHandleNo
exceptionHandleNoLink
taskNo
reworkTaskNo
productSN
abnormalType
abnormalPhenomenon
abnormalImage
status
handleMethod
```

工序物料字段示例：

```text
productSN
materialCode
materialName
materialModel
quantity
materialSN
materialBatch
serialNumber
```

## 六、接口接入示例

### 生产任务列表

`ApiDefinition`：

```cpp
QUrl ApiDefinition::productionTasksUrl()
{
    return buildUrl("/api/production/tasks");
}
```

`ApiService` 请求：

```cpp
void ApiService::loadProductionTasks(int page,
                                     int pageSize,
                                     const QJsonObject& filters)
{
    QJsonObject body = filters;
    body.insert("page", page);
    body.insert("pageSize", pageSize);

    const QString requestId =
        m_httpClient.postJson(ApiDefinition::productionTasksUrl(), body);
    m_requestTypes.insert(requestId, RequestType::ProductionTasks);
}
```

返回处理：

```cpp
void ApiService::handleProductionTasksResponse(const ApiResponse& response)
{
    QVector<QVariantMap> rows;
    int total = 0;

    if (response.success)
    {
        const QJsonObject data = response.data.toObject();
        total = data.value("total").toInt();
        const QJsonArray array = data.value("rows").toArray();

        for (const QJsonValue& value : array)
            rows.append(value.toObject().toVariantMap());
    }

    emit productionTasksLoaded(response.success,
                               rows,
                               total,
                               response.message);
}
```

页面收到信号后：

```cpp
model->setRows(rows);
```

## 七、注意事项

- 大列表必须由后台分页、筛选、排序。
- 图片和 PDF 不要放进主列表 JSON，列表只返回 `fileId` 或 `url`。
- 导出报表建议走后台导出接口，不要客户端拉全量数据再导出。
- PASS、NG、维修判定、右侧保存这些动作，提交成功后应由后台返回最新状态，前端再刷新对应列表或当前行。
- 后台字段如果和前端不同，统一在 `ApiService` 里转换。
