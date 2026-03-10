#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, SliceTest)
{
    // str[1:5]
    // str[-1:]
    // str[5:1]
    // str[1:5]
    // str[-4:5]
    // str[1:-1]
    EXPECT_EQ(StringUtils::Slice("hello", 0, 0), std::string("hello"));
    EXPECT_EQ(StringUtils::Slice("hello", 0), std::string("hello"));
    EXPECT_EQ(StringUtils::Slice("hello", 1, 1), std::string(""));
    EXPECT_EQ(StringUtils::Slice("hello", 3, 0), std::string("lo"));
    EXPECT_EQ(StringUtils::Slice("hello", 7, 0), std::string(""));
    EXPECT_EQ(StringUtils::Slice("hello", 1, 5), std::string("ello"));
    EXPECT_EQ(StringUtils::Slice("hello", 0, 3), std::string("hel"));
    EXPECT_EQ(StringUtils::Slice("heLlo", 5, 1), std::string(""));
    EXPECT_EQ(StringUtils::Slice("hello", -300, 0), std::string("hello"));
    EXPECT_EQ(StringUtils::Slice("hello", -1, 0), std::string("o"));
    EXPECT_EQ(StringUtils::Slice("hello", -4, 5), std::string("ello"));
    EXPECT_EQ(StringUtils::Slice("hello", 1, -1), std::string("ell"));
    EXPECT_EQ(StringUtils::Slice("hello", 1, 3), std::string("el"));
    EXPECT_EQ(StringUtils::Slice("hello", 10, 0), std::string(""));
    EXPECT_EQ(StringUtils::Slice("hello", 10, 15), std::string(""));
    EXPECT_EQ(StringUtils::Slice("hello", 10, 150), std::string(""));
    EXPECT_EQ(StringUtils::Slice("x", -4, 3), std::string("x"));
    EXPECT_EQ(StringUtils::Slice("hello", 1, -100), std::string(""));
    EXPECT_EQ(StringUtils::Slice("hello", 0, 15), std::string("hello"));
    EXPECT_EQ(StringUtils::Slice("HELLO WORLD", 0, 100), std::string("HELLO WORLD"));
    EXPECT_EQ(StringUtils::Slice("", 0, 3), std::string(""));
}

