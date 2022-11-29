//------------------------------------------------------------------------------
// Automatically generated by the Fast Binary Encoding compiler, do not modify!
// https://github.com/chronoxor/FastBinaryEncoding
// Source: user_model.fbe
// FBE version: 1.14.1.0
//------------------------------------------------------------------------------

#include "bank_dom.h"

namespace bank_dom {

Transaction::Transaction()
    : counterparty("")
    , receiving(false)
    , amount((uint32_t)0ull)
    , time((uint64_t)0ull)
{}

Transaction::Transaction(const std::string& arg_counterparty, bool arg_receiving, uint32_t arg_amount, uint64_t arg_time)
    : counterparty(arg_counterparty)
    , receiving(arg_receiving)
    , amount(arg_amount)
    , time(arg_time)
{}

bool Transaction::operator==(const Transaction& other) const noexcept
{
    return (
        true
        );
}

bool Transaction::operator<(const Transaction& other) const noexcept
{
    return false;
}

void Transaction::swap(Transaction& other) noexcept
{
    using std::swap;
    swap(counterparty, other.counterparty);
    swap(receiving, other.receiving);
    swap(amount, other.amount);
    swap(time, other.time);
}

std::ostream& operator<<(std::ostream& stream, const Transaction& value)
{
    stream << "Transaction(";
    stream << "counterparty="; stream << "\"" << value.counterparty << "\"";
    stream << ",receiving="; stream << (value.receiving ? "true" : "false");
    stream << ",amount="; stream << value.amount;
    stream << ",time="; stream << value.time;
    stream << ")";
    return stream;
}

Logs::Logs()
    : data()
{}

Logs::Logs(const std::vector<::bank_dom::Transaction>& arg_data)
    : data(arg_data)
{}

bool Logs::operator==(const Logs& other) const noexcept
{
    return (
        true
        );
}

bool Logs::operator<(const Logs& other) const noexcept
{
    return false;
}

void Logs::swap(Logs& other) noexcept
{
    using std::swap;
    swap(data, other.data);
}

std::ostream& operator<<(std::ostream& stream, const Logs& value)
{
    stream << "Logs(";
    {
        bool first = true;
        stream << "data=[" << value.data.size() << "][";
        for (const auto& it : value.data)
        {
            stream << std::string(first ? "" : ",") << it;
            first = false;
        }
        stream << "]";
    }
    stream << ")";
    return stream;
}

User::User()
    : balance((uint32_t)0ull)
    , password((uint64_t)0ull)
    , logs(std::nullopt)
{}

User::User(uint32_t arg_balance, uint64_t arg_password, const std::optional<::bank_dom::Logs>& arg_logs)
    : balance(arg_balance)
    , password(arg_password)
    , logs(arg_logs)
{}

bool User::operator==(const User& other) const noexcept
{
    return (
        true
        );
}

bool User::operator<(const User& other) const noexcept
{
    return false;
}

void User::swap(User& other) noexcept
{
    using std::swap;
    swap(balance, other.balance);
    swap(password, other.password);
    swap(logs, other.logs);
}

std::ostream& operator<<(std::ostream& stream, const User& value)
{
    stream << "User(";
    stream << "balance="; stream << value.balance;
    stream << ",password="; stream << value.password;
    stream << ",logs="; if (value.logs) stream << *value.logs; else stream << "null";
    stream << ")";
    return stream;
}

Global::Global()
    : keys()
    , users()
{}

Global::Global(const std::vector<std::string>& arg_keys, const std::vector<::bank_dom::User>& arg_users)
    : keys(arg_keys)
    , users(arg_users)
{}

bool Global::operator==(const Global& other) const noexcept
{
    return (
        true
        );
}

bool Global::operator<(const Global& other) const noexcept
{
    return false;
}

void Global::swap(Global& other) noexcept
{
    using std::swap;
    swap(keys, other.keys);
    swap(users, other.users);
}

std::ostream& operator<<(std::ostream& stream, const Global& value)
{
    stream << "Global(";
    {
        bool first = true;
        stream << "keys=[" << value.keys.size() << "][";
        for (const auto& it : value.keys)
        {
            stream << std::string(first ? "" : ",") << "\"" << it << "\"";
            first = false;
        }
        stream << "]";
    }
    {
        bool first = true;
        stream << ",users=[" << value.users.size() << "][";
        for (const auto& it : value.users)
        {
            stream << std::string(first ? "" : ",") << it;
            first = false;
        }
        stream << "]";
    }
    stream << ")";
    return stream;
}

} // namespace bank_dom
