#include "StringUtils.h"
#include <cctype>

namespace StringUtils
{
    // https://www.w3schools.com/python/gloss_python_string_slice.asp
    // used to test python slice behavior
    std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept
    {

        // Replace code here
        std::string Temp = str;
        ssize_t temp_start = start;
        ssize_t temp_end = end;

        // str[1:5]
        // str[-1:]
        // str[5:1]

        ssize_t Length = Temp.length();

        if (end == 0)
        {
            temp_end = Length;
        }

        if (temp_start < 0 - Length)
        {
            temp_start = 0;
        }
        else if (temp_start < 0)
        {
            temp_start = Length + temp_start;
        }
        if (temp_end < 0 - Length)
        {
            temp_end = 0;
        }
        else if (temp_end < 0)
        {
            temp_end = Length + temp_end;
        }

        if (temp_start >= temp_end || temp_start >= Length)
        { // if start is after end: str[5:1]
            return "";
        }

        temp_end = temp_end - temp_start;
        if (temp_end > Length - temp_start)
        {
            temp_end = Length - temp_start;
        }

        if (temp_start >= 0)
        { // str[1:5] end can be > len, start can be < -len, becomes 0
            return Temp.substr(temp_start, temp_end);
        }

        return "";
    }

    std::string Capitalize(const std::string &str) noexcept
    {
        // Replace code here
        std::string Temp = str;
        if (!Temp.empty())
        {
            Temp[0] = toupper(Temp[0]);
            for (size_t Index = 1; Index < Temp.length(); Index++)
            {
                Temp[Index] = tolower(Temp[Index]);
            }
        }

        return Temp;
    }

    std::string Upper(const std::string &str) noexcept
    {
        std::string Temp = str;

        if (!Temp.empty())
        {
            for (size_t i = 0; i < Temp.length(); i++)
            {
                Temp[i] = toupper(Temp[i]);
            }
        }

        return Temp;
    }

    std::string Lower(const std::string &str) noexcept
    {
        std::string Temp = str;

        if (!Temp.empty())
        {
            for (size_t i = 0; i < Temp.length(); i++)
            {
                Temp[i] = tolower(Temp[i]);
            }
        }

        return Temp;
    }

    std::string LStrip(const std::string &str) noexcept
    {
        std::string Temp = str;
        size_t Index = 0;
        size_t Length = Temp.length();
        while (Index < Length && isspace(Temp[Index]))
        {
            Index++;
        }

        return Temp.substr(Index);
    }

    std::string RStrip(const std::string &str) noexcept
    {
        std::string Temp = str;
        if (!Temp.empty())
        {
            size_t Index = Temp.length() - 1;
            while (isspace(Temp[Index]))
            {
                if (!Index)
                {
                    return "";
                }
                Index--;
            }
            return Temp.substr(0, Index + 1);
        }

        return "";
    }

    std::string Strip(const std::string &str) noexcept
    {
        return LStrip(RStrip(str));
    }

    // https://www.geeksforgeeks.org/python/string-alignment-in-python-f-string/
    // alignment behavior
    std::string Center(const std::string &str, int width, char fill) noexcept
    {
        // Replace code here
        std::string Temp;
        std::string add;
        size_t len = str.length();

        if (width <= len)
        {
            return str;
        }

        size_t space = (width - len) / 2; // automatically truncates

        for (size_t i = 0; i < space; i++)
        {
            add += fill;
        }
        Temp = add;
        Temp += str;
        Temp += add;

        if ((width - len) % 2 != 0)
            Temp += fill;

        return Temp;
    }

    std::string LJust(const std::string &str, int width, char fill) noexcept
    {
        // Replace code here
        std::string Temp = str;
        size_t len = str.length();

        if (width <= len)
        {
            return str;
        }

        for (size_t i = 0; i < width - len; i++)
        {
            Temp += fill;
        }

        return Temp;
    }

