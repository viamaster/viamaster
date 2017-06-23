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
    ResetArgs("-via");
    BOOST_CHECK(GetBoolArg("-via"));
    BOOST_CHECK(GetBoolArg("-via", false));
    BOOST_CHECK(GetBoolArg("-via", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-viao"));
    BOOST_CHECK(!GetBoolArg("-viao", false));
    BOOST_CHECK(GetBoolArg("-viao", true));

    ResetArgs("-via=0");
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", false));
    BOOST_CHECK(!GetBoolArg("-via", true));

    ResetArgs("-via=1");
    BOOST_CHECK(GetBoolArg("-via"));
    BOOST_CHECK(GetBoolArg("-via", false));
    BOOST_CHECK(GetBoolArg("-via", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-novia");
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", false));
    BOOST_CHECK(!GetBoolArg("-via", true));

    ResetArgs("-novia=1");
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", false));
    BOOST_CHECK(!GetBoolArg("-via", true));

    ResetArgs("-via -novia");  // -via should win
    BOOST_CHECK(GetBoolArg("-via"));
    BOOST_CHECK(GetBoolArg("-via", false));
    BOOST_CHECK(GetBoolArg("-via", true));

    ResetArgs("-via=1 -novia=1");  // -via should win
    BOOST_CHECK(GetBoolArg("-via"));
    BOOST_CHECK(GetBoolArg("-via", false));
    BOOST_CHECK(GetBoolArg("-via", true));

    ResetArgs("-via=0 -novia=0");  // -via should win
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", false));
    BOOST_CHECK(!GetBoolArg("-via", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--via=1");
    BOOST_CHECK(GetBoolArg("-via"));
    BOOST_CHECK(GetBoolArg("-via", false));
    BOOST_CHECK(GetBoolArg("-via", true));

    ResetArgs("--novia=1");
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", false));
    BOOST_CHECK(!GetBoolArg("-via", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-via", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-via", "eleven"), "eleven");

    ResetArgs("-via -bar");
    BOOST_CHECK_EQUAL(GetArg("-via", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-via", "eleven"), "");

    ResetArgs("-via=");
    BOOST_CHECK_EQUAL(GetArg("-via", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-via", "eleven"), "");

    ResetArgs("-via=11");
    BOOST_CHECK_EQUAL(GetArg("-via", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-via", "eleven"), "11");

    ResetArgs("-via=eleven");
    BOOST_CHECK_EQUAL(GetArg("-via", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-via", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-via", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-via", 0), 0);

    ResetArgs("-via -bar");
    BOOST_CHECK_EQUAL(GetArg("-via", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-via=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-via", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-via=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-via", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--via");
    BOOST_CHECK_EQUAL(GetBoolArg("-via"), true);

    ResetArgs("--via=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-via", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-novia");
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", true));
    BOOST_CHECK(!GetBoolArg("-via", false));

    ResetArgs("-novia=1");
    BOOST_CHECK(!GetBoolArg("-via"));
    BOOST_CHECK(!GetBoolArg("-via", true));
    BOOST_CHECK(!GetBoolArg("-via", false));

    ResetArgs("-novia=0");
    BOOST_CHECK(GetBoolArg("-via"));
    BOOST_CHECK(GetBoolArg("-via", true));
    BOOST_CHECK(GetBoolArg("-via", false));

    ResetArgs("-via --novia");
    BOOST_CHECK(GetBoolArg("-via"));

    ResetArgs("-novia -via"); // via always wins:
    BOOST_CHECK(GetBoolArg("-via"));
}

BOOST_AUTO_TEST_SUITE_END()
