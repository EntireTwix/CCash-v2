#pragma once
#include <array>
#include <algorithm>
#include "log_consts.hpp"
#include "transactions.hpp"

struct Log
{
    std::vector<Transaction> data;
    uint32_t end = 0;
    void AddTrans(Transaction &&v)
    {
        if (data.size() <= end+1 && end+1 <= max_log_size) //if memory reserved is full
        {
            data.resize(data.size() + pre_log_size); //prefetching memory
        }
        for (uint32_t i = end; i > 0; --i)
        {
            data[i] = std::move(data[i - 1]);
        }
        data[0] = std::move(v);
        end += (end < max_log_size); //branchless
    }
    Json::Value Serialize() const
    {
        Json::Value res;
        for (uint32_t i = 0; i < end; ++i)
        {
            res[i]["to"] = data[i].to;
            res[i]["from"] = data[i].from;
            res[i]["amount"] = (Json::UInt)data[i].amount;
            res[i]["time"] = (Json::UInt64)data[i].time;
        }
        return res;
    }
};