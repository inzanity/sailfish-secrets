/*
 * Copyright (C) 2017 Jolla Ltd.
 * Contact: Chris Adams <chris.adams@jollamobile.com>
 * All rights reserved.
 * BSD 3-Clause License, see LICENSE.
 */

#ifndef SAILFISHSECRETS_APIIMPL_REQUESTPROCESSOR_P_H
#define SAILFISHSECRETS_APIIMPL_REQUESTPROCESSOR_P_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QDateTime>
#include <QtCore/QMultiMap>
#include <QtCore/QTimer>

#include <sys/types.h>

#include "Secrets/result.h"
#include "Secrets/interactionparameters.h"
#include "Secrets/secretmanager.h"
#include "Secrets/secret.h"
#include "Secrets/interactionparameters.h"
#include "Secrets/extensionplugins.h"
#include "Secrets/lockcoderequest.h"

#include "SecretsImpl/secrets_p.h"
#include "SecretsImpl/applicationpermissions_p.h"

#include "bookkeepingdatabase_p.h"
#include "requestqueue_p.h"

namespace Sailfish {

namespace Secrets {

namespace Daemon {

namespace ApiImpl {

// The RequestProcessor implements the Secrets Daemon API.
// It processes requests from clients which are forwarded
// by the RequestQueue, by interacting with the database
// and returns the results to the RequestQueue to forward
// back to clients.
class RequestProcessor : public QObject
{
    Q_OBJECT

public:
    RequestProcessor(Sailfish::Secrets::Daemon::ApiImpl::BookkeepingDatabase *bkdb,
                     Sailfish::Secrets::Daemon::ApiImpl::ApplicationPermissions *appPermissions,
                     bool autotestMode,
                     Sailfish::Secrets::Daemon::ApiImpl::SecretsRequestQueue *parent = Q_NULLPTR);

    bool loadPlugins();

    // retrieve information about available plugins
    Sailfish::Secrets::Result getPluginInfo(
            pid_t callerPid,
            quint64 requestId,
            QVector<Sailfish::Secrets::StoragePluginInfo> *storagePlugins,
            QVector<Sailfish::Secrets::EncryptionPluginInfo> *encryptionPlugins,
            QVector<Sailfish::Secrets::EncryptedStoragePluginInfo> *encryptedStoragePlugins,
            QVector<Sailfish::Secrets::AuthenticationPluginInfo> *authenticationPlugins);

    // retrieve the names of collections
    Sailfish::Secrets::Result collectionNames(
            pid_t callerPid,
            quint64 requestId,
            QStringList *names);

    // create a DeviceLock-protected collection
    Sailfish::Secrets::Result createDeviceLockCollection(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            Sailfish::Secrets::SecretManager::DeviceLockUnlockSemantic unlockSemantic,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode);

