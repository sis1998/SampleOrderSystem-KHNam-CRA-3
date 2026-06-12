#pragma once
#include <nlohmann/json.hpp>
#include "../domain/Sample.h"
#include "../domain/Order.h"
#include "../domain/Production.h"

nlohmann::json toJson(const Sample&);
Sample sampleFromJson(const nlohmann::json&);

nlohmann::json toJson(const Order&);
Order orderFromJson(const nlohmann::json&);

nlohmann::json toJson(const QueueItem&);
QueueItem queueItemFromJson(const nlohmann::json&);

nlohmann::json toJson(const ProductionItem&);
ProductionItem productionItemFromJson(const nlohmann::json&);
