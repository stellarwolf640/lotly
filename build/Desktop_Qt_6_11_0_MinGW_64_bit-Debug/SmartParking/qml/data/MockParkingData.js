.pragma library

var lots = [
    {
        id: "lot-a-north",
        name: "Lot A -- North",
        percentOpen: 72,
        confidenceLabel: "High",
        confidenceScore: 0.91,
        statusColor: "#2D6DCC",
        explanation: "High confidence data, 72% open, historically stable at this time on Wednesdays.",
        hourlyAvailability: [88, 82, 70, 60, 55, 58, 62, 68, 70, 72, 68, 65, 62, 68, 74, 80, 85, 88, 92],
        recentReports: 14,
        bestPick: true
    },
    {
        id: "lot-b-science",
        name: "Lot B -- Science",
        percentOpen: 41,
        confidenceLabel: "Medium",
        confidenceScore: 0.67,
        statusColor: "#D6A51D",
        explanation: "Why recommended: Moderate confidence data with steady turnover near class change windows.",
        hourlyAvailability: [80, 72, 55, 44, 40, 38, 41, 45, 43, 41, 40, 42, 48, 55, 60, 65, 68, 70, 75],
        recentReports: 9,
        bestPick: false
    },
    {
        id: "lot-c-library",
        name: "Lot C -- Library",
        percentOpen: 63,
        confidenceLabel: "Medium",
        confidenceScore: 0.70,
        statusColor: "#2D6DCC",
        explanation: "Why recommended: Library demand is moderate this morning, with enough turnover to keep spaces opening up.",
        hourlyAvailability: [85, 78, 68, 60, 58, 60, 63, 65, 68, 63, 60, 58, 55, 60, 65, 70, 73, 75, 80],
        recentReports: 11,
        bestPick: false
    },
    {
        id: "lot-d-visitor",
        name: "Lot D -- Visitor",
        percentOpen: 55,
        confidenceLabel: "Medium",
        confidenceScore: 0.64,
        statusColor: "#2D6DCC",
        explanation: "Why recommended: Visitor parking remains moderately open with useful backup availability right now.",
        hourlyAvailability: [75, 68, 58, 52, 50, 52, 55, 58, 60, 55, 52, 50, 52, 56, 60, 64, 67, 69, 72],
        recentReports: 6,
        bestPick: false
    },
    {
        id: "lot-e-east-deck",
        name: "Lot E -- East Deck",
        percentOpen: 23,
        confidenceLabel: "Low",
        confidenceScore: 0.39,
        statusColor: "#D5523F",
        explanation: "Why recommended: East Deck is filling quickly, with low remaining capacity and weaker confidence in turnover.",
        hourlyAvailability: [62, 52, 38, 30, 26, 24, 23, 22, 20, 23, 25, 28, 32, 35, 38, 40, 42, 44, 48],
        recentReports: 4,
        bestPick: false
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