    // create a CustomLock-protected collection
    Sailfish::Secrets::Result createCustomLockCollection(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const QString &authenticationPluginName,
            Sailfish::Secrets::SecretManager::CustomLockUnlockSemantic unlockSemantic,
            int customLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // delete a collection
    Sailfish::Secrets::Result deleteCollection(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode);

    // set a secret in a collection
    Sailfish::Secrets::Result setCollectionSecret(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret &secret,
            const Sailfish::Secrets::InteractionParameters &uiParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // set a standalone DeviceLock-protected secret
    Sailfish::Secrets::Result setStandaloneDeviceLockSecret(
            pid_t callerPid,
            quint64 requestId,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const Sailfish::Secrets::Secret &secret,
            const Sailfish::Secrets::InteractionParameters &uiParams,
            Sailfish::Secrets::SecretManager::DeviceLockUnlockSemantic unlockSemantic,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // set a standalone CustomLock-protected secret
    Sailfish::Secrets::Result setStandaloneCustomLockSecret(
            pid_t callerPid,
            quint64 requestId,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const QString &authenticationPluginName,
            const Sailfish::Secrets::Secret &secret,
            const Sailfish::Secrets::InteractionParameters &uiParams,
            Sailfish::Secrets::SecretManager::CustomLockUnlockSemantic unlockSemantic,
            int customLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // get a secret in a collection
    Sailfish::Secrets::Result getCollectionSecret(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            Sailfish::Secrets::Secret *secret);

    // get a standalone secret
    Sailfish::Secrets::Result getStandaloneSecret(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            Sailfish::Secrets::Secret *secret);

    // find collection secrets via filter
    Sailfish::Secrets::Result findCollectionSecrets(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            const Sailfish::Secrets::Secret::FilterData &filter,
            Sailfish::Secrets::SecretManager::FilterOperator filterOperator,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            QVector<Sailfish::Secrets::Secret::Identifier> *identifiers);

    // find standalone secrets via filter
    Sailfish::Secrets::Result findStandaloneSecrets(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::FilterData &filter,
            Sailfish::Secrets::SecretManager::FilterOperator filterOperator,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            QVector<Sailfish::Secrets::Secret::Identifier> *identifiers);

    // delete a secret in a collection
    Sailfish::Secrets::Result deleteCollectionSecret(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // delete a standalone secret
    Sailfish::Secrets::Result deleteStandaloneSecret(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode);

    // modify a lock code (re-key a plugin, encrypted collection or standalone secret)
    Sailfish::Secrets::Result modifyLockCode(
            pid_t callerPid,
            quint64 requestId,
            Sailfish::Secrets::LockCodeRequest::LockCodeTargetType lockCodeTargetType,
            const QString &lockCodeTarget,
            const Sailfish::Secrets::InteractionParameters &interactionParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // provide a lock code (unlock a plugin, encrypted collection or standalone secret)
    Sailfish::Secrets::Result provideLockCode(
            pid_t callerPid,
            quint64 requestId,
            Sailfish::Secrets::LockCodeRequest::LockCodeTargetType lockCodeTargetType,
            const QString &lockCodeTarget,
            const Sailfish::Secrets::InteractionParameters &interactionParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    // forget a lock code (lock a plugin, encrypted collection or standalone secret)
    Sailfish::Secrets::Result forgetLockCode(
            pid_t callerPid,
            quint64 requestId,
            Sailfish::Secrets::LockCodeRequest::LockCodeTargetType lockCodeTargetType,
            const QString &lockCodeTarget,
            const Sailfish::Secrets::InteractionParameters &interactionParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

public: // helper methods for crypto API bridge (secretscryptohelpers)
    QMap<QString, QObject*> potentialCryptoStoragePlugins() const;
    QStringList storagePluginNames() const;
    Sailfish::Secrets::Result confirmKeyStoragePlugin(
            const QString &hashedKeyName,
            const QString &collectionName,
            const QString &storagePluginName) const;
    Sailfish::Secrets::Result confirmCollectionStoragePlugin(
            const QString &collectionName,
            const QString &storagePluginName) const;
    Sailfish::Secrets::Result setCollectionSecretMetadata(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier);
    Sailfish::Secrets::Result deleteCollectionSecretMetadata(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier);
    Sailfish::Secrets::Result userInput(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::InteractionParameters &uiParams);

private Q_SLOTS:
    void authenticationCompleted(
            uint callerPid,
            qint64 requestId,
            const Sailfish::Secrets::Result &result);
    void userInputInteractionCompleted(
            uint callerPid,
            qint64 requestId,
            const Sailfish::Secrets::InteractionParameters &request,
            const QString &interactionServiceAddress,
            const Sailfish::Secrets::Result &result,
            const QByteArray &authenticationCode);

    void timeoutRelockCollection();
    void timeoutRelockSecret();

private:
    Sailfish::Secrets::Result createCustomLockCollectionWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const QString &authenticationPluginName,
            Sailfish::Secrets::SecretManager::CustomLockUnlockSemantic unlockSemantic,
            int customLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &authenticationCode);

    Sailfish::Secrets::Result setCollectionSecretGetAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const Secret &secret,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            bool collectionUsesDeviceLockKey,
            const QString &collectionApplicationId,
            const QString &collectionStoragePluginName,
            const QString &collectionEncryptionPluginName,
            const QString &collectionAuthenticationPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode collectionAccessControlMode);

    Sailfish::Secrets::Result setCollectionSecretWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret &secret,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            bool collectionUsesDeviceLockKey,
            const QString &collectionApplicationId,
            const QString &collectionStoragePluginName,
            const QString &collectionEncryptionPluginName,
            const QString &collectionAuthenticationPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode collectionAccessControlMode,
            const QByteArray &authenticationCode);

    Sailfish::Secrets::Result setCollectionSecretWithEncryptionKey(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret &secret,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            bool collectionUsesDeviceLockKey,
            const QString &collectionApplicationId,
            const QString &collectionStoragePluginName,
            const QString &collectionEncryptionPluginName,
            const QString &collectionAuthenticationPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode collectionAccessControlMode,
            const QByteArray &encryptionKey);

    Sailfish::Secrets::Result writeStandaloneDeviceLockSecret(
            pid_t callerPid,
            quint64 requestId,
            const QString &callerApplicationId,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const Sailfish::Secrets::Secret &secret,
            const QString &collectionName,
            const QString &hashedSecretName,
            bool found,
            Sailfish::Secrets::SecretManager::DeviceLockUnlockSemantic unlockSemantic,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode);

    Sailfish::Secrets::Result setStandaloneCustomLockSecretGetAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const QString &callerApplicationId,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const QString &authenticationPluginName,
            const Sailfish::Secrets::Secret &secret,
            Sailfish::Secrets::SecretManager::CustomLockUnlockSemantic unlockSemantic,
            int customLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress);

    Sailfish::Secrets::Result setStandaloneCustomLockSecretWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const QString &authenticationPluginName,
            const Sailfish::Secrets::Secret &secret,
            Sailfish::Secrets::SecretManager::CustomLockUnlockSemantic unlockSemantic,
            int customLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &authenticationCode);

    Sailfish::Secrets::Result setStandaloneCustomLockSecretWithEncryptionKey(
            pid_t callerPid,
            quint64 requestId,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            const QString &authenticationPluginName,
            const Sailfish::Secrets::Secret &secret,
            Sailfish::Secrets::SecretManager::CustomLockUnlockSemantic unlockSemantic,
            int customLockTimeoutMs,
            Sailfish::Secrets::SecretManager::AccessControlMode accessControlMode,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &encryptionKey);

    Sailfish::Secrets::Result getCollectionSecretWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            const QByteArray &authenticationCode,
            Sailfish::Secrets::Secret *secret);

    Sailfish::Secrets::Result getCollectionSecretWithEncryptionKey(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            const QByteArray &encryptionKey,
            Sailfish::Secrets::Secret *secret);

    Sailfish::Secrets::Result getStandaloneSecretWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            int lockSemantic,
            int customLockTimeoutMs,
            const QByteArray &authenticationCode,
            Sailfish::Secrets::Secret *secret);

    Sailfish::Secrets::Result getStandaloneSecretWithEncryptionKey(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            int lockSemantic,
            int customLockTimeoutMs,
            const QByteArray &encryptionKey,
            Sailfish::Secrets::Secret *secret);

    Sailfish::Secrets::Result findCollectionSecretsWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            const Sailfish::Secrets::Secret::FilterData &filter,
            Sailfish::Secrets::SecretManager::FilterOperator filterOperator,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            const QByteArray &authenticationCode,
            QVector<Sailfish::Secrets::Secret::Identifier> *identifiers);

