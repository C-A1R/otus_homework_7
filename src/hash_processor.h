#ifndef HASH_PROCESSOR_H
#define HASH_PROCESSOR_H

#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/algorithm/hex.hpp>

class IHashProcessor
{
public:
    IHashProcessor() = default;
    virtual ~IHashProcessor() = default;
    virtual std::string encrypt(const std::string &s) = 0;
};

class EmptyProcessor : public IHashProcessor
{
public:
    EmptyProcessor() = default;
    std::string encrypt(const std::string &s) override
    {
        return s;
    }
};

class MD5Processor : public IHashProcessor
{
public:
    MD5Processor() = default;
    std::string encrypt(const std::string &s) override
    {
        using boost::uuids::detail::md5;
        md5 hash;
        hash.process_bytes(s.data(), s.size());
        md5::digest_type digest;
        hash.get_digest(digest);

        const auto charDigest = reinterpret_cast<const char *>(&digest);
        std::string result;
        boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(result));
        return result;
    }
};

class SHA1Processor : public IHashProcessor
{
public:
    SHA1Processor() = default;
    std::string encrypt(const std::string &s) override
    {
        using boost::uuids::detail::sha1;
        sha1 hash;
        hash.process_bytes(s.data(), s.size());
        sha1::digest_type digest;
        hash.get_digest(digest);

        const auto charDigest = reinterpret_cast<const char *>(&digest);
        std::string result;
        boost::algorithm::hex(charDigest, charDigest + sizeof(sha1::digest_type), std::back_inserter(result));
        return result;
    }
};

#endif //HASH_PROCESSOR_H