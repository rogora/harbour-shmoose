#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QObject>

class Database;
class MessageController;
class SessionController;
class GcmController;

class Persistence : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MessageController* messageController READ getMessageController NOTIFY messageControllerChanged)
    Q_PROPERTY(SessionController* sessionController READ getSessionController NOTIFY sessionControllerChanged)
    Q_PROPERTY(GcmController* gcmController READ getGcmController NOTIFY gcmControllerChanged)

public:
    explicit Persistence(QObject *parent = 0);
    ~Persistence();

    bool isValid();
    void markMessageDisplayedConfirmedId(QString const &id);
    void markMessageAsDisplayedId(QString const &id);
    void markMessageAsReceivedById(QString const &id);
    void markMessageAsSentById(QString const &id);

    QPair<QString, int> getNewestReceivedMessageIdAndStateOfJid(QString const &jid);

    void openDatabaseForJid(QString const &jid);

    QString getCurrentChatPartner();

    void markGroupMessageReceivedByMember(const QString& msgId, const QString& groupChatMember);
    void markGroupMessageDisplayedByMember(const QString& msgId, const QString& groupChatMember);

signals:
    void messageControllerChanged();
    void sessionControllerChanged();
    void gcmControllerChanged();

public slots:
    void addMessage(bool isGroupMessage, const QString &id, QString const &jid, QString const &resource, QString const &message, const QString &type, unsigned int direction);
    void setCurrentChatPartner(QString const &jid);

private:
    MessageController* getMessageController();
    SessionController* getSessionController();
    GcmController* getGcmController();

    Database *db_;
    MessageController *messageController_;
    SessionController *sessionController_;
    GcmController *gcmController_;

    QString currentChatPartner_;
    bool persistenceValid_;
};

#endif // PERSISTENCE_H