    std::string RJust(const std::string &str, int width, char fill) noexcept
    {
        // Replace code here
        std::string Temp;
        size_t len = str.length();

        if (width <= len)
        {
            return str;
        }

        for (size_t i = 0; i < width - len; i++)
        {
            Temp += fill;
        }
        Temp += str;

        return Temp;
    }

    std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept
    {
        // Replace code here
        std::string Temp;
        size_t old_len = old.length();
        size_t len = str.length();

        if (old == "")
        {
            Temp += rep;
            for (size_t i = 0; i < len; i++)
            {
                Temp += str[i];
                Temp += rep;
            }
        }
        else
        {
            for (size_t i = 0; i < len; i++)
            {
                if (i < len - old_len + 1)
                {
                    if (str.substr(i, old_len) == old)
                    {
                        Temp += rep;
                        i += old_len - 1;
                    }
                    else
                    {
                        Temp += str[i];
                    }
                }
                else
                {
                    Temp += str[i];
                }
            }
        }

        return Temp;
    }

    std::vector<std::string> Split(const std::string &str, const std::string &splt) noexcept
    {
        // Replace code here
        // https://www.geeksforgeeks.org/cpp/vector-insert-function-in-cpp-stl/
        // how to use vector c++

        std::string split = splt;

        size_t len = str.length();
        std::vector<std::string> Temp;
        std::string s;

        if (splt == "")
        {
            split = " ";
        }
        size_t splt_len = split.length();

        for (size_t i = 0; i < len; i++)
        {
            if (i < len - splt_len + 1)
            {
                if (str.substr(i, splt_len) == split)
                {
                    Temp.push_back(s);
                    s = "";
                    i += splt_len - 1;
                }
                else
                {
                    s += str[i];
                }
            }
            else
            {
                s += str[i];
            }
        }

        Temp.push_back(s);
        return Temp;
    }

    std::string Join(const std::string &str, const std::vector<std::string> &vect) noexcept
    {
        // Replace code here
        // std::string join = str;

        size_t len = vect.size();
        std::string Temp;
        std::string s;

        // size_t splt_len = split.length();

        if (len <= 0)
        {
            return "";
        }

        for (size_t i = 0; i < len; i++)
        {
            if (i >= 1)
            {
                Temp += str;
            }
            Temp += vect.at(i);
        }

        return Temp;
    }

    std::string ExpandTabs(const std::string &str, int tabsize) noexcept
    {
        // Replace code here
        std::string Temp;
        if (str == "")
        {
            return "";
        }

        size_t pos = 0;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (str[i] == '\t')
            {
                // if (pos == 0)
                for (size_t j = pos; j < tabsize; j++)
                {
                    Temp += " ";
                }

                pos = 0;
            }
            else if (str[i] == '\n')
            {
                pos = 0;
                Temp += '\n';
            }
            else
            {
                Temp += str[i];
                pos++;
                if (pos > tabsize - 1)
                {
                    pos = 0;
                }
            }
        }
        return Temp;
    }

    int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept
    {
        // Replace code here
        if (left == "")
        {
            if (right == "")
            {
                return 0;
            }

            return right.length();
        }
        else if (right == "")
        {
            return left.length();
        }

        std::string templ = left;
        std::string tempr = right;
        if (ignorecase)
        {
            templ = Lower(left);
            tempr = Lower(right);
        }

        // lev (tail(a), tail(b))
        std::string taill;
        std::string tailr;

        if (templ.length() == 1)
        {
            taill = "";
        }
        else
        {
            taill = templ.substr(1, templ.length() - 1);
        }

        if (tempr.length() == 1)
        {
            tailr = "";
        }
        else
        {
            tailr = tempr.substr(1, tempr.length() - 1);
        }

        std::string headl = templ.substr(0, 1);
        std::string headr = tempr.substr(0, 1);

        if (headl == headr)
        {
            return EditDistance(taill, tailr);
        }

        ssize_t del = EditDistance(taill, right);
        ssize_t insert = EditDistance(left, tailr);
        ssize_t repl = EditDistance(taill, tailr);

        return 1 + std::min(del, std::min(insert, repl));
    }
};