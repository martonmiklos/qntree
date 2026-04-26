#include "inventreecategorypathlineedit.h"

#include "inventreesettingsdialog.h"

InventreeCategoryListModel::InventreeCategoryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int InventreeCategoryListModel::rowCount(const QModelIndex &) const
{
    return m_items.size();
}

QVariant InventreeCategoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return QVariant();

    const auto &item = m_items[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return item.path;

    if (role == IdRole)
        return item.id;

    return QVariant();
}

void InventreeCategoryListModel::setCategories(const InvenTree::PaginatedCategoryList &list)
{
    beginResetModel();

    m_items.clear();

    for (const auto &c : list.getResults())
    {
        m_items.append({
            c.getPk(),
            c.getPathstring()
        });
    }

    endResetModel();
}

InventreeCategoryPathLineEdit::InventreeCategoryPathLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    m_api = new InvenTree::PartApi();
    QSettings settings;
    settings.beginGroup("InventTree");
    auto token = settings.value(InventreeSettingsDialog::KEY_TOKEN).toString();
    settings.endGroup();
    m_api->addHeaders("Authorization", "Token " + token);

    m_model = new InventreeCategoryListModel(this);
    m_completer = new QCompleter(m_model, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setModel(m_model);
    setCompleter(m_completer);

    connect(this, &QLineEdit::textEdited,
            this, &InventreeCategoryPathLineEdit::onTextEdited);

    connect(m_completer,
            QOverload<const QModelIndex &>::of(&QCompleter::activated),
            this,
            [&](const QModelIndex &idx)
            {
                int id = idx.data(InventreeCategoryListModel::IdRole).toInt();
                emit categorySelected(id);
            });

    connect(m_api,
            &InvenTree::PartApi::partCategoryListSignal,
            this,
            &InventreeCategoryPathLineEdit::onCategoryRetrieved);
}

InventreeCategoryPathLineEdit::~InventreeCategoryPathLineEdit()
{
    delete m_api;
}

void InventreeCategoryPathLineEdit::onTextEdited(const QString &text)
{
    m_pendingText = text.trimmed();
    if (m_pendingText.length() < 2)
        return;

    m_api->partCategoryList(
        10, // limit
        ::InvenTree::OptionalParam<bool>(), // cascade
        ::InvenTree::OptionalParam<double>(), // depth
        ::InvenTree::OptionalParam<qint32>(), // exclude_tree
        ::InvenTree::OptionalParam<QString>(), // name
        0, // offset
        ::InvenTree::OptionalParam<QString>(), // ordering
        ::InvenTree::OptionalParam<qint32>(), //&parent,
        ::InvenTree::OptionalParam<bool>(), // &path_detail,
        m_pendingText
        );
}

void InventreeCategoryPathLineEdit::onCategoryRetrieved(InvenTree::PaginatedCategoryList categories)
{
    m_model->setCategories(categories);

    if (!text().isEmpty())
        m_completer->complete();
}
