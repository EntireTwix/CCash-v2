int main(int argc, char **argv)
{
    bank.AddUser("twix", 0, "root");
    bank.AddUser("jolly", 0, "root");
    bank.admin_account = "twix";
    Op_a(bank.AddUser("", 0, ""), "add user: ", 100000, bank.DelUser(""));
    Op(bank.AddBal("twix", 1), "give bal: ", 100000);
    Op(bank.SubBal("twix", 1), "sub bal: ", 100000);
    Op(bank.SetBal("twix", 100000), "set bal: ", 100000);
    Op(bank.SendFunds("twix", "jolly", 1), "send funds: ", 100000);
    Op(bank.SendFunds("twix", "twix", 1), "invalid send funds: ", 100000);

    bank.AddUser("", 0, "");
    Op_a(bank.DelUser(""), "del user: ", 100000, bank.AddUser("", 0, ""));
    bank.DelUser("");

    Op(bank.Contains("twix"), "contains: ", 100000);
    Op(bank.AdminVerifyAccount("twix"), "admin verify pass: ", 100000);
    Op(bank.GetBal("twix"), "get bal: ", 100000);
    Op(bank.VerifyPassword("twix", "root"), "verify pass: ", 100000);
    Op(bank.ChangePassword("twix", "root"), "change pass: ", 100000);
    Op(bank.GetLogs("twix"), "get logs: ", 10000);
    Op(bank.Save(), "saving: ", 1);
#if CONSERVATIVE_DISK_SAVE
    Op(bank.GetChangeState(), "change flag: ", 10000);
#endif

    //GetBal scalining test
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (size_t i = 0; i < 10000000; ++i)
    {
        bank.AddUser(std::to_string(i), 100000, "root");
        if (i % 10000 == 0)
        {
            auto u = std::to_string((int)(distribution(generator) * i));
            Op(bank.GetBal(u), std::to_string(i) + ", ", 100000);
        }
    }

    return 0;
}
