#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BLU");
    BOOST_CHECK(GetBoolArg("-BLU"));
    BOOST_CHECK(GetBoolArg("-BLU", false));
    BOOST_CHECK(GetBoolArg("-BLU", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BLUo"));
    BOOST_CHECK(!GetBoolArg("-BLUo", false));
    BOOST_CHECK(GetBoolArg("-BLUo", true));

    ResetArgs("-BLU=0");
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", false));
    BOOST_CHECK(!GetBoolArg("-BLU", true));

    ResetArgs("-BLU=1");
    BOOST_CHECK(GetBoolArg("-BLU"));
    BOOST_CHECK(GetBoolArg("-BLU", false));
    BOOST_CHECK(GetBoolArg("-BLU", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBLU");
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", false));
    BOOST_CHECK(!GetBoolArg("-BLU", true));

    ResetArgs("-noBLU=1");
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", false));
    BOOST_CHECK(!GetBoolArg("-BLU", true));

    ResetArgs("-BLU -noBLU");  // -BLU should win
    BOOST_CHECK(GetBoolArg("-BLU"));
    BOOST_CHECK(GetBoolArg("-BLU", false));
    BOOST_CHECK(GetBoolArg("-BLU", true));

    ResetArgs("-BLU=1 -noBLU=1");  // -BLU should win
    BOOST_CHECK(GetBoolArg("-BLU"));
    BOOST_CHECK(GetBoolArg("-BLU", false));
    BOOST_CHECK(GetBoolArg("-BLU", true));

    ResetArgs("-BLU=0 -noBLU=0");  // -BLU should win
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", false));
    BOOST_CHECK(!GetBoolArg("-BLU", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BLU=1");
    BOOST_CHECK(GetBoolArg("-BLU"));
    BOOST_CHECK(GetBoolArg("-BLU", false));
    BOOST_CHECK(GetBoolArg("-BLU", true));

    ResetArgs("--noBLU=1");
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", false));
    BOOST_CHECK(!GetBoolArg("-BLU", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BLU", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BLU", "eleven"), "eleven");

    ResetArgs("-BLU -bar");
    BOOST_CHECK_EQUAL(GetArg("-BLU", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BLU", "eleven"), "");

    ResetArgs("-BLU=");
    BOOST_CHECK_EQUAL(GetArg("-BLU", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BLU", "eleven"), "");

    ResetArgs("-BLU=11");
    BOOST_CHECK_EQUAL(GetArg("-BLU", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BLU", "eleven"), "11");

    ResetArgs("-BLU=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BLU", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BLU", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BLU", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BLU", 0), 0);

    ResetArgs("-BLU -bar");
    BOOST_CHECK_EQUAL(GetArg("-BLU", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BLU=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BLU", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BLU=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BLU", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BLU");
    BOOST_CHECK_EQUAL(GetBoolArg("-BLU"), true);

    ResetArgs("--BLU=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BLU", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBLU");
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", true));
    BOOST_CHECK(!GetBoolArg("-BLU", false));

    ResetArgs("-noBLU=1");
    BOOST_CHECK(!GetBoolArg("-BLU"));
    BOOST_CHECK(!GetBoolArg("-BLU", true));
    BOOST_CHECK(!GetBoolArg("-BLU", false));

    ResetArgs("-noBLU=0");
    BOOST_CHECK(GetBoolArg("-BLU"));
    BOOST_CHECK(GetBoolArg("-BLU", true));
    BOOST_CHECK(GetBoolArg("-BLU", false));

    ResetArgs("-BLU --noBLU");
    BOOST_CHECK(GetBoolArg("-BLU"));

    ResetArgs("-noBLU -BLU"); // BLU always wins:
    BOOST_CHECK(GetBoolArg("-BLU"));
}

BOOST_AUTO_TEST_SUITE_END()
