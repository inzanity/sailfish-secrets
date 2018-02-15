/*
 * Copyright (C) 2017 Jolla Ltd.
 * Contact: Chris Adams <chris.adams@jollamobile.com>
 * All rights reserved.
 * BSD 3-Clause License, see LICENSE.
 */

#include "Crypto/key.h"
#include "Crypto/key_p.h"
#include "Crypto/x509certificate.h"

#define SAILFISH_SECRETS_SECRET_FILTERDATAFIELDTYPE QLatin1String("Type")
#define SAILFISH_SECRETS_SECRET_TYPECRYPTOKEY QLatin1String("CryptoKey")

using namespace Sailfish::Crypto;

//--------------------------------------------

KeyIdentifierPrivate::KeyIdentifierPrivate()
    : QSharedData()
{
}

KeyIdentifierPrivate::KeyIdentifierPrivate(const KeyIdentifierPrivate &other)
    : QSharedData(other)
    , m_name(other.m_name)
    , m_collectionName(other.m_collectionName)
{
}


KeyIdentifierPrivate::~KeyIdentifierPrivate()
{
}

//--------------------------------------------

KeyPrivate::KeyPrivate()
    : QSharedData()
    , m_origin(Key::OriginUnknown)
    , m_algorithm(CryptoManager::AlgorithmUnknown)
    , m_operations(CryptoManager::OperationUnknown)
    , m_componentConstraints(Key::MetaData | Key::PublicKeyData)
    , m_keySize(0)
{
    m_filterData.insert(SAILFISH_SECRETS_SECRET_FILTERDATAFIELDTYPE, SAILFISH_SECRETS_SECRET_TYPECRYPTOKEY);
}

KeyPrivate::KeyPrivate(const KeyPrivate &other)
    : QSharedData(other)
    , m_filterData(other.m_filterData)
    , m_customParameters(other.m_customParameters)
    , m_publicKey(other.m_publicKey)
    , m_privateKey(other.m_privateKey)
    , m_secretKey(other.m_secretKey)
    , m_identifier(other.m_identifier)
    , m_origin(other.m_origin)
    , m_algorithm(other.m_algorithm)
    , m_operations(other.m_operations)
    , m_componentConstraints(other.m_componentConstraints)
    , m_keySize(other.m_keySize)
{
}

KeyPrivate::~KeyPrivate()
{
}

//--------------------------------------------

/*!
 * \class Key::Identifier
 * \brief An identifier for a key
 *
 * The identifier consists of the name (alias) of the key, along with
 * the name of the collection in which the key is stored (note that the
 * collection name can be empty if the key is stored as a standalone
 * secret).
 *
 * Together, the key name and collection name uniquely identify the key
 * as a specific secret in the secrets storage.
 */

/*!
 * \brief Constructs a new, empty identifier
 */
Key::Identifier::Identifier()
    : d_ptr(new KeyIdentifierPrivate)
{
}

/*!
 * \brief Constructs a new identifier from the given key \a name and \a collectionName
 */
Key::Identifier::Identifier(const QString &name, const QString &collectionName)
        : d_ptr(new KeyIdentifierPrivate)
{
    d_ptr->m_name = name;
    d_ptr->m_collectionName = collectionName;
}

/*!
 * \brief Constructs a copy of the \a other identifier
 */
Key::Identifier::Identifier(const Key::Identifier &other)
    : d_ptr(other.d_ptr)
{
}

/*!
 * \brief Destroys the identifier
 */
Key::Identifier::~Identifier()
{
}

/*!
 * \brief Assigns the \a other identifier to this identifier
 */