TEST(StringUtilsTest, Capitalize)
{
    EXPECT_EQ(StringUtils::Capitalize("hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Capitalize(" hello world"), std::string(" hello world"));
    EXPECT_EQ(StringUtils::Capitalize("hello world"), std::string("Hello world"));
    EXPECT_EQ(StringUtils::Capitalize("HELLO WORLD"), std::string("Hello world"));
}

TEST(StringUtilsTest, Upper)
{
    EXPECT_EQ(StringUtils::Upper("HELLO WORLD"), std::string("HELLO WORLD"));
    EXPECT_EQ(StringUtils::Upper("HELLO world"), std::string("HELLO WORLD"));
    EXPECT_EQ(StringUtils::Upper("heLlo WOrLD"), std::string("HELLO WORLD"));
    EXPECT_EQ(StringUtils::Upper("hello world"), std::string("HELLO WORLD"));
    EXPECT_EQ(StringUtils::Upper(" hello world"), std::string(" HELLO WORLD"));
    EXPECT_EQ(StringUtils::Upper("\t hello world"), std::string("\t HELLO WORLD"));
}

TEST(StringUtilsTest, Lower)
{
    EXPECT_EQ(StringUtils::Lower("HELLO WORLD"), std::string("hello world"));
    EXPECT_EQ(StringUtils::Lower("HELLO world"), std::string("hello world"));
    EXPECT_EQ(StringUtils::Lower("heLlo WOrLD"), std::string("hello world"));
    EXPECT_EQ(StringUtils::Lower("HELLO WORLD"), std::string("hello world"));
    EXPECT_EQ(StringUtils::Lower(" hello world"), std::string(" hello world"));
    EXPECT_EQ(StringUtils::Lower("\t HELLO WORLD"), std::string("\t hello world"));
}

TEST(StringUtilsTest, LStrip)
{
    EXPECT_EQ(StringUtils::LStrip("Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::LStrip(" Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::LStrip("Hello "), std::string("Hello "));
    EXPECT_EQ(StringUtils::LStrip(" Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::LStrip("\r \t \nHello"), std::string("Hello"));
}

TEST(StringUtilsTest, RStrip)
{
    EXPECT_EQ(StringUtils::RStrip("Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("     Hello"), std::string("     Hello"));
    EXPECT_EQ(StringUtils::RStrip("Hello "), std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("Hello    "), std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("Hello \r \t \n"), std::string("Hello"));
    EXPECT_EQ(StringUtils::RStrip("     "), std::string(""));
    EXPECT_EQ(StringUtils::RStrip(""), std::string(""));
}

TEST(StringUtilsTest, Strip)
{
    EXPECT_EQ(StringUtils::Strip(" Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("Hello "), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("     Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("\r \t \nHello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("Hello"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("Hello    "), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("Hello \r \t \n"), std::string("Hello"));
    EXPECT_EQ(StringUtils::Strip("     "), std::string(""));
    EXPECT_EQ(StringUtils::Strip(""), std::string(""));
}

TEST(StringUtilsTest, Center)
{
    EXPECT_EQ(StringUtils::Center("Helo", 10, ' '), std::string("   Helo   "));
    EXPECT_EQ(StringUtils::Center("Hello", 10, ' '), std::string("  Hello   "));
    EXPECT_EQ(StringUtils::Center("Helo", 3, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::Center("Helo", 4, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::Center("Helo", 0, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::Center("Helo", 10, '-'), std::string("---Helo---"));
    EXPECT_EQ(StringUtils::Center("Helo", 9, '-'), std::string("--Helo---"));
    EXPECT_EQ(StringUtils::Center("", 10, '-'), std::string("----------"));
    EXPECT_EQ(StringUtils::Center("H", 3, '-'), std::string("-H-"));
    EXPECT_EQ(StringUtils::Center("", 0, '-'), std::string(""));
}

TEST(StringUtilsTest, LJust)
{
    EXPECT_EQ(StringUtils::LJust("Helo", 10, ' '), std::string("Helo      "));
    EXPECT_EQ(StringUtils::LJust("Hello", 10, ' '), std::string("Hello     "));
    EXPECT_EQ(StringUtils::LJust("Helo", 3, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::LJust("Helo", 4, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::LJust("Helo", 0, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::LJust("Helo", 10, '-'), std::string("Helo------"));
    EXPECT_EQ(StringUtils::LJust("Helo", 9, '-'), std::string("Helo-----"));
    EXPECT_EQ(StringUtils::LJust("", 10, '-'), std::string("----------"));
    EXPECT_EQ(StringUtils::LJust("H", 3, '-'), std::string("H--"));
}

TEST(StringUtilsTest, RJust)
{
    EXPECT_EQ(StringUtils::RJust("Helo", 10, ' '), std::string("      Helo"));
    EXPECT_EQ(StringUtils::RJust("Hello", 10, ' '), std::string("     Hello"));
    EXPECT_EQ(StringUtils::RJust("Helo", 3, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::RJust("Helo", 4, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::RJust("Helo", 0, ' '), std::string("Helo"));
    EXPECT_EQ(StringUtils::RJust("Helo", 10, '-'), std::string("------Helo"));
    EXPECT_EQ(StringUtils::RJust("Helo", 9, '-'), std::string("-----Helo"));
    EXPECT_EQ(StringUtils::RJust("", 10, '-'), std::string("----------"));
    EXPECT_EQ(StringUtils::RJust("H", 3, '-'), std::string("--H"));
}

TEST(StringUtilsTest, Replace)
{
    EXPECT_EQ(StringUtils::Replace("Hello world", "e", "a"), std::string("Hallo world"));
    EXPECT_EQ(StringUtils::Replace("Helle world", "e", "a"), std::string("Halla world"));
    EXPECT_EQ(StringUtils::Replace("Helle world", " ", "a"), std::string("Helleaworld"));
    EXPECT_EQ(StringUtils::Replace("Hello world", "", "a"), std::string("aHaealalaoa awaoaralada"));
    EXPECT_EQ(StringUtils::Replace("Helellelo world", "ell", "rep"), std::string("Helrepelo world"));
    EXPECT_EQ(StringUtils::Replace("Hellllo world", "l", "a"), std::string("Heaaaao worad"));
    EXPECT_EQ(StringUtils::Replace("Hellllo world", "ll", "aa"), std::string("Heaaaao world"));
    EXPECT_EQ(StringUtils::Replace("Hello world", "z", "a"), std::string("Hello world"));

    EXPECT_EQ(StringUtils::Replace("Hello welrld", "e", ""), std::string("Hllo wlrld"));
    EXPECT_EQ(StringUtils::Replace("Hello hello", "hello", "a"), std::string("Hello a"));
    EXPECT_EQ(StringUtils::Replace("hellohello", "hello", "bye"), std::string("byebye"));
    EXPECT_EQ(StringUtils::Replace("hellohello", "hello", ""), std::string(""));
    EXPECT_EQ(StringUtils::Replace("", "", "hello"), std::string("hello"));
    EXPECT_EQ(StringUtils::Replace("Hello", "Hello", "bye"), std::string("bye"));
    EXPECT_EQ(StringUtils::Replace("aaabaabaaaabaa", "aaa", "rep"), std::string("repbaabrepabaa"));
}

TEST(StringUtilsTest, Split)
{
    EXPECT_EQ(StringUtils::Split("hello world", " "), std::vector<std::string>({"hello", "world"}));
    EXPECT_EQ(StringUtils::Split("hello, world", ","), std::vector<std::string>({"hello", " world"}));
    EXPECT_EQ(StringUtils::Split("hello world", "l"), std::vector<std::string>({"he", "", "o wor", "d"}));
    EXPECT_EQ(StringUtils::Split("axxbxxcxx", "xx"), std::vector<std::string>({"a", "b", "c", ""}));

    EXPECT_EQ(StringUtils::Split("Helle world", "e"), std::vector<std::string>({"H", "ll", " world"}));
    EXPECT_EQ(StringUtils::Split(" Hello world", ""), std::vector<std::string>({"", "Hello", "world"}));
    EXPECT_EQ(StringUtils::Split("Helellelo world", "ell"), std::vector<std::string>({"Hel", "elo world"}));
    EXPECT_EQ(StringUtils::Split("Hellllo world", "l"), std::vector<std::string>({"He", "", "", "", "o wor", "d"}));
    EXPECT_EQ(StringUtils::Split("Hellllo world", "ll"), std::vector<std::string>({"He", "", "o world"}));
    EXPECT_EQ(StringUtils::Split("Hello world", "z"), std::vector<std::string>({"Hello world"}));

    EXPECT_EQ(StringUtils::Split("Hello hello", "h"), std::vector<std::string>({"Hello ", "ello"}));
    EXPECT_EQ(StringUtils::Split("hellobhello", "hello"), std::vector<std::string>({"", "b", ""}));
    EXPECT_EQ(StringUtils::Split("hellohello", "hello"), std::vector<std::string>({"", "", ""}));
    EXPECT_EQ(StringUtils::Split("", ""), std::vector<std::string>({""}));
    EXPECT_EQ(StringUtils::Split("", "lk"), std::vector<std::string>({""}));
    EXPECT_EQ(StringUtils::Split("aaabaabaaaabaa", "aaa"), std::vector<std::string>({"", "baab", "abaa"}));

    EXPECT_EQ(StringUtils::Split(",a,a,", ","), std::vector<std::string>({"", "a", "a", ""}));
    EXPECT_EQ(StringUtils::Split("aaa", ","), std::vector<std::string>({"aaa"}));
}

TEST(StringUtilsTest, Join)
{
    EXPECT_EQ(StringUtils::Join(" ", {"hello", "world"}), std::string("hello world"));
    EXPECT_EQ(StringUtils::Join(",", {"hello", " world"}), std::string("hello, world"));
    EXPECT_EQ(StringUtils::Join("l", {"he", "", "o wor", "d"}), std::string("hello world"));
    EXPECT_EQ(StringUtils::Join("xx", {"a", "b", "c", ""}), std::string("axxbxxcxx"));

    EXPECT_EQ(StringUtils::Join("e", {"H", "ll", " world"}), std::string("Helle world"));
    EXPECT_EQ(StringUtils::Join("", {"", "Hello", "world"}), std::string("Helloworld"));
    EXPECT_EQ(StringUtils::Join("", {"Hello", "world"}), std::string("Helloworld"));
    EXPECT_EQ(StringUtils::Join("ell", {"Hel", "elo world"}), std::string("Helellelo world"));
    EXPECT_EQ(StringUtils::Join("l", {"He", "", "", "", "o wor", "d"}), std::string("Hellllo world"));
    EXPECT_EQ(StringUtils::Join("ll", {"He", "", "o world"}), std::string("Hellllo world"));
    EXPECT_EQ(StringUtils::Join("z", {"Hello world"}), std::string("Hello world"));

    EXPECT_EQ(StringUtils::Join("h", {"Hello ", "ello"}), std::string("Hello hello"));
    EXPECT_EQ(StringUtils::Join("hello", {"", "b", ""}), std::string("hellobhello"));
    EXPECT_EQ(StringUtils::Join("hello", {"", "", ""}), std::string("hellohello"));
    EXPECT_EQ(StringUtils::Join("", {""}), std::string(""));
    EXPECT_EQ(StringUtils::Join("", {}), std::string(""));
    EXPECT_EQ(StringUtils::Join("kjk", {""}), std::string(""));
    EXPECT_EQ(StringUtils::Join("aaa", {"", "baab", "abaa"}), std::string("aaabaabaaaabaa"));

    EXPECT_EQ(StringUtils::Join(",", {"", "a", "a", ""}), std::string(",a,a,"));
    EXPECT_EQ(StringUtils::Join(",", {"aaa"}), std::string("aaa"));
    EXPECT_EQ(StringUtils::Join("\n", {"a", "a", "a"}), std::string("a\na\na"));
}

TEST(StringUtilsTest, ExpandTabs)
{
    EXPECT_EQ(StringUtils::ExpandTabs("hel\tlo world"), std::string("hel lo world"));
    EXPECT_EQ(StringUtils::ExpandTabs("hell\to world"), std::string("hell    o world"));
    EXPECT_EQ(StringUtils::ExpandTabs("hel\tlo \tw\torld"), std::string("hel lo  w   orld"));
    EXPECT_EQ(StringUtils::ExpandTabs("hel\tl\no \tworld"), std::string("hel l\no   world"));
    EXPECT_EQ(StringUtils::ExpandTabs("hello \tworld"), std::string("hello   world"));
    EXPECT_EQ(StringUtils::ExpandTabs("hello world"), std::string("hello world"));
    EXPECT_EQ(StringUtils::ExpandTabs("h\te"), std::string("h   e"));
    EXPECT_EQ(StringUtils::ExpandTabs("h\te\tl"), std::string("h   e   l"));
    EXPECT_EQ(StringUtils::ExpandTabs("he\tllo\t\t w\n\tor\tld", 1), std::string("he llo   w\n or ld"));
    EXPECT_EQ(StringUtils::ExpandTabs("he\tllo\t\t w\n\tor\tld", 2), std::string("he  llo    w\n  or  ld"));
    EXPECT_EQ(StringUtils::ExpandTabs("hello w\t\tord", 3), std::string("hello w     ord"));
    EXPECT_EQ(StringUtils::ExpandTabs("hello w\n\tord"), std::string("hello w\n    ord"));
    EXPECT_EQ(StringUtils::ExpandTabs(""), std::string(""));
    EXPECT_EQ(StringUtils::ExpandTabs("\t"), std::string("    "));
    EXPECT_EQ(StringUtils::ExpandTabs("hello w\tor\nld\tm"), std::string("hello w or\nld  m"));
    EXPECT_EQ(StringUtils::ExpandTabs("hell\to world", 5), std::string("hell o world"));
    EXPECT_EQ(StringUtils::ExpandTabs("hello w\tor\nld\tm", 5), std::string("hello w   or\nld   m"));
    EXPECT_EQ(StringUtils::ExpandTabs("hel\tl\no \tword", 3), std::string("hel   l\no  word"));
}

TEST(StringUtilsTest, EditDistance)
{
    EXPECT_EQ(StringUtils::EditDistance("kitten", "sitting"), int(3));
    EXPECT_EQ(StringUtils::EditDistance("kitten", "kittei"), int(1));
    EXPECT_EQ(StringUtils::EditDistance("kiten", "kitten"), int(1));
    EXPECT_EQ(StringUtils::EditDistance("", ""), int(0));
    EXPECT_EQ(StringUtils::EditDistance("kitten", "kitten"), int(0));
    EXPECT_EQ(StringUtils::EditDistance("kittieng", "kitten"), int(2));
    EXPECT_EQ(StringUtils::EditDistance("", "kitten"), int(6));
    EXPECT_EQ(StringUtils::EditDistance("kittennnn", "kitten"), int(3));
    EXPECT_EQ(StringUtils::EditDistance("kiTten", "kitten", true), int(0));
    EXPECT_EQ(StringUtils::EditDistance("kiTten", "kitten"), int(1));
}
