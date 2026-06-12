#include "MainView.h"

void MainView::renderHeader(std::ostream& os, int sampleCount, int totalStock, int orderCount, int queueSize) {
    os << "=== Order Production System ===\n";
    os << "Samples: " << sampleCount
       << " | Stock: " << totalStock
       << " | Orders: " << orderCount
       << " | Queue: " << queueSize << "\n";
    os << "================================\n";
}

void MainView::renderMenu(std::ostream& os) {
    os << "1. Sample list\n";
    os << "2. Place order\n";
    os << "3. Reserved orders\n";
    os << "4. Confirmed orders\n";
    os << "5. Production monitor\n";
    os << "6. Production queue\n";
    os << "0. Exit\n";
    os << "Select: ";
}