    Sailfish::Secrets::Result findCollectionSecretsWithEncryptionKey(
            pid_t callerPid,
            quint64 requestId,
            const QString &collectionName,
            const Sailfish::Secrets::Secret::FilterData &filter,
            Sailfish::Secrets::SecretManager::FilterOperator filterOperator,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QString &storagePluginName,
            const QString &encryptionPluginName,
            int collectionUnlockSemantic,
            int collectionCustomLockTimeoutMs,
            const QByteArray &encryptionKey,
            QVector<Sailfish::Secrets::Secret::Identifier> *identifiers);

    Sailfish::Secrets::Result deleteCollectionSecretWithAuthenticationCode(
            pid_t callerPid,
            quint64 requestId,
            const Sailfish::Secrets::Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &authenticationCode);

    Sailfish::Secrets::Result deleteCollectionSecretWithEncryptionKey(
            pid_t callerPid,
            quint64 requestId,
            const Secret::Identifier &identifier,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &encryptionKey);

    Sailfish::Secrets::Result modifyLockCodeWithLockCode(
            pid_t callerPid,
            quint64 requestId,
            Sailfish::Secrets::LockCodeRequest::LockCodeTargetType lockCodeTargetType,
            const QString &lockCodeTarget,
            const Sailfish::Secrets::InteractionParameters &interactionParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &oldLockCode);

