#include "inventreestocklocationpathlineedit.h"

#include "inventreesettingsdialog.h"

InventreeStockLocationListModel::InventreeStockLocationListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int InventreeStockLocationListModel::rowCount(const QModelIndex &) const
{
    return m_items.size();
}

QVariant InventreeStockLocationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};
    const auto &item = m_items[index.row()];
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return item.path;
    if (role == IdRole)
        return item.id;
    return {};
}

void InventreeStockLocationListModel::setLocations(const InvenTree::PaginatedLocationList &list)
{
    beginResetModel();
    m_items.clear();
    for (const auto &location : list.getResults())
        m_items.append({location.getPk(), location.getPathstring()});
    endResetModel();
}

InventreeStockLocationPathLineEdit::InventreeStockLocationPathLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    m_api = new InvenTree::StockApi();
    QSettings settings;
    settings.beginGroup("InventTree");
    const auto token = settings.value(InventreeSettingsDialog::KEY_TOKEN).toString();
    settings.endGroup();
    m_api->addHeaders("Authorization", "Token " + token);

    m_model = new InventreeStockLocationListModel(this);
    m_completer = new QCompleter(m_model, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    setCompleter(m_completer);

    connect(this, &QLineEdit::textEdited, this, &InventreeStockLocationPathLineEdit::onTextEdited);
    connect(m_completer, QOverload<const QModelIndex &>::of(&QCompleter::activated), this,
            [this](const QModelIndex &index) { emit locationSelected(index.data(InventreeStockLocationListModel::IdRole).toUInt()); });
    connect(m_api, &InvenTree::StockApi::stockLocationListSignal,
            this, &InventreeStockLocationPathLineEdit::onLocationRetrieved);
}

InventreeStockLocationPathLineEdit::~InventreeStockLocationPathLineEdit()
{
    delete m_api;
}

void InventreeStockLocationPathLineEdit::onTextEdited(const QString &text)
{
    m_pendingText = text.trimmed();
    if (m_pendingText.length() < 2)
        return;
    m_api->stockLocationList(10, {}, {}, {}, {}, {}, {}, 0, {}, {}, true, m_pendingText);
}

void InventreeStockLocationPathLineEdit::onLocationRetrieved(InvenTree::PaginatedLocationList locations)
{
    m_model->setLocations(locations);
    if (!text().isEmpty())
        m_completer->complete();
}
