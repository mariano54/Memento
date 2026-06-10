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

#include <memory>

#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QSet>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroQmlTask>
#include <QCoroTask>
#else
#include <qcoro/qcorotask.h>
#include <qcoro/qml/qcoroqmltask.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "dict/data/term.h"

class Context;

/**
 * @brief Object for communicating with an ISSEN server for word saves.
 */
class IssenClient : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Create a new IssenClient.
     *
     * @param context The application context.
     * @param parent The parent of this object.
     */
    IssenClient(Context *context, QObject *parent = nullptr);

    /**
     * @brief Log in to the ISSEN server with the configured credentials.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false,
     * nothing otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask login();

    /**
     * @brief Log in to the ISSEN server with the configured credentials.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false,
     * nothing otherwise.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> loginAsync();

    /**
     * @brief Save a term and the current subtitle to ISSEN.
     *
     * @param term The term to save.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false,
     * nothing otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask addTerm(const Term *term);

    /**
     * @brief Save a word and sentence to ISSEN.
     *
     * @param word The word to save.
     * @param sentence The sentence the word appeared in.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false,
     * nothing otherwise.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> addWordAsync(QString word, QString sentence);

    /**
     * @brief Get if a term has already been saved to ISSEN.
     *
     * @param term The term to check.
     * @return true if the term's expression or reading is a saved word,
     * @return false otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE bool isTermSaved(const Term *term) const;

public slots:
    /**
     * @brief Refetch the set of saved words from the ISSEN server.
     * Does nothing when ISSEN integration is disabled.
     */
    void refreshSavedWords();

signals:
    /**
     * @brief Emitted when the set of saved words changes.
     */
    void savedWordsChanged();

private slots:
    /**
     * @brief Update the saved subtitle filter regex.
     *
     * @param filter The regex string to use.
     */
    void updateSubtitleFilterRegex(const QString &filter);

private:
    /**
     * @brief Refetch the set of saved words from the ISSEN server.
     */
    QCoro::Task<> refreshSavedWordsAsync();

    /**
     * @brief Check if the client has a valid ISSEN session.
     *
     * @return true if the session is valid,
     * @return false otherwise.
     */
    [[nodiscard]]
    QCoro::Task<bool> isConnected();

    /**
     * @brief Make a POST request to the ISSEN server.
     *
     * @param path The path of the API endpoint.
     * @param body The JSON body of the request.
     * @return The reply from the server.
     */
    [[nodiscard]]
    QCoro::Task<std::unique_ptr<QNetworkReply>> makeRequest(
        QString path, QJsonObject body);

    /**
     * @brief Extract a human-readable error from a reply.
     *
     * @param reply The reply to extract the error from.
     * @return A human-readable error string.
     */
    [[nodiscard]]
    QString replyError(QNetworkReply &reply) const;

    /**
     * @brief Get the current subtitle text with the search filter applied.
     *
     * @return The current subtitle text. Empty if no subtitle is showing.
     */
    [[nodiscard]]
    QString currentSubtitle() const;

    /* The application context */
    const Context *m_context{nullptr};

    /* The Network Manager for this object. Holds the ISSEN session cookie. */
    QNetworkAccessManager m_manager;

    /* The regular expression to filter subtitles with */
    QRegularExpression m_subtitleFilterRegex;

    /* The words already saved to ISSEN */
    QSet<QString> m_savedWords;
};
