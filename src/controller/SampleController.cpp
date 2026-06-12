#include "SampleController.h"
#include <string>

SampleController::SampleController(SampleModel& model)
    : model_(model) {}

void SampleController::run(std::istream& in, std::ostream& out) {
    std::string choice;
    while (true) {
        out << "\n[시료 관리]\n"
            << "1. 시료 등록\n"
            << "2. 목록 조회\n"
            << "3. 이름 검색\n"
            << "0. 뒤로\n"
            << "선택: ";
        if (!std::getline(in, choice)) break;
        if (choice == "0") {
            break;
        } else if (choice == "1") {
            Sample s;
            out << "시료 ID: ";
            std::getline(in, s.sampleId);
            out << "이름: ";
            std::getline(in, s.name);
            out << "평균 생산 시간: ";
            std::string tmp;
            std::getline(in, tmp);
            s.avgProductionTime = std::stod(tmp);
            out << "수율: ";
            std::getline(in, tmp);
            s.yield = std::stod(tmp);
            out << "재고: ";
            std::getline(in, tmp);
            s.stock = std::stoi(tmp);
            model_.add(s);
            view_.renderMessage(out, "시료가 등록되었습니다.");
        } else if (choice == "2") {
            view_.renderList(out, model_.getAll());
        } else if (choice == "3") {
            out << "검색어: ";
            std::string query;
            std::getline(in, query);
            view_.renderList(out, model_.findByName(query));
        }
    }
}
