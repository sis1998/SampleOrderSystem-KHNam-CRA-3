#pragma once
#include <optional>
#include <string>
#include <vector>
#include "../domain/Sample.h"

class SampleModel {
public:
    bool add(const Sample& s);
    std::optional<Sample> findById(const std::string& id) const;
    std::vector<Sample> findByName(const std::string& query) const;
    std::vector<Sample> getAll() const;
    bool deductStock(const std::string& id, int qty);
    void addStock(const std::string& id, int qty);
private:
    std::vector<Sample> items_;
};
