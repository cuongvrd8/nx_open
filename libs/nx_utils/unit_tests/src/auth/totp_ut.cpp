// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include <gtest/gtest.h>

#include <nx/utils/auth/totp.h>
#include <nx/utils/auth/utils.h>
#include <nx/utils/time.h>

namespace nx::utils::test {

class TotpGenerator: public ::testing::Test
{
public:
    nx::utils::auth::TotpGenerator m_generator =
        nx::utils::auth::TotpGenerator(std::chrono::seconds(30), 6);
    const std::string m_key = "12345678901234567890";
    nx::utils::test::ScopedTimeShift m_timeShift =
        nx::utils::test::ScopedTimeShift(test::ClockType::system);

    void whenTimeShiftedToGivenMoment()
    {
        // A moment in past, for which (and given key) totp was generated by goggle authenticator.
        static const auto kFixedMoment = std::chrono::milliseconds(1612889916015);
        m_timeShift.applyRelativeShift(
            kFixedMoment
            - std::chrono::duration_cast<std::chrono::milliseconds>(
                nx::utils::utcTime().time_since_epoch()));
    }

    void whenTimeShiftedThirtySecondsBefore()
    {
        m_timeShift.applyRelativeShift(-std::chrono::seconds(30));
    }

    void whenTimeShiftedThirtySecondsAfter()
    {
        m_timeShift.applyRelativeShift(std::chrono::seconds(30));
    }
};

TEST_F(TotpGenerator, generateHotpKey1)
{
    // Examples from RFC 4226.
    ASSERT_EQ(m_generator.getHotp(m_key, 0), 755224);
    ASSERT_EQ(m_generator.getHotp(m_key, 1), 287082);
    ASSERT_EQ(m_generator.getHotp(m_key, 2), 359152);
}

TEST_F(TotpGenerator, generateHotpKey2)
{
    // Another example.
    const std::string key = "12345670123456701234";
    ASSERT_EQ(m_generator.getHotp(key, 0), 966976);
    ASSERT_EQ(m_generator.getHotp(key, 1), 136556);
    ASSERT_EQ(m_generator.getHotp(key, 2), 696757);
}

TEST_F(TotpGenerator, validateHotp)
{
    ASSERT_TRUE(m_generator.validateHotp(m_key, 7, "162583"));
}

TEST_F(TotpGenerator, generateTotp)
{
    whenTimeShiftedToGivenMoment();
    ASSERT_EQ(m_generator.getTotp(m_key), "245776");
}

TEST_F(TotpGenerator, validateTotp)
{
    whenTimeShiftedToGivenMoment();
    ASSERT_TRUE(m_generator.validateTotp(m_key, "245776", 1));
}

TEST_F(TotpGenerator, acceptPriorTotp)
{
    whenTimeShiftedToGivenMoment();
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "245776"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "076804"));
    ASSERT_FALSE(m_generator.validateTotpRelaxed(m_key, "742269"));
    whenTimeShiftedThirtySecondsBefore();
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "245776"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "076804"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "742269"));
    whenTimeShiftedThirtySecondsBefore();
    ASSERT_FALSE(m_generator.validateTotpRelaxed(m_key, "245776"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "076804"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "742269"));
}

TEST_F(TotpGenerator, acceptFutureTotp)
{
    whenTimeShiftedToGivenMoment();
    ASSERT_FALSE(m_generator.validateTotpRelaxed(m_key, "150624"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "133927"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "245776"));
    whenTimeShiftedThirtySecondsAfter();
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "150624"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "133927"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "245776"));
    whenTimeShiftedThirtySecondsAfter();
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "150624"));
    ASSERT_TRUE(m_generator.validateTotpRelaxed(m_key, "133927"));
    ASSERT_FALSE(m_generator.validateTotpRelaxed(m_key, "245776"));
}

TEST_F(TotpGenerator, base32Encode)
{
    using namespace nx::utils::auth;
    // Some pre-calculated examples.
    ASSERT_EQ(encodeToBase32("1234567012345670"), "GEZDGNBVGY3TAMJSGM2DKNRXGA======");
    ASSERT_EQ(encodeToBase32("AdsfkGJE34321KIO01ZP"), "IFSHGZTLI5FEKMZUGMZDCS2JJ4YDCWSQ");
    ASSERT_EQ(encodeToBase32("XY"), "LBMQ====");
    ASSERT_EQ(encodeToBase32("ferTryqP"), "MZSXEVDSPFYVA===");
    ASSERT_EQ(encodeToBase32("2398405KO"), "GIZTSOBUGA2UWTY=");
}

TEST_F(TotpGenerator, base32Decode)
{
    using namespace nx::utils::auth;
    // Some pre-calculated examples.
    std::vector<std::pair<std::string, std::string>> base32Utf8Pairs = {
        {"GEZDGNBVGY3TAMJSGM2DKNRXGA======", "1234567012345670"},
        {"IFSHGZTLI5FEKMZUGMZDCS2JJ4YDCWSQ", "AdsfkGJE34321KIO01ZP"},
        {"LBMQ====", "XY"},
        {"MZSXEVDSPFYVA===", "ferTryqP"},
        {"GIZTSOBUGA2UWTY=", "2398405KO"},
        {"", ""}
    };

    for (const auto& i: base32Utf8Pairs)
    {
        const auto [success, result] = decodeBase32(i.first);
        ASSERT_TRUE(success);
        ASSERT_EQ(result, i.second);
    }
}

TEST_F(TotpGenerator, base32DecodeIncorrectStr)
{
    // 1 is prohibited, base32 str can't end with exactly 2 '=', length must be a multiple of 8.
    std::vector<std::string> incorrectStrs = {"12345678", "AAAAAA==", "AA"};

    for (const auto& str: incorrectStrs)
    {
        const auto [success, result] = nx::utils::auth::decodeBase32(str);
        ASSERT_FALSE(success);
    }
}

} // namespace nx::utils::test