#include "MainView.h"

void MainView::renderHeader(std::ostream& os, int sampleCount, int totalStock, int orderCount, int queueSize) {
    os << "=== 주문 생산 관리 시스템 ===\n";
    os << "시료: " << sampleCount
       << " | 재고: " << totalStock
       << " | 주문: " << orderCount
       << " | 생산대기: " << queueSize << "\n";
    os << "============================\n";
}

void MainView::renderMenu(std::ostream& os) {
    os << "1. 시료 관리\n";
    os << "2. 시료 주문\n";
    os << "3. 주문 승인/거절\n";
    os << "4. 모니터링\n";
    os << "5. 생산라인 조회\n";
    os << "6. 출고 처리\n";
    os << "0. 종료\n";
    os << "선택: ";
}
