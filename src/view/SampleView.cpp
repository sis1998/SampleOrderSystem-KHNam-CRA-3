#include "SampleView.h"

void SampleView::renderList(std::ostream& os, const std::vector<Sample>& samples) {
    os << "=== 시료 목록 ===\n";
    for (const auto& s : samples) {
        os << "[" << s.sampleId << "] " << s.name
           << " | 재고: " << s.stock << "\n";
    }
}

void SampleView::renderIndexedList(std::ostream& os, const std::vector<Sample>& samples) {
    os << "=== 시료 목록 ===\n";
    for (int i = 0; i < static_cast<int>(samples.size()); ++i) {
        const auto& s = samples[i];
        os << " " << (i + 1) << ". [" << s.sampleId << "] " << s.name
           << " | 재고: " << s.stock << "\n";
    }
}

void SampleView::renderMessage(std::ostream& os, std::string_view message) {
    os << message << "\n";
}
