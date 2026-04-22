#include "autocompleteservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include "mapsservice.h"

AutocompleteService::AutocompleteService(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{
    connect(m_network, &QNetworkAccessManager::finished,
            this, &AutocompleteService::onReplyFinished);
}

void AutocompleteService::fetchSuggestions(const QString &input,
                                           double biasLat,
                                           double biasLng,
                                           double biasRadiusMeters)
{
    // Places API (New) — Autocomplete
    // POST https://places.googleapis.com/v1/places:autocomplete
    // Docs: https://developers.google.com/maps/documentation/places/web-service/place-autocomplete
    QNetworkRequest request(
        QUrl(QStringLiteral("https://places.googleapis.com/v1/places:autocomplete")));

    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setHeader(QNetworkRequest::UserAgentHeader,   QStringLiteral("LotlyApp/1.0"));
    request.setRawHeader("X-Goog-Api-Key", MapsService::PLACEHOLDER_API_KEY.toUtf8());

    QJsonObject body;
    body[QStringLiteral("input")] = input;

    // Bias toward the current search area when coordinates are available
    if (biasLat != 0.0 || biasLng != 0.0) {
        QJsonObject center;
        center[QStringLiteral("latitude")]  = biasLat;
        center[QStringLiteral("longitude")] = biasLng;
        body[QStringLiteral("locationBias")] = QJsonObject{{
            QStringLiteral("circle"), QJsonObject{
                {QStringLiteral("center"), center},
                {QStringLiteral("radius"), biasRadiusMeters}
            }
        }};
    }

    m_network->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
}

void AutocompleteService::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(
            QStringLiteral("Network error: %1").arg(reply->errorString()));
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull()) {
        emit suggestionsReady({});
        return;
    }

    const QJsonObject root = doc.object();

    if (root.contains(QStringLiteral("error"))) {
        emit errorOccurred(
            root.value(QStringLiteral("error")).toObject()
                .value(QStringLiteral("message")).toString());
        return;
    }

    QVariantList suggestions;
    const QJsonArray items = root.value(QStringLiteral("suggestions")).toArray();

    for (const QJsonValue &val : items) {
        const QJsonObject prediction =
            val.toObject().value(QStringLiteral("placePrediction")).toObject();
        if (prediction.isEmpty())
            continue;

        const QString placeId  = prediction.value(QStringLiteral("placeId")).toString();
        const QString fullText = prediction.value(QStringLiteral("text")).toObject()
                                           .value(QStringLiteral("text")).toString();

        const QJsonObject sf      = prediction.value(QStringLiteral("structuredFormat")).toObject();
        const QString mainText    = sf.value(QStringLiteral("mainText")).toObject()
                                      .value(QStringLiteral("text")).toString();
        const QString secondaryText = sf.value(QStringLiteral("secondaryText")).toObject()
                                        .value(QStringLiteral("text")).toString();

        if (placeId.isEmpty() || fullText.isEmpty())
            continue;

        QVariantMap item;
        item[QStringLiteral("placeId")]       = placeId;
        item[QStringLiteral("fullText")]      = fullText;
        item[QStringLiteral("mainText")]      = mainText.isEmpty() ? fullText : mainText;
        item[QStringLiteral("secondaryText")] = secondaryText;
        suggestions.append(item);
    }

    emit suggestionsReady(suggestions);
}
