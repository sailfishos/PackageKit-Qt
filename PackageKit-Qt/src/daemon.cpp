/*
* This file is part of the QPackageKit project
* Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
* Copyright (C) 2010-2011 Daniel Nicoletti <dantti85-pk@yahoo.com.br>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB. If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include <QtSql>
#include <QDebug>

#include "daemon.h"
#include "daemonprivate.h"
#include "daemonproxy.h"

#include "common.h"

#define PENDING_CALL(blurb)                      \
       Q_D(Daemon);                              \
       QDBusPendingReply<> r = d->daemon->blurb; \
       r.waitForFinished();                      \
       d->lastError = r.error();                 \

using namespace PackageKit;

Daemon* Daemon::m_global = 0;

Daemon* Daemon::global()
{
    if(!m_global) {
        m_global = new Daemon(qApp);
    }

    return m_global;
}

Daemon::Daemon(QObject *parent) :
    QObject(parent),
    d_ptr(new DaemonPrivate(this))
{
    Q_D(Daemon);
    d->daemon = new ::DaemonProxy(QLatin1String(PK_NAME),
                                  QLatin1String(PK_PATH),
                                  QDBusConnection::systemBus(),
                                  this);

    // Set up database for desktop files
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE", PK_DESKTOP_DEFAULT_DATABASE);
    db.setDatabaseName(PK_DESKTOP_DEFAULT_DATABASE);
    if (!db.open()) {
        qDebug() << "Failed to initialize the desktop files database";
    }

    qRegisterMetaType<PackageKit::Daemon::Network>("PackageKit::Daemon::Network");
    qRegisterMetaType<PackageKit::Daemon::Authorize>("PackageKit::Daemon::Authorize");
    qRegisterMetaType<PackageKit::Transaction::InternalError>("PackageKit::Transaction::InternalError");
    qRegisterMetaType<PackageKit::Transaction::Role>("PackageKit::Transaction::Role");
    qRegisterMetaType<PackageKit::Transaction::Error>("PackageKit::Transaction::Error");
    qRegisterMetaType<PackageKit::Transaction::Exit>("PackageKit::Transaction::Exit");
    qRegisterMetaType<PackageKit::Transaction::Filter>("PackageKit::Transaction::Filter");
    qRegisterMetaType<PackageKit::Transaction::Message>("PackageKit::Transaction::Message");
    qRegisterMetaType<PackageKit::Transaction::Status>("PackageKit::Transaction::Status");
    qRegisterMetaType<PackageKit::Transaction::MediaType>("PackageKit::Transaction::MediaType");
    qRegisterMetaType<PackageKit::Transaction::Provides>("PackageKit::Transaction::Provides");
    qRegisterMetaType<PackageKit::Transaction::ProvidesFlag>("PackageKit::Transaction::ProvidesFlag");
    qRegisterMetaType<PackageKit::Transaction::DistroUpgrade>("PackageKit::Transaction::DistroUpgrade");
    qRegisterMetaType<PackageKit::Transaction::UpgradeKind>("PackageKit::Transaction::UpgradeKind");
    qRegisterMetaType<PackageKit::Transaction::TransactionFlag>("PackageKit::Transaction::TransactionFlag");
    qRegisterMetaType<PackageKit::Transaction::TransactionFlags>("PackageKit::Transaction::TransactionFlags");
    qRegisterMetaType<PackageKit::Transaction::Restart>("PackageKit::Transaction::Restart");
    qRegisterMetaType<PackageKit::Transaction::UpdateState>("PackageKit::Transaction::UpdateState");
    qRegisterMetaType<PackageKit::Transaction::Group>("PackageKit::Transaction::Group");
    qRegisterMetaType<PackageKit::Transaction::Info>("PackageKit::Transaction::Info");
    qRegisterMetaType<PackageKit::Transaction::SigType>("PackageKit::Transaction::SigType");
}

void DaemonPrivate::setupSignal(const QString &signal, bool connect)
{
    Q_Q(Daemon);

    const char *signalToConnect = 0;
    const char *memberToConnect = 0;

    if (signal == SIGNAL(changed())) {
        signalToConnect = SIGNAL(Changed());
        memberToConnect = SIGNAL(changed());
    } else if (signal == SIGNAL(repoListChanged())) {
        signalToConnect = SIGNAL(RepoListChanged());
        memberToConnect = SIGNAL(repoListChanged());
    } else if (signal == SIGNAL(restartScheduled())) {
        signalToConnect = SIGNAL(RestartSchedule());
        memberToConnect = SIGNAL(restartScheduled());
    } else if (signal == SIGNAL(transactionListChanged(QStringList))) {
        signalToConnect = SIGNAL(TransactionListChanged(QStringList));
        memberToConnect = SIGNAL(transactionListChanged(QStringList));
    } else if (signal == SIGNAL(updatesChanged())) {
        signalToConnect = SIGNAL(UpdatesChanged());
        memberToConnect = SIGNAL(updatesChanged());
    }

    if (signalToConnect && memberToConnect) {
        if (connect) {
            q->connect(daemon, signalToConnect, memberToConnect);
        } else {
            daemon->disconnect(signalToConnect, q, memberToConnect);
        }
    }
}

void Daemon::connectNotify(const char *signal)
{
    Q_D(Daemon);
    if (!d->connectedSignals.contains(signal) && d->daemon) {
        d->setupSignal(signal, true);
    }
    d->connectedSignals << signal;
}

void Daemon::disconnectNotify(const char *signal)
{
    Q_D(Daemon);
    if (d->connectedSignals.contains(signal)) {
        d->connectedSignals.removeOne(signal);
        if (d->daemon && !d->connectedSignals.contains(signal)) {
            d->setupSignal(signal, false);
        }
    }
}

Daemon::~Daemon()
{
}

Transaction::Roles Daemon::actions()
{
    Q_D(const Daemon);
    return d->daemon->roles();
}

Transaction::ProvidesFlag Daemon::provides()
{
    Q_D(const Daemon);
    return static_cast<Transaction::ProvidesFlag>(static_cast<uint>(d->daemon->provides()));
}

QString Daemon::backendName()
{
    Q_D(const Daemon);
    return d->daemon->backendName();
}

QString Daemon::backendDescription()
{
    Q_D(const Daemon);
    return d->daemon->backendDescription();
}

QString Daemon::backendAuthor()
{
    Q_D(const Daemon);
    return d->daemon->backendAuthor();
}

Transaction::Filters Daemon::filters()
{
    Q_D(const Daemon);
    return static_cast<Transaction::Filters>(static_cast<uint>(d->daemon->filters()));
}

Transaction::Groups Daemon::groups()
{
    Q_D(const Daemon);
    return static_cast<Transaction::Groups>(d->daemon->groups());
}

bool Daemon::locked()
{
    Q_D(const Daemon);
    return d->daemon->locked();
}

QStringList Daemon::mimeTypes()
{
    Q_D(const Daemon);
    return d->daemon->mimeTypes();
}

Daemon::Network Daemon::networkState()
{
    Q_D(const Daemon);
    return static_cast<Daemon::Network>(d->daemon->networkState());
}

QString Daemon::distroID()
{
    Q_D(const Daemon);
    return d->daemon->distroId();
}

Daemon::Authorize Daemon::canAuthorize(const QString &actionId)
{
    Q_D(Daemon);
    QDBusPendingReply<uint> reply = d->daemon->CanAuthorize(actionId);
    reply.waitForFinished();
    d->lastError = reply.error();
    if (reply.isValid()) {
        return static_cast<Daemon::Authorize>(reply.value());
    }
    return Daemon::AuthorizeUnknown;
}

QDBusObjectPath Daemon::getTid()
{
    Q_D(Daemon);
    QDBusPendingReply<QDBusObjectPath> reply = d->daemon->CreateTransaction();
    reply.waitForFinished();
    d->lastError = reply.error();
    if (reply.isValid()) {
        return reply.value();
    }
    return QDBusObjectPath();
}

uint Daemon::getTimeSinceAction(Transaction::Role role)
{
    Q_D(Daemon);
    QDBusPendingReply<uint> reply = d->daemon->GetTimeSinceAction(role);
    reply.waitForFinished();
    d->lastError = reply.error();
    if (reply.isValid()) {
        return reply.value();
    }
    return 0u;
}

QList<QDBusObjectPath> Daemon::getTransactionList()
{
    Q_D(Daemon);
    QDBusPendingReply<QList<QDBusObjectPath> > reply = d->daemon->GetTransactionList();
    reply.waitForFinished();
    d->lastError = reply.error();
    if (reply.isValid()) {
        return reply.value();
    }
    return QList<QDBusObjectPath>();
}

QList<Transaction*> Daemon::getTransactionObjects(QObject *parent)
{
    Q_D(Daemon);
    return d->transactions(getTransactionList(), parent);
}

void Daemon::setHints(const QStringList &hints)
{
    Q_D(Daemon);
    d->hints = hints;
}

void Daemon::setHints(const QString &hints)
{
    Q_D(Daemon);
    d->hints = QStringList() << hints;
}

QStringList Daemon::hints()
{
    Q_D(const Daemon);
    return d->hints;
}

Transaction::InternalError Daemon::setProxy(const QString& http_proxy, const QString& https_proxy, const QString& ftp_proxy, const QString& socks_proxy, const QString& no_proxy, const QString& pac)
{
    Q_D(Daemon);
    QDBusPendingReply<> r = d->daemon->SetProxy(http_proxy, https_proxy, ftp_proxy, socks_proxy, no_proxy, pac);
    r.waitForFinished();
    d->lastError = r.error();
    if (r.isError()) {
        return Transaction::parseError(r.error().name());
    } else {
        return Transaction::InternalErrorNone;
    }
}

void Daemon::stateHasChanged(const QString& reason)
{
    PENDING_CALL(StateHasChanged(reason))
}

void Daemon::suggestDaemonQuit()
{
    PENDING_CALL(SuggestDaemonQuit())
}

QDBusError Daemon::lastError() const
{
    Q_D(const Daemon);
    return d->lastError;
}

uint Daemon::versionMajor()
{
    Q_D(const Daemon);
    return d->daemon->versionMajor();
}

uint Daemon::versionMinor()
{
    Q_D(const Daemon);
    return d->daemon->versionMinor();
}

uint Daemon::versionMicro()
{
    Q_D(const Daemon);
    return d->daemon->versionMicro();
}

QString Daemon::packageName(const QString &packageID)
{
    return packageID.section(QLatin1Char(';'), 0, 0);
}

QString Daemon::packageVersion(const QString &packageID)
{
    return packageID.section(QLatin1Char(';'), 1, 1);
}

QString Daemon::packageArch(const QString &packageID)
{
    return packageID.section(QLatin1Char(';'), 2, 2);
}

QString Daemon::packageData(const QString &packageID)
{
    return packageID.section(QLatin1Char(';'), 3, 3);
}

QString Daemon::packageIcon(const QString &packageID)
{
    return Transaction::packageIcon(packageID);
}

#include "daemon.moc"
