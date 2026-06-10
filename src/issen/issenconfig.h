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

#pragma once

#include <QObject>

#include "issen/issenconfigkeys.h"

/**
 * @brief Manages reading and writing the ISSEN configuration file as well as
 * modeling its contents.
 */
class IssenConfig : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool enabled
        READ enabled
        WRITE setEnabled
        NOTIFY enabledChanged
    )

    Q_PROPERTY(
        QString server
        READ server
        WRITE setServer
        NOTIFY serverChanged
    )

    Q_PROPERTY(
        QString username
        READ username
        WRITE setUsername
        NOTIFY usernameChanged
    )

    Q_PROPERTY(
        QString password
        READ password
        WRITE setPassword
        NOTIFY passwordChanged
    )

    Q_PROPERTY(
        QString language
        READ language
        WRITE setLanguage
        NOTIFY languageChanged
    )

public:
    /**
     * @brief Construct a new IssenConfig.
     *
     * @param path The path to the config file. It will be created if it
     * doesn't exist.
     * @param parent The parent of this object.
     */
    IssenConfig(const QString &path, QObject *parent = nullptr);
    virtual ~IssenConfig();

    /**
     * @brief Load settings from the config file.
     *
     * @return true if the config was loaded,
     * @return false on error.
     */
    Q_INVOKABLE bool load();

    /**
     * @brief Write settings to the config file.
     *
     * @return true if the config was written,
     * @return false on error.
     */
    Q_INVOKABLE bool write();

    /**
     * @brief Get if ISSEN integration is enabled.
     *
     * @return true if enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool enabled() const noexcept;

    /**
     * @brief Set if ISSEN integration is enabled.
     *
     * @param value true if enabled, false otherwise.
     */
    void setEnabled(bool value);

    /**
     * @brief Get the ISSEN server URL.
     *
     * @return The ISSEN server URL without a trailing slash.
     */
    [[nodiscard]]
    const QString &server() const noexcept;

    /**
     * @brief Set the ISSEN server URL.
     *
     * @param value The new ISSEN server URL.
     */
    void setServer(const QString &value);

    /**
     * @brief Get the ISSEN account username.
     *
     * @return The ISSEN account username.
     */
    [[nodiscard]]
    const QString &username() const noexcept;

    /**
     * @brief Set the ISSEN account username.
     *
     * @param value The new ISSEN account username.
     */
    void setUsername(const QString &value);

    /**
     * @brief Get the ISSEN account password.
     *
     * @return The ISSEN account password.
     */
    [[nodiscard]]
    const QString &password() const noexcept;

    /**
     * @brief Set the ISSEN account password.
     *
     * @param value The new ISSEN account password.
     */
    void setPassword(const QString &value);

    /**
     * @brief Get the language words are saved under.
     *
     * @return The language words are saved under.
     */
    [[nodiscard]]
    const QString &language() const noexcept;

    /**
     * @brief Set the language words are saved under.
     *
     * @param value The new language.
     */
    void setLanguage(const QString &value);

signals:
    /**
     * @brief Emitted when ISSEN integration enabled is changed.
     *
     * @param value The new enabled value.
     */
    void enabledChanged(bool value);

    /**
     * @brief Emitted when the server URL is changed.
     *
     * @param value The new server URL.
     */
    void serverChanged(const QString &value);

    /**
     * @brief Emitted when the username is changed.
     *
     * @param value The new username.
     */
    void usernameChanged(const QString &value);

    /**
     * @brief Emitted when the password is changed.
     *
     * @param value The new password.
     */
    void passwordChanged(const QString &value);

    /**
     * @brief Emitted when the language is changed.
     *
     * @param value The new language.
     */
    void languageChanged(const QString &value);

private:
    /* Path to the config file */
    const QString m_path;

    /* true if ISSEN integration is enabled */
    bool m_enabled{Issen::Keys::ENABLED_DEFAULT};

    /* The ISSEN server URL */
    QString m_server{Issen::Keys::SERVER_DEFAULT};

    /* The ISSEN account username */
    QString m_username{Issen::Keys::USERNAME_DEFAULT};

    /* The ISSEN account password */
    QString m_password{Issen::Keys::PASSWORD_DEFAULT};

    /* The language words are saved under */
    QString m_language{Issen::Keys::LANGUAGE_DEFAULT};
};
