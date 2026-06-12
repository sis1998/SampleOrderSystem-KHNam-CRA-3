#pragma once
#include <ostream>
#include <optional>
#include <deque>
#include "../domain/Production.h"

class ProductionView {
public:
    void renderQueue(std::ostream& os, const std::optional<ProductionItem>& current, const std::deque<QueueItem>& queue);
};
