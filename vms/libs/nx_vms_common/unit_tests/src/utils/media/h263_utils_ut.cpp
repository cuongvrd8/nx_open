// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include <gtest/gtest.h>

#include <utils/media/h263_utils.h>

using namespace nx::media::h263;

TEST(H263, PictureHeaderTest)
{
    PictureHeader header;
    uint8_t dataP[] = {
        0x00, 0x00, 0x82, 0xfa, 0x1c, 0xe8, 0x01, 0x04, 0x90, 0xa7, 0xe5, 0xaf,
        0xf0, 0x4e, 0x93, 0x69, 0x60, 0x57, 0x0d, 0x41, 0xb4, 0xff, 0x32, 0x7d };

    uint8_t dataI[] = {
        0x00, 0x00, 0x80, 0x9e, 0x1c, 0xe8, 0x01, 0x00, 0x10, 0xa7, 0xe5, 0xaf,
        0xfc, 0x69, 0xb1, 0xb1, 0xb1, 0xb1, 0xc0, 0xbf, 0x4d, 0x8d, 0x8d, 0x8d};

    ASSERT_TRUE(header.decode(dataP, sizeof(dataP)));
    ASSERT_FALSE(nx::media::h263::isKeyFrame(header.pictureType));
    ASSERT_TRUE(header.decode(dataI, sizeof(dataI)));
    ASSERT_TRUE(nx::media::h263::isKeyFrame(header.pictureType));
    ASSERT_EQ(header.width, 640);
    ASSERT_EQ(header.height, 360);
}