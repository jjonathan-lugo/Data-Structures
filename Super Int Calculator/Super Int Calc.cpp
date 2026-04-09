#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace std;

struct super_int
{
    int* digits;
    int size;
    bool is_negative;

    super_int(const char* s)
    {
        is_negative = false;
        int start_idx = 0;
        if (s[0] == '-')
        {
            is_negative = true;
            start_idx = 1;
        }

        while (s[start_idx] == '0' && s[start_idx + 1] != '\0')
            start_idx++;

        size = 0;
        while (s[start_idx + size] != '\0')
            size++;

        digits = new int[size];
        for (int i = 0; i < size; i++)
            digits[i] = s[start_idx + i] - '0';

        remove_leading_zeros();
    }

    ~super_int()
    {
        delete[] digits;
    }

    super_int(const super_int& other)
    {
        size = other.size;
        is_negative = other.is_negative;
        digits = new int[size];
        for (int i = 0; i < size; i++)
            digits[i] = other.digits[i];
    }

    super_int& operator=(const super_int& other)
    {
        if (this != &other)
        {
            delete[] digits;
            size = other.size;
            is_negative = other.is_negative;
            digits = new int[size];
            for (int i = 0; i < size; i++)
                digits[i] = other.digits[i];
        }
        return *this;
    }

    void resize(int new_size)
    {
        int* new_digits = new int[new_size];
        int min_size = (new_size < size) ? new_size : size;
        for (int i = 0; i < min_size; i++)
            new_digits[new_size - 1 - i] = digits[size - 1 - i];
        for (int i = 0; i < new_size - min_size; i++)
            new_digits[i] = 0;
        delete[] digits;
        digits = new_digits;
        size = new_size;
    }

    void remove_leading_zeros()
    {
        int first_nonzero = 0;
        while (first_nonzero < size - 1 && digits[first_nonzero] == 0)
            first_nonzero++;

        if (first_nonzero > 0)
        {
            int new_size = size - first_nonzero;
            int* new_digits = new int[new_size];
            for (int i = 0; i < new_size; i++)
                new_digits[i] = digits[first_nonzero + i];
            delete[] digits;
            digits = new_digits;
            size = new_size;
        }

        if (size == 1 && digits[0] == 0)
            is_negative = false;
    }

    bool operator<=(const super_int& other) const
    {
        if (is_negative != other.is_negative)
            return is_negative;
        if (size != other.size)
        {
            if (is_negative)
                return size > other.size;
            return size < other.size;
        }
        for (int i = 0; i < size; i++)
        {
            if (digits[i] != other.digits[i])
            {
                if (is_negative)
                    return digits[i] > other.digits[i];
                return digits[i] < other.digits[i];
            }
        }
        return true;
    }

    bool operator>=(const super_int& other) const
    {
        return !(*this <= other);
    }

    super_int& operator-=(const super_int& other)
    {
        if (is_negative != other.is_negative)
        {
            super_int temp = other;
            temp.is_negative = !other.is_negative;
            *this += temp;
            return *this;
        }

        bool negate_result = false;
        super_int larger = *this, smaller = other;
        if (*this <= other)
        {
            swap(larger, smaller);
            negate_result = true;
        }

        int borrow = 0;
        int i = larger.size;
        int j = smaller.size;
        super_int result("0");
        result.resize(larger.size);

        int pos = larger.size;
        while (i > 0 || j > 0)
        {
            int larger_digit = (i > 0) ? larger.digits[--i] : 0;
            int smaller_digit = (j > 0) ? smaller.digits[--j] : 0;
            int sub = larger_digit - smaller_digit - borrow;

            if (sub < 0)
            {
                sub += 10;
                borrow = 1;
            }
            else
                borrow = 0;

            result.digits[--pos] = sub;
        }

        *this = result;
        remove_leading_zeros();
        is_negative = negate_result;
        return *this;
    }

    super_int& operator+=(const super_int& other)
    {
        int carry = 0;
        int i = size;
        int j = other.size;
        int max_size = (size > other.size ? size : other.size) + 1;

        super_int result("0");
        result.resize(max_size);

        int pos = max_size;
        while (i > 0 || j > 0 || carry)
        {
            int sum = carry;
            if (i > 0)
                sum += digits[--i];
            if (j > 0)
                sum += other.digits[--j];

            carry = sum / 10;
            result.digits[--pos] = sum % 10;
        }

        *this = result;
        remove_leading_zeros();
        return *this;
    }

    super_int& operator*=(const super_int& other)
    {
        bool result_negative = (is_negative != other.is_negative);
        int new_size = size + other.size;
        super_int result("0");
        result.resize(new_size);

        for (int i = size - 1; i >= 0; i--)
        {
            int carry = 0;
            for (int j = other.size - 1; j >= 0; j--)
            {
                int product = digits[i] * other.digits[j]
                    + result.digits[i + j + 1] + carry;
                carry = product / 10;
                result.digits[i + j + 1] = product % 10;
            }
            result.digits[i] += carry;
        }

        *this = result;
        remove_leading_zeros();
        is_negative = result_negative && !(size == 1 && digits[0] == 0);
        return *this;
    }

    void factorial()
    {
        if (is_negative)
        {
            cout << "factorial of negative number is undefined!" << '\n';
            return;
        }

        super_int result("1");
        super_int one("1");
        super_int counter("2");

        while (counter <= *this)
        {
            result *= counter;
            counter += one;
        }
        *this = result;
    }

    void print() const
    {
        if (is_negative)
            cout << "-";
        for (int i = 0; i < size; i++)
            cout << digits[i];
    }
};

int main()
{
    super_int a("123456789"), b("1000000000"), c("987654321");
    a *= b;
    a += c;
    a.print(); cout << "\n\n";

    super_int d("1234567890987654321"), e("98765432123456789");
    d *= e;
    d.print(); cout << "\n\n";

    super_int f("121932631239140373304069513112635269");
    f *= f;
    f.print(); cout << "\n\n";
    f *= f;
    f.print(); cout << "\n\n";
    f *= f;
    f.print(); cout << "\n\n";

    super_int g("1000");
    g.factorial();
    g.print(); cout << "\n\n";

    super_int h("25");
    super_int i("13573657");
    h -= i;
    h.print(); cout << "\n\n";

    super_int j("32348783425");
    super_int k("10938449");
    j += k;
    j.print(); cout << "\n\n";

    super_int l("123456789012345678901234567890");
    super_int m("987654321098765432109876543210");
    l *= m;
    l.print(); cout << "\n\n";

    super_int n("9999999999999999999999");
    super_int o("8888888888888888888888");
    super_int p("7777777777777777777777");
    n *= o;
    n += p;
    n.print(); cout << "\n\n";

    return 0;
}
