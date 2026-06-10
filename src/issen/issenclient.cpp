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

#include "issen/issenclient.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroNetworkReply>
#else
#include <qcoro/network/qcoronetworkreply.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "issen/issenconfig.h"
#include "state/context.h"

/* The Client-Version header value sent with every request */
static constexpr const char *ISSEN_CLIENT_VERSION = "1.13.6";

namespace Key
{

static constexpr const char *SUCCESS = "success";
static constexpr const char *ERROR = "error";

} // namespace Key

/* Begin Constructor/Destructors */

IssenClient::IssenClient(Context *context, QObject *parent) :
    QObject(parent),
    m_context(context),
    m_manager(this),
    m_subtitleFilterRegex(m_context->settings()->searchRemoveRegex())
{
    constexpr int TIMEOUT = 10000;
    m_manager.setTransferTimeout(TIMEOUT);

    connect(
        m_context->settings(), &Settings::searchRemoveRegexChanged,
        this, &IssenClient::updateSubtitleFilterRegex
    );
}

/* End Constructor/Destructors */
/* Begin Slots */

void IssenClient::updateSubtitleFilterRegex(const QString &filter)
{
    m_subtitleFilterRegex = QRegularExpression(filter);
}

/* End Slots */
/* Begin Commands */

QCoro::QmlTask IssenClient::login()
{
    return loginAsync();
}

QCoro::Task<QVariantMap> IssenClient::loginAsync()
{
    const IssenConfig *config = m_context->issenConfig();

    QVariantMap result;
    if (config->username().isEmpty() || config->password().isEmpty())
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("ISSEN username and password are required");
        co_return result;
    }

    QByteArray prehashed = QCryptographicHash::hash(
        config->password().toUtf8(), QCryptographicHash::Sha256
    ).toHex();

    QJsonObject body;
    body["username"] = config->username();
    body["prehashed"] = QString::fromUtf8(prehashed);
    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest("/login", std::move(body));

    if (reply->error() != QNetworkReply::NoError)
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = replyError(*reply);
        co_return result;
    }

    QJsonObject replyObj = QJsonDocument::fromJson(reply->readAll()).object();
    if (!replyObj[Key::SUCCESS].toBool(false))
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("ISSEN login failed");
        co_return result;
    }

    result[Key::SUCCESS] = true;
    co_return result;
}

QCoro::QmlTask IssenClient::addTerm(const Term *term)
{
    QString word = term->readingAsExpression() ?
        term->reading() : term->expression();
    if (word.isEmpty())
    {
        word = term->readingAsExpression() ?
            term->expression() : term->reading();
    }
    return addWordAsync(std::move(word), currentSubtitle());
}

QCoro::Task<QVariantMap> IssenClient::addWordAsync(
    QString word, QString sentence)
{
    QVariantMap result;
    if (word.isEmpty())
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = tr("No word to add to ISSEN");
        co_return result;
    }

    if (!co_await isConnected())
    {
        QVariantMap loginResult = co_await loginAsync();
        if (!loginResult[Key::SUCCESS].toBool())
        {
            co_return loginResult;
        }
    }

    QJsonObject wordObj;
    wordObj["word"] = word.trimmed();
    wordObj["sentence"] = sentence.trimmed();
    QJsonObject body;
    body["language"] = m_context->issenConfig()->language().trimmed().toLower();
    body["words"] = QJsonArray{wordObj};
    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest("/add_words", std::move(body));

    if (reply->error() != QNetworkReply::NoError)
    {
        result[Key::SUCCESS] = false;
        result[Key::ERROR] = replyError(*reply);
        co_return result;
    }

    result[Key::SUCCESS] = true;
    co_return result;
}

/* End Commands */
/* Begin Network Helpers */

QCoro::Task<bool> IssenClient::isConnected()
{
    std::unique_ptr<QNetworkReply> reply =
        co_await makeRequest("/get_user", QJsonObject());
    if (reply->error() != QNetworkReply::NoError)
    {
        co_return false;
    }
    QJsonObject replyObj = QJsonDocument::fromJson(reply->readAll()).object();
    co_return replyObj["user"].isObject();
}

QCoro::Task<std::unique_ptr<QNetworkReply>> IssenClient::makeRequest(
    QString path, QJsonObject body)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_context->issenConfig()->server() + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Client-Platform", "web");
    request.setRawHeader("Client-Version", ISSEN_CLIENT_VERSION);

    co_return std::unique_ptr<QNetworkReply>{
        co_await m_manager.post(request, QJsonDocument(body).toJson())
    };
}

QString IssenClient::replyError(QNetworkReply &reply) const
{
    QJsonObject replyObj = QJsonDocument::fromJson(reply.readAll()).object();
    for (const char *key : {"message", "error"})
    {
        if (replyObj[key].isString())
        {
            return replyObj[key].toString();
        }
    }
    return reply.errorString();
}

/* End Network Helpers */
/* Begin Expression Helpers */

QString IssenClient::currentSubtitle() const
{
    if (m_context->player() == nullptr)
    {
        return {};
    }
    QString subtitleText = m_context->player()->state()->subtitle()->text();
    return subtitleText.remove(m_subtitleFilterRegex);
}

/* End Expression Helpers */
