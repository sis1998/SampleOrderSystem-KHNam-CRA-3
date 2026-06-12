#include "SampleView.h"

void SampleView::renderList(std::ostream& os, const std::vector<Sample>& samples) {
    os << "=== 시료 목록 ===\n";
    for (const auto& s : samples) {
        os << "[" << s.sampleId << "] " << s.name
           << " | 재고: " << s.stock << "\n";
    }
}

void SampleView::renderMessage(std::ostream& os, std::string_view message) {
    os << message << "\n";
}
