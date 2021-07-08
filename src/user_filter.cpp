#include "user_filter.h"

template <bool set_body_flag, bool require_admin>
UserFilter<set_body_flag, require_admin>::UserFilter(Bank &b) : bank(b) {}

template <bool set_body_flag, bool require_admin>
void UserFilter<set_body_flag, require_admin>::doFilter(const HttpRequestPtr &req,
                                                        FilterCallback &&fcb,
                                                        FilterChainCallback &&fccb)
{
    static thread_local std::string_view auth_header = req->getHeader("Authorization");
    if (auth_header.size() > 6 && auth_header.size() <= 517) //"Basic " + username + ':' + password
    {
        if (auth_header.substr(0, 6) == "Basic ")
        {
            static thread_local std::string_view base64_input = auth_header.substr(6);
            static thread_local std::array<char, 384> base64_result; //(255 username + ':' + 255 password) * 3/4
            static thread_local size_t new_sz;
            base64_decode(base64_input.data(), base64_input.size(), base64_result.begin(), &new_sz, 0);

            static thread_local std::string_view results_view(base64_result.begin(), new_sz);
            static thread_local std::size_t middle = results_view.find(':');
            if (middle != std::string::npos)
            {
                static thread_local std::string username;
                string_view_to_string(username, results_view.substr(0, middle));
                if constexpr (require_admin)
                {
                    if (bank.AdminVerifyAccount(username))
                    {
                        static thread_local std::string password;
                        string_view_to_string(password, results_view.substr(middle + 1));
                        if (bank.VerifyPassword(username, password))
                        {
                            fccb();
                            return;
                        }
                    }
                }
                else
                {
                    static thread_local std::string password;
                    string_view_to_string(password, results_view.substr(middle + 1));
                    if (bank.VerifyPassword(username, results_view.substr(middle + 1)))
                    {
                        if constexpr (set_body_flag)
                        {
                            req->setParameter("name", username);
                        }
                        fccb();
                        return;
                    }
                }
            }
        }
    }
    static thread_local const auto &resp = HttpResponse::newCustomHttpResponse(BankResponse(k401Unauthorized, "\"Invalid Credentials\""));
    fcb(resp);
}

template class UserFilter<true, false>;  //user default
template class UserFilter<false, false>; //user sparse
template class UserFilter<false, true>;  //admin