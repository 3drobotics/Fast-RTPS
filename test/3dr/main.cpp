
#include <gtest/gtest.h>
#include "fastrtps/fastrtps_all.h"

int main(int argc, char *argv[])
{
    eprosima::Log::setVerbosity(eprosima::VERB_ERROR);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
