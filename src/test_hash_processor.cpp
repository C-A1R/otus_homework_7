#define BOOST_TEST_MODULE test_hash_processor

#include "hash_processor.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_hash_processor)

BOOST_AUTO_TEST_CASE(test_empty_encrypt)
{
    EmptyProcessor p;
    BOOST_CHECK(p.encrypt("") == "");
    BOOST_CHECK(p.encrypt("123") == "123");
    BOOST_CHECK(p.encrypt("Hello World!") == "Hello World!");
}

BOOST_AUTO_TEST_CASE(test_md5_encrypt)
{
    MD5Processor p;
    BOOST_CHECK(p.encrypt("") == "D98C1DD404B2008F980980E97E42F8EC");
    BOOST_CHECK(p.encrypt("123") == "62B92C205B0759AC15074B96704B232D");
    BOOST_CHECK(p.encrypt("Hello World!") == "876207ED36862E53921E845E8C0DC5BF");
}

BOOST_AUTO_TEST_CASE(test_sha1_encrypt)
{
    SHA1Processor p;
    BOOST_CHECK(p.encrypt("") == "EEA339DA0D4B6B5EEFBF5532901860950907D8AF");
    BOOST_CHECK(p.encrypt("123") == "1500BD40C35F08639E3265515E5CFFA1EFBEDBCB");
    BOOST_CHECK(p.encrypt("Hello World!") == "E6BDF72E0454CE08045F7DE99FF8952F7128231C");
}

BOOST_AUTO_TEST_SUITE_END() 