Key::Identifier& Key::Identifier::operator=(const Key::Identifier &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
 * \brief Returns true if the key name and collection name are identical to that of the \a other identifier
 */
bool Key::Identifier::operator==(const Key::Identifier &other) const
{
    return d_ptr->m_name == other.d_ptr->m_name
            && d_ptr->m_collectionName == other.d_ptr->m_collectionName;
}

/*!
 * \brief Returns true if this identifier should sort before the \a other identifier
 *
 * It is sorted first on collection name, and then on the key name.
 */
bool operator<(const Key::Identifier &lhs, const Key::Identifier &rhs)
{
    return (lhs.collectionName() == rhs.collectionName())
            ? (lhs.name() < rhs.name())
            : (lhs.collectionName() < rhs.collectionName());
}

/*!
 * \brief Returns the key name from the identifier
 */
QString Key::Identifier::name() const
{
    return d_ptr->m_name;
}

/*!
 * \brief Sets the key name in the identifier to \a name
 */
void Key::Identifier::setName(const QString &name)
{
    d_ptr->m_name = name;
}

/*!
 * \brief Returns the collection name from the identifier
 */
QString Key::Identifier::collectionName() const
{
    return d_ptr->m_collectionName;
}

/*!
 * \brief Sets the collection name in the identifier to \a collectionName
 */
void Key::Identifier::setCollectionName(const QString &collectionName)
{
    d_ptr->m_collectionName = collectionName;
}

//--------------------------------------------

/*!
 * \class Key
 * \brief An instance of a key which can be used for cryptographic operations.
 *
 * The Key class encapsulates information about a
 * cryptographic key, including metadata such as the cryptosystem algorithm
 * the key is used with, the types of operations which may be performed
 * with the key, and the parameters which are supported when performing
 * operations with the key, as well as key data (private/public key data
 * for asymmetric cryptosystems, and secret key data for symmetric cryptosystems).
 *
 * In many cases, client applications need never know the key data, as the
 * key can be generated and stored securely, and then used securely by name reference,
 * without the key data ever entering the client application process address space.
 */

/*!
 * \brief Constructs an empty key
 */
Key::Key()
    : d_ptr(new KeyPrivate)
{
}

/*!
 * \brief Constructs a copy of the \a other key
 */
Key::Key(const Key &other)
    : d_ptr(other.d_ptr)
{
}

/*!
 * \brief Constructs a key which references a stored key with the given \a name from the given \a collection.
 *
 * A stored key is one which is stored securely by the Sailfish Crypto daemon,
 * whose underlying secret data (e.g. private key or secret key data) will never
 * be exposed to the client process.
 */
Key::Key(const QString &name, const QString &collection)
    : d_ptr(new KeyPrivate)
{
    setIdentifier(Key::Identifier(name, collection));
}

/*!
 * \brief Destroys the key
 */
Key::~Key()
{
}

/*!
 * \brief Assigns the \a other key to this key, and returns a reference to this key
 */
Key& Key::operator=(const Key &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
 * \brief Returns true if the underlying data and metadata in this key are identical to those in \a other, otherwise false
 */
bool Key::operator==(const Key &other) const
{
    return d_ptr->m_filterData == other.d_ptr->m_filterData
        && d_ptr->m_customParameters == other.d_ptr->m_customParameters
        && d_ptr->m_publicKey == other.d_ptr->m_publicKey
        && d_ptr->m_privateKey == other.d_ptr->m_privateKey
        && d_ptr->m_secretKey == other.d_ptr->m_secretKey
        && d_ptr->m_identifier == other.d_ptr->m_identifier
        && d_ptr->m_origin == other.d_ptr->m_origin
        && d_ptr->m_algorithm == other.d_ptr->m_algorithm
        && d_ptr->m_operations == other.d_ptr->m_operations;
}

/*!
 * \brief Returns true if this key should sort before the \a other key
 */
bool operator<(const Key &lhs, const Key &rhs)
{
    if (lhs.identifier() != rhs.identifier()) {
        return lhs.identifier() < rhs.identifier();
    } else if (lhs.publicKey() != rhs.publicKey()) {
        return lhs.publicKey() < rhs.publicKey();
    } else if (lhs.privateKey() != rhs.privateKey()) {
        return lhs.privateKey() < rhs.privateKey();
    } else if (lhs.secretKey() != rhs.secretKey()) {
        return lhs.secretKey() < rhs.secretKey();
    } else {
        return lhs.algorithm() < rhs.algorithm();
    }
}

/*!
 * \brief Returns the identifier of the stored key which this key references
 */
Key::Identifier Key::identifier() const
{
    return d_ptr->m_identifier;
}

/*!
 * \brief Sets the identifier of the stored key which this key references to the given \a identifier
 */
void Key::setIdentifier(const Key::Identifier &identifier)
{
    d_ptr->m_identifier = identifier;
}

/*!
 * \brief Returns information about the origin of the key
 */
Key::Origin Key::origin() const
{
    return d_ptr->m_origin;
}

/*!
 * \brief Sets origin information for the key to the given \a origin
 */
void Key::setOrigin(Key::Origin origin)
{
    d_ptr->m_origin = origin;
}

/*!
 * \brief Returns the cryptosystem algorithm this key is intended to be used with
 */
CryptoManager::Algorithm Key::algorithm() const
{
    return d_ptr->m_algorithm;
}

/*!
 * \brief Sets the cryptosystem algorithm this key is intended to be used with to \a algorithm
 */
void Key::setAlgorithm(CryptoManager::Algorithm algorithm)
{
    d_ptr->m_algorithm = algorithm;
}

/*!
 * \brief Returns the set of operations which are supported for this key
 */
CryptoManager::Operations Key::operations() const
{
    return d_ptr->m_operations;
}

/*!
 * \brief Sets the operations which are supported for this key to \a operations
 */
void Key::setOperations(CryptoManager::Operations operations)
{
    d_ptr->m_operations = operations;
}

/*!
 * \brief Returns the types of key components which the client is allowed to retrieve after the key has been stored
 */
Key::Components Key::componentConstraints() const
{
    return d_ptr->m_componentConstraints;
}

/*!
 * \brief Sets the types of key components which the client is allowed to retrieve after the key has been stored to \a components
 *
 * The client is able to retrieve the key components specified in the
 * componentsConstraints() after the key has been stored.
 *
 * When a key is generated and stored, the client can specify constraints which
 * should be enforced by the crypto storage plugin in which the key is stored.
 * This allows the client to specify, for example, that no client (including
 * itself) is allowed to retrieve the secret key data from the key, after the
 * key has been stored, to ensure the security of the key is maintained.
 *
 * By default, only Key::MetaData and Key::PublicKeyData are included in the
 * components constraints, and so any secret or private key data will NOT
 * be able to be read back by clients, if the key is stored in a crypto plugin
 * which enforces key component constraints.
 *
 * Note that only crypto storage plugins (that is, any plugin which implements both
 * the Sailfish::Crypto::CryptoPlugin and the Sailfish::Secrets::EncryptedStoragePlugin
 * interfaces) can enforce these key component constraints.  If the key is stored
 * in any other type of storage plugin (e.g. a Sailfish::Secrets::StoragePlugin)
 * then the key component constraints will not be enforced.
 *
 * Also note that whether the crypto storage plugin enforces the constraint or not
 * is up to the plugin.  Please see the documentation for the plugin you intend
 * to use, to see if it supports enforcing key component constraints.
 */
void Key::setComponentConstraints(Key::Components components)
{
    d_ptr->m_componentConstraints = components;
}

/*!
 * \brief Returns the security size, in bits, of the key.
 *
 * Note that this will NOT necessarily be the data size of any of
 * the key fields, depending on the type of algorithm the key
 * is designed to be used for.
 *
 * For symmetric algorithm keys, the security size is generally also
 * the data size (in bits) of the secret key.
 *
 * For asymmetric keys, the security size is generally the size
 * of the modulus (in the case of RSA keys) or the curve group
 * size (in the case of ECC keys), and the actual data size of
 * the private and public key data may be much larger (for example,
 * the private key data for an RSA key could include modulus,
 * public exponent, private exponent, prime factors, reduced modulo
 * factors, and inverse factor modulo, in order to avoid having to
 * recalculate those pieces of data at every use - which altogether
 * adds up to a much larger data size than the security size).
 *
 * As such, an RSA key with a security size of 2048 bits could
 * have a data (storage) size of 1232 bytes (in PKCS#8 format).
 */
int Key::keySize() const
{
    return d_ptr->m_keySize;
}

/*!
 * \brief Sets the security size, in bits, of the key to \a size
 *
 * In general, this should never be called by client applications,
 * as the required key security size is instead defined when
 * generating the key.
 */
void Key::setKeySize(int size)
{
    d_ptr->m_keySize = size;
}

/*!
 * \brief Returns the public key data associated with this key (asymmetric cryptosystems only)
 */
QByteArray Key::publicKey() const
{
    return d_ptr->m_publicKey;
}

/*!
 * \brief Sets the public key data associated with this key to \a key
 */
void Key::setPublicKey(const QByteArray &key)
{
    d_ptr->m_publicKey = key;
}

/*!
 * \brief Returns the private key data associated with this key (asymmetric cryptosystems only)
 */
QByteArray Key::privateKey() const
{
    return d_ptr->m_privateKey;
}

/*!
 * \brief Sets the private key data associated with this key to \a key
 *
 * This field will be ignored if the algorithm specified for the key
 * is that of a symmetric cryptosystem.
 */
void Key::setPrivateKey(const QByteArray &key)
{
    d_ptr->m_privateKey = key;
}

/*!
 * \brief Returns the private key data associated with this key (symmetric cryptosystems only)
 */
QByteArray Key::secretKey() const
{
    return d_ptr->m_secretKey;
}

/*!
 * \breif Sets the secret key data associated with this key to \a key
 *
 * This field will be ignored if the algorithm specified for the key
 * is that of an asymmetric cryptosystem.
 */
void Key::setSecretKey(const QByteArray &key)
{
    d_ptr->m_secretKey = key;
}

/*!
 * \brief Returns the custom parameters associated with this key
 */
QVector<QByteArray> Key::customParameters() const
{
    return d_ptr->m_customParameters;
}

/*!
 * \brief Sets the custom parameters associated with this key to \a parameters.
 *
 * Some cryptosystem providers (i.e. Sailfish Crypto API extension plugins)
 * may require some custom parameters to be supplied when generating, storing
 * or performing operations with keys.
 *
 * In general, these parameters will be ignored unless the extension plugin
 * requires them for some operation.
 */
void Key::setCustomParameters(const QVector<QByteArray> &parameters)
{
    d_ptr->m_customParameters = parameters;
}

/*!
 * \brief Extracts metadata and the public key from the given \a certificate and returns a Key encapsulating that data
 */
Key Key::fromCertificate(const Certificate &certificate)
{
    if (certificate.type() != Certificate::X509) {
        // TODO: other certificate types.
        return Key();
    }

    X509Certificate x509cert(X509Certificate::fromCertificate(certificate));
    Key retn;
    retn.setPublicKey(x509cert.publicKey());
    // TODO: read the algorithm from the certificate
    // TODO: read the digests from the certificate
    // TODO: read the allowed operations from the X509v3 Key Usage extension:
    // retn.setOperations(x509cert.tbsCertificate().extensions.find(id-ce-keyUsage).convertToOperations(extnValue));
    // etc.

    return retn;
}

/*!
 * \brief Returns the filter data associated with this key.
 *
 * Other clients can use the filter data to find this key,
 * if they have permission to access it.  The filter data
 * is a simple map of string field to string value.
 */
Key::FilterData Key::filterData() const
{
    return d_ptr->m_filterData;
}

/*!
 * \brief Returns the filter data value for the given \a field.
 */
QString Key::filterData(const QString &field) const
{
    return d_ptr->m_filterData.value(field);
}

/*!
 * \brief Replaces the filter data in this key with the given \a data.
 *
 * Note that the field "Type" will always have the value "CryptoKey"
 * and this field value cannot be overwritten.
 */
void Key::setFilterData(const Key::FilterData &data)
{
    Key::FilterData v(data);
    v.insert(SAILFISH_SECRETS_SECRET_FILTERDATAFIELDTYPE, SAILFISH_SECRETS_SECRET_TYPECRYPTOKEY);
    d_ptr->m_filterData = v;
}

/*!
 * \brief Sets filter data for the given \a field to the given \a value.
 *
 * Note that the field "Type" will always have the value "CryptoKey"
 * and this field value cannot be overwritten.
 */
void Key::setFilterData(const QString &field, const QString &value)
{
    if (field.compare(SAILFISH_SECRETS_SECRET_FILTERDATAFIELDTYPE, Qt::CaseInsensitive) != 0) {
        d_ptr->m_filterData.insert(field, value);
    }
}

/*!
 * \brief Returns true if the key has a filter data value specified for the given \a field.
 *
 * Note that this function will always return true for the field "Type".
 */
bool Key::hasFilterData(const QString &field)
{
    return d_ptr->m_filterData.contains(field);
}