    Sailfish::Secrets::Result modifyLockCodeWithLockCodes(
            pid_t callerPid,
            quint64 requestId,
            Sailfish::Secrets::LockCodeRequest::LockCodeTargetType lockCodeTargetType,
            const QString &lockCodeTarget,
            const Sailfish::Secrets::InteractionParameters &interactionParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &oldLockCode,
            const QByteArray &newLockCode);

    Sailfish::Secrets::Result provideLockCodeWithLockCode(
            pid_t callerPid,
            quint64 requestId,
            Sailfish::Secrets::LockCodeRequest::LockCodeTargetType lockCodeTargetType,
            const QString &lockCodeTarget,
            const Sailfish::Secrets::InteractionParameters &interactionParams,
            Sailfish::Secrets::SecretManager::UserInteractionMode userInteractionMode,
            const QString &interactionServiceAddress,
            const QByteArray &lockCode);

private:
    struct PendingRequest {
        PendingRequest()
            : callerPid(0), requestId(0), requestType(Sailfish::Secrets::Daemon::ApiImpl::InvalidRequest) {}
        PendingRequest(uint pid, quint64 rid, Sailfish::Secrets::Daemon::ApiImpl::RequestType rtype, QVariantList params)
            : callerPid(pid), requestId(rid), requestType(rtype), parameters(params) {}
        PendingRequest(const PendingRequest &other)
            : callerPid(other.callerPid), requestId(other.requestId), requestType(other.requestType), parameters(other.parameters) {}
        uint callerPid;
        quint64 requestId;
        Sailfish::Secrets::Daemon::ApiImpl::RequestType requestType;
        QVariantList parameters;
    };

    Sailfish::Secrets::Daemon::ApiImpl::BookkeepingDatabase *m_bkdb;
    Sailfish::Secrets::Daemon::ApiImpl::SecretsRequestQueue *m_requestQueue;
    Sailfish::Secrets::Daemon::ApiImpl::ApplicationPermissions *m_appPermissions;

    QMap<QString, Sailfish::Secrets::StoragePlugin*> m_storagePlugins;
    QMap<QString, Sailfish::Secrets::EncryptionPlugin*> m_encryptionPlugins;
    QMap<QString, Sailfish::Secrets::EncryptedStoragePlugin*> m_encryptedStoragePlugins;
    QMap<QString, Sailfish::Secrets::AuthenticationPlugin*> m_authenticationPlugins;
    QMap<QString, QObject*> m_potentialCryptoStoragePlugins;

    QMap<QString, QTimer*> m_collectionLockTimers;
    QMap<QString, QByteArray> m_collectionEncryptionKeys;
    QMap<QString, QTimer*> m_standaloneSecretLockTimers;
    QMap<QString, QByteArray> m_standaloneSecretEncryptionKeys;
    QMap<quint64, Sailfish::Secrets::Daemon::ApiImpl::RequestProcessor::PendingRequest> m_pendingRequests;

    bool m_autotestMode;
    bool loadPlugins(const QString &pluginDir);
};

} // namespace ApiImpl

} // namespace Daemon

} // namespace Secrets

} // namespace Sailfish

#endif // SAILFISHSECRETS_APIIMPL_REQUESTPROCESSOR_P_H
