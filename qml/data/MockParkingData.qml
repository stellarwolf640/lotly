pragma Singleton

import QtQuick

// Central mock data source used to populate the demo UI immediately.
QtObject {
    id: root

    readonly property var lots: [
        {
            "id": "lot-a-north",
            "name": "Lot A -- North",
            "percentOpen": 72,
            "confidenceLabel": "High",
            "confidenceScore": 0.91,
            "statusColor": "#2D6DCC",
            "explanation": "High confidence data, 72% open, historically stable at this time on Wednesdays.",
            "hourlyAvailability": [22, 38, 61, 72, 49, 31, 24],
            "recentReports": 14,
            "bestPick": true
        },
        {
            "id": "lot-b-science",
            "name": "Lot B -- Science",
            "percentOpen": 41,
            "confidenceLabel": "Medium",
            "confidenceScore": 0.67,
            "statusColor": "#D6A51D",
            "explanation": "Midday traffic is steady, but class changes make this lot less predictable right now.",
            "hourlyAvailability": [33, 45, 52, 41, 37, 29, 24],
            "recentReports": 9,
            "bestPick": false
        },
        {
            "id": "lot-c-library",
            "name": "Lot C -- Library",
            "percentOpen": 63,
            "confidenceLabel": "Medium",
            "confidenceScore": 0.7,
            "statusColor": "#2D6DCC",
            "explanation": "Library demand is moderate this morning, with enough turnover to keep spaces opening up.",
            "hourlyAvailability": [41, 48, 56, 63, 58, 43, 32],
            "recentReports": 11,
            "bestPick": false
        },
        {
            "id": "lot-d-visitor",
            "name": "Lot D -- Visitor",
            "percentOpen": 55,
            "confidenceLabel": "Medium",
            "confidenceScore": 0.64,
            "statusColor": "#2D6DCC",
            "explanation": "Visitor parking has moderate open space with a gradual decline expected through late morning.",
            "hourlyAvailability": [39, 46, 53, 55, 47, 34, 28],
            "recentReports": 6,
            "bestPick": false
        },
        {
            "id": "lot-e-east-deck",
            "name": "Lot E -- East Deck",
            "percentOpen": 23,
            "confidenceLabel": "Low",
            "confidenceScore": 0.39,
            "statusColor": "#D5523F",
            "explanation": "East Deck is filling quickly, with low remaining capacity and weaker confidence in turnover.",
            "hourlyAvailability": [44, 37, 29, 23, 18, 16, 14],
            "recentReports": 4,
            "bestPick": false
        }
    ]

    function lotById(lotId) {
        for (var index = 0; index < lots.length; ++index) {
            if (lots[index].id === lotId) {
                return lots[index]
            }
        }
        return lots[0]
    }

    function bestLot() {
        return lots[0]
    }

    function backupLot() {
        return lots[3]
    }
}
