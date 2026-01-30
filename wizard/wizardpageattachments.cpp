#include "wizardpageattachments.h"
#include "ui_wizardpageattachments.h"
#include "wizard/models/supplierattachmentactiondelegate.h"
#include "wizard/models/supplierattachmentsmodel.h"

WizardPageAttachments::WizardPageAttachments(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageAttachments)
{
    ui->setupUi(this);

    m_attachmentsModel = new SupplierAttachmentsModel(this);
    ui->tableViewAttachmentMapping->setModel(m_attachmentsModel);

    ui->tableViewAttachmentMapping->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableViewAttachmentMapping->setItemDelegateForColumn(SupplierAttachmentsModel::Col_Action, new SupplierAttachmentActionDelegate(ui->tableViewAttachmentMapping));
}

void WizardPageAttachments::setSelectedPart(SupplierPart *newSelectedPart)
{
    m_attachmentsModel->setPart(newSelectedPart);
}

WizardPageAttachments::~WizardPageAttachments()
{
    delete ui;
}

QString WizardPageAttachments::summary() const
{
    if (!m_attachmentsModel->hasSaveable())
        return QString();
    QString ret = tr("<b>Save attachments</b><br>");
    for (int i = 0; i<m_attachmentsModel->rowCount(); i++) {
        if (m_attachmentsModel->data(m_attachmentsModel->index(i, SupplierAttachmentsModel::Col_Action), SupplierAttachmentsModel::Role_Save).toBool()) {
            QString comment = m_attachmentsModel->data(m_attachmentsModel->index(i, SupplierAttachmentsModel::Col_Comment)).toString();
            if (!comment.isEmpty()) {
                comment = tr(" (%1)").arg(comment);
            }
            ret.append(tr("<li>%1%2</li>").arg(
                m_attachmentsModel->data(m_attachmentsModel->index(i, SupplierAttachmentsModel::Col_FileName)).toString(),
                comment));
        }
    }
    ret.append("</ul>");
    return ret;
}

void WizardPageAttachments::on_tableViewAttachmentMapping_clicked(const QModelIndex &index)
{
    if (index.column() == SupplierAttachmentsModel::Col_FileName) {
        auto fn = m_attachmentsModel->data(m_attachmentsModel->index(index.row(), SupplierAttachmentsModel::Col_FileName), Qt::DisplayRole).toString();
        fn = QDir::tempPath() + QDir::separator() + fn;
        QFileInfo fi(fn);
        if (fi.exists() && fi.size() == m_attachmentsModel->data(index, SupplierAttachmentsModel::Role_Size)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
        } else {
            QNetworkRequest req;
            req.setUrl(m_attachmentsModel->data(index, SupplierAttachmentsModel::Role_Url).toUrl());
            req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
            // TODO make user agent settable
            req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));

            if (!m_networkAccessManager)
                m_networkAccessManager = new QNetworkAccessManager(this);

            auto reply = m_networkAccessManager->get(req);
            connect(reply, &QNetworkReply::finished, this, [=]() {
                QFile file(fn);
                if (file.open(QFile::WriteOnly)) {
                    file.write(reply->readAll());
                }
                file.close();
                QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
            });
        }
    }
}

