#pragma once
#include <string>
#include "../model/OrderModel.h"

class OrderRepository {
public:
    void load(OrderModel& model, const std::string& path);
    void save(const OrderModel& model, const std::string& path);
};
