#include <common/path.h>
#include <gtest/gtest.h>

TEST(path_resolving_test, root_name) {
    const std::string example_path_sym = "C:\\symemu\\";
    ASSERT_EQ(eka2l1::root_name(example_path_sym, true), "C:");
}

TEST(path_resolving_test, absolute_checking) {
    const std::string example_path = "Z:\\resource\\";
    const std::string current_dir = "Z:\\sys\\bin\\";

    ASSERT_TRUE(eka2l1::is_absolute(example_path, current_dir, true));
}

TEST(path_resolving_test, absolute) {
    const std::string example_path = "despacito";
    const std::string current_dir = "Z:\\sys\\bin\\";
    const std::string expected = "Z:\\sys\\bin\\despacito";

    ASSERT_EQ(eka2l1::absolute_path(example_path, current_dir, true), expected);
}

TEST(path_resolving_test, add_path_mess) {
    const std::string example_path = "Z:\\sys/bin\\hi";
    const std::string example_path2 = "ha/ma";
    const std::string expected_result = "Z:\\sys\\bin\\hi\\ha\\ma";
    
    ASSERT_EQ(eka2l1::add_path(example_path, example_path2, true), expected_result);
}

TEST(path_resolving_test, filename) {
    const std::string example_path = "Z:\\sys\\bin\\euser.dll";
    ASSERT_EQ(strncmp(eka2l1::filename(example_path, true).data(), "euser.dll", 9), 0);
}

TEST(path_resolving_test, filedirectory) {
    const std::string example_path = "Z:\\sys\\bin\\euser.dll";
    ASSERT_EQ(strncmp(eka2l1::file_directory(example_path, true).data(), "Z:\\sys\\bin\\", 11), 0);
}

TEST(path_resolving_test, file_directory_directory_alone) {
    const std::string example_path = "Z:\\sys\\bin\\";
    ASSERT_EQ(strncmp(eka2l1::file_directory(example_path, true).data(), "Z:\\sys\\bin\\", 11), 0);
}

TEST(path_resolving_test, path_iterator) {
    const std::string path = "z:\\private\\10202bef\\";
    const std::string expected_components[3] = { "z:", "private", "10202bef" };

    eka2l1::path_iterator iterator(path);
    std::size_t i = 0;

    for (; iterator; i++, iterator++) {
        if (i >= 3) {
            ASSERT_EQ("", *iterator);
            break;
        }

        ASSERT_EQ(expected_components[i], *iterator);
    }

    ASSERT_EQ(i, 3);
}

TEST(path_resolving_test, path_iterator_file) {
    const std::string path = "Z:\\sys\\bin\\euser.dll";
    const std::string expected_components[4] = { "Z:", "sys", "bin", "euser.dll" };

    eka2l1::path_iterator iterator(path);
    std::size_t i = 0;

    for (; iterator; i++, iterator++) {
        if (i >= 4) {
            ASSERT_EQ("", *iterator);
            break;
        }

        ASSERT_EQ(expected_components[i], *iterator);
    }

    ASSERT_EQ(4, i);
}

TEST(path_resolving_test, extension) {
    std::string test_path = "hiyou.ne.py";
    std::string result_ext = eka2l1::path_extension(test_path);

    ASSERT_EQ(".py", result_ext);
}

TEST(path_resolving_test, replace_extension) {
    std::string test_path = "hiyou.ne.py";
    std::string expected = eka2l1::replace_extension(test_path, ".mom");

    ASSERT_EQ(expected, "hiyou.ne.mom");
}