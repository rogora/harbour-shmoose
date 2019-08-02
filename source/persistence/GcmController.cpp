#include "GcmController.h"
#include "Database.h"

#include <QSqlRecord>
#include <QSqlError>
#include <QDateTime>
#include <QSqlQuery>

#include <QDebug>

GcmController::GcmController(QObject *parent) : QSqlTableModel(parent)
{
}

GcmController::GcmController(Database *db, QObject *parent) :
    QSqlTableModel(parent, *(db->getPointer())), database_(db)
{
}

bool GcmController::setup()
{
    bool returnValue = true;

    setEditStrategy(QSqlTableModel::OnRowChange);
    setTable(Database::sqlGcmName_);
    setSort(3, Qt::DescendingOrder); // timestamp
    if (!select())
    {
        qDebug() << "error on select in GcmController::setup";
        returnValue = false;
    }

    return returnValue;
}

QVariant GcmController::data ( const QModelIndex & requestedIndex, int role ) const
{
    if(requestedIndex.row() >= rowCount())
    {
        return QString("");
    }

    if(role < Qt::UserRole)
    {
        QVariant data = QSqlTableModel::data(requestedIndex, role);
        return data;
    }

    QModelIndex modelIndex = this->index(requestedIndex.row(), role - Qt::UserRole - 1 );
    QVariant editData = QSqlQueryModel::data(modelIndex, Qt::EditRole);

    return editData;
}

// Role names are set to whatever your db table's column names are.
void GcmController::generateRoleNames()
{
    roles_.clear();
    for (int i = 0; i < columnCount(); i++)
    {
        roles_[Qt::UserRole + i + 1] = QVariant(headerData(i, Qt::Horizontal).toString()).toByteArray();
    }
}

QHash<int, QByteArray> GcmController::roleNames() const
{
    return roles_;
}

void GcmController::setTable ( const QString &table_name )
{
    QSqlTableModel::setTable(table_name);
    generateRoleNames();
}

void GcmController::markGroupMessageReceivedByMember(const QString &msgId, const QString& groupChatMember)
{
    int status = getMsgStatus(msgId);
    if (status < 1) // only if msg is not already marked as displayed
    {
        markGroupMessage(msgId, groupChatMember, 1);
    }
}

void GcmController::markGroupMessageDisplayedByMember(const QString& msgId, const QString& groupChatMember)
{
    markGroupMessage(msgId, groupChatMember, 2);
}

void GcmController::markGroupMessage(const QString& msgId, const QString& groupChatMember, int state)
{
    /*
     * REPLACE INTO positions (title, min_salary)
VALUES
 ('Full Stack Developer', 140000);
     */

    QSqlQuery query(*(database_->getPointer()));

    if (! query.exec("REPLACE INTO " + Database::sqlGcmName_ +
                     " (" + Database::sqlId_ + ", " + Database::sqlGcmChatMemberName_ + ", " + Database::sqlTimestamp_ + ", " + Database::sqlGcmState_ + ") " +
                     "VALUES " +
                     "('" + msgId + "', '" + groupChatMember + "', '" + QString::number(QDateTime::currentDateTime().toTime_t()) + "', " + QString::number(state) + ");"
                     ))
    {
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().text();
    }
    else
    {
        // update the model with the changes of the database
        if (select() != true)
        {
            qDebug() << "sql error in GcmController::markGroupMessage";
        }
    }
}

void GcmController::setFilterOnMsg(QString const &msg)
{
    setFilter(Database::sqlId_ + " = '" + msg + "'");

    if (!select())
    {
        qDebug() << "error on select in GcmController::setFilterOnMsg";
    }
}

int GcmController::getMsgStatus(const QString& msgId)
{
    int msgState = 0;

    QSqlQuery query(*(database_->getPointer()));
    if (! query.exec("SELECT " + Database::sqlGcmState_ + " FROM " + Database::sqlGcmName_ + " WHERE " + Database::sqlId_ + " = \"" + msgId + "\""))
    {
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().text();
    }
    else
    {
        while (query.next())
        {
            msgState = query.value(0).toInt();
            break;
        }
    }

    qDebug() << "found msg state: " << msgState << "for msg id " << msgId;

    return msgState;
}

void GcmController::printSqlError()
{
    qDebug() << this->lastError().databaseText();
    qDebug() << this->lastError().driverText();
    qDebug() << this->lastError().text();
}
