////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "issen/issenconfig.h"

#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>

IssenConfig::IssenConfig(const QString &path, QObject *parent) :
    QObject(parent),
    m_path(path)
{
    load();
}

IssenConfig::~IssenConfig()
{
    write();
}

bool IssenConfig::load()
{
    QFile configFile(m_path);
    if (!configFile.exists())
    {
        return true;
    }

    if (!configFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(configFile.readAll());
    configFile.close();

    if (!jsonDoc.isObject())
    {
        return false;
    }
    QJsonObject jsonObj = jsonDoc.object();

    setEnabled(
        jsonObj[Issen::Keys::ENABLED].toBool(Issen::Keys::ENABLED_DEFAULT)
    );
    setServer(
        jsonObj[Issen::Keys::SERVER].toString(Issen::Keys::SERVER_DEFAULT)
    );
    setUsername(
        jsonObj[Issen::Keys::USERNAME].toString(Issen::Keys::USERNAME_DEFAULT)
    );
    setPassword(
        jsonObj[Issen::Keys::PASSWORD].toString(Issen::Keys::PASSWORD_DEFAULT)
    );
    setLanguage(
        jsonObj[Issen::Keys::LANGUAGE].toString(Issen::Keys::LANGUAGE_DEFAULT)
    );

    return true;
}

bool IssenConfig::write()
{
    QFile configFile(m_path);
    if (!configFile.open(QIODevice::ReadWrite |
                         QIODevice::Truncate |
                         QIODevice::Text))
    {
        return false;
    }

    QJsonObject jsonObj;
    jsonObj[Issen::Keys::ENABLED] = enabled();
    jsonObj[Issen::Keys::SERVER] = server();
    jsonObj[Issen::Keys::USERNAME] = username();
    jsonObj[Issen::Keys::PASSWORD] = password();
    jsonObj[Issen::Keys::LANGUAGE] = language();

    configFile.write(QJsonDocument(jsonObj).toJson());
    configFile.close();

    return true;
}

bool IssenConfig::enabled() const noexcept
{
    return m_enabled;
}

void IssenConfig::setEnabled(bool value)
{
    if (m_enabled == value)
    {
        return;
    }
    m_enabled = value;
    emit enabledChanged(m_enabled);
}

const QString &IssenConfig::server() const noexcept
{
    return m_server;
}

void IssenConfig::setServer(const QString &value)
{
    QString server = value;
    while (server.endsWith('/'))
    {
        server.chop(1);
    }
    if (m_server == server)
    {
        return;
    }
    m_server = server;
    emit serverChanged(m_server);
}

const QString &IssenConfig::username() const noexcept
{
    return m_username;
}

void IssenConfig::setUsername(const QString &value)
{
    if (m_username == value)
    {
        return;
    }
    m_username = value;
    emit usernameChanged(m_username);
}

const QString &IssenConfig::password() const noexcept
{
    return m_password;
}

void IssenConfig::setPassword(const QString &value)
{
    if (m_password == value)
    {
        return;
    }
    m_password = value;
    emit passwordChanged(m_password);
}

const QString &IssenConfig::language() const noexcept
{
    return m_language;
}

void IssenConfig::setLanguage(const QString &value)
{
    if (m_language == value)
    {
        return;
    }
    m_language = value;
    emit languageChanged(m_language);
}
