#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../OptionParser.hpp"


TEST_CASE("Instantiate only", "[test]") {
    // Setup test values...
    int argc = 1;
    char *argv[] = {
        (char *)"appName.exe"
    };

    // Create instance...
    OptionParser* parser = new OptionParser(argc, argv);

    REQUIRE( parser != nullptr );
}

TEST_CASE("Command only", "[test]") {
    // Setup test values...
    int argc = 1;
    char *argv[] = {
        (char *)"appName.exe"
    };

    // Create instance...
    OptionParser parser(argc, argv);

    // Check if HasOptions() working properly...
    REQUIRE( parser.HasOptions() == false );

    // Check if HasParams() working properly...
    REQUIRE( parser.HasParams() == false );
}

TEST_CASE("Mixed operation", "[test]") {
    // Setup test values...
    int argc = 18;
    char *argv[] = {
        (char *)"appName.exe",
        (char *)"--test",
        (char *)"--verbose",
        (char *)"--mode=3",
        (char *)"3.14159",  // param 0
        // Following term should appear as "Some message" on command line.
        (char *)"Some message", // param 1
        (char *)"--quit",
        (char *)"-1.0",  // param 2
        (char *)"+1.0",  // param 3
        (char *)"--123",  // param 4
        (char *)"word",  // param 5
        (char *)"--area=14.285",
        (char *)"--label=NONE",
        // Following term should appear as --desc="Two Word" on command line.
        (char *)"--desc=Two Word",
        (char *)"---321", //param 6
        (char *)"----1234", //param 7
        (char *)"-abc",
        (char *)"---abc"
    };

    // Create instance...
    OptionParser parser(argc, argv);

    // Check if HasOptions() working properly...
    REQUIRE( parser.HasOptions() == true );

    // Check if HasParams() working properly...
    REQUIRE( parser.HasParams() == true );

    // Check if HasFlag() working properly...
    REQUIRE( parser.HasFlag("--test") == true );
    REQUIRE( parser.HasFlag("--verbose") == true );
    REQUIRE( parser.HasFlag("--quit") == true );
    REQUIRE( parser.HasFlag("--area") == false);
    REQUIRE( parser.HasFlag("-abc") == false);
    REQUIRE( parser.HasFlag("---abc") == false);
    REQUIRE( parser.HasFlag("--123") == false);
    REQUIRE( parser.HasFlag("----1234") == false);
    REQUIRE( parser.HasFlag("-1.0") == false);
    REQUIRE( parser.HasFlag("Some message") == false);
    
    // Check if HasValue() working properly...
    // .. Check name
    REQUIRE( parser.HasValue("--mode") == true );
    // .. Get (integer) value
    REQUIRE( parser.GetInteger("--mode") == 3 );
    // .. Check name
    REQUIRE( parser.HasValue("--area") == true );
    // .. Get (double) value
    REQUIRE( parser.GetDouble("--area") == Approx( 14.285 ) );
    // .. Check name
    REQUIRE( parser.HasValue("--label") == true );
    // .. Get (string) value
    REQUIRE( parser.GetString("--label") == "NONE" );
    // .. Check name
    REQUIRE( parser.HasValue("--desc") == true );
    // .. Get (string) value
    REQUIRE( parser.GetString("--desc") == "Two Word" );

    // Check if positional parameters can be read
    // .. Get first parameter
    REQUIRE( parser.GetParam(0) == "3.14159" );
    // .. Get second parameter
    REQUIRE( parser.GetParam(1) == "Some message" );
    // .. Get third parameter
    REQUIRE( parser.GetParam(2) == "-1.0" );
    // .. Get fourth parameter
    REQUIRE( parser.GetParam(3) == "+1.0" );
    // .. Get fifth parameter
    CHECK( parser.GetParam(4) == "--123" );
    // .. Get sixth parameter
    CHECK( parser.GetParam(5) == "word" );
    // .. Get seventh parameter
    CHECK( parser.GetParam(6) == "---321" );
    // .. Get eight parameter
    CHECK( parser.GetParam(7) == "----1234" );



    // Positional variables should not work as flags..
    REQUIRE( parser.HasFlag("-1.0") == false );
    REQUIRE( parser.HasFlag("3.14159") == false );
    REQUIRE( parser.HasFlag("word") == false );
    REQUIRE( parser.HasFlag("---321") == false);
     REQUIRE( parser.HasFlag("----1234") == false);

    // If given index is greater than the number of positional parameters, throw.
    REQUIRE_THROWS_AS( parser.GetParam(argc + 2), std::string );

    // Flag options should not be used as value options.
    CHECK( parser.HasValue("--quit") == false );
    CHECK( parser.HasValue("---321") == false );

    // Values should not be queried for flag options.
    CHECK_THROWS_AS( parser.GetInteger("--quit"), std::string );
    CHECK_THROWS_AS( parser.GetDouble("--verbose"), std::string );
    CHECK_THROWS_AS( parser.GetString("--test"), std::string );

    // Flag options cannot be numeric
    CHECK( parser.HasFlag("--123") == false );
}