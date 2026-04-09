#include <iostream>
#include <cassert>
using namespace std;

class super_int 
{
private:
    struct Node 
    {
        int digit;
        Node* next;
        Node(int d = 0, Node* n = nullptr) : digit(d), next(n) {}
    };

    Node* head;     
    int size;     
    bool negative;

public:
    super_int() : head(new Node(0)), size(1), negative(false) {}

    super_int(int num) : head(nullptr), size(0), negative(num < 0) 
    {
        if (num == 0) {
            head = new Node(0);
            size = 1;
            negative = false;
        }
        else {
            if (num < 0) num = -num;
            while (num > 0) {
                push_back(num % 10);
                num /= 10;
            }
        }
    }

    ~super_int() 
    {
        clear();
    }

    super_int(const super_int& other) 
    {
        copy_from(other);
    }

    super_int& operator=(const super_int& other) 
    {
        if (this != &other) 
        {
            clear();
            copy_from(other);
        }
        return *this;
    }

    void print() const 
    {
        if (negative && !is_zero()) cout << '-';
        print_reverse(head);
    }

    super_int operator+(const super_int& other) const 
    {
        if (negative == other.negative) 
        {
            super_int result;
            result.clear();
            result.negative = negative;

            Node* p = head;
            Node* q = other.head;
            int carry = 0;

            while (p || q || carry) 
            {
                int sum = carry;
                if (p) { sum += p->digit; p = p->next; }
                if (q) { sum += q->digit; q = q->next; }
                result.push_back(sum % 10);
                carry = sum / 10;
            }
            return result;
        }
        else 
        {
            if (negative) 
            {
                super_int temp = *this;
                temp.negative = false;
                return other - temp;
            }
            else 
            {
                super_int temp = other;
                temp.negative = false;
                return *this - temp;
            }
        }
    }

    super_int operator-(const super_int& other) const 
    {
        if (!negative && other.negative) 
        {
            super_int temp = other;
            temp.negative = false;
            return *this + temp;
        }
        if (negative && !other.negative) 
        {
            super_int temp = *this;
            temp.negative = false;
            super_int res = temp + other;
            res.negative = true;
            return res;
        }
        if (negative && other.negative) 
        {
            super_int temp1 = *this;
            super_int temp2 = other;
            temp1.negative = false;
            temp2.negative = false;
            return temp2 - temp1;
        }

        if (*this < other) 
        {
            super_int res = other - *this;
            res.negative = true;
            return res;
        }

        super_int result;
        result.clear();

        Node* p = head;
        Node* q = other.head;
        int borrow = 0;

        while (p) 
        {
            int sub = p->digit - borrow;
            if (q) { sub -= q->digit; q = q->next; }
            if (sub < 0) {
                sub += 10;
                borrow = 1;
            }
            else 
            {
                borrow = 0;
            }
            result.push_back(sub);
            p = p->next;
        }

        result.remove_leading_zeros();
        return result;
    }

    super_int operator*(const super_int& other) const 
    {
        super_int result(0);
        Node* p = head;
        int pos = 0;

        while (p) 
        {
            super_int temp;
            temp.clear();
            for (int i = 0; i < pos; i++) temp.push_back(0);

            Node* q = other.head;
            int carry = 0;
            while (q || carry) 
            {
                long long mul = carry;
                if (q) { mul += p->digit * q->digit; q = q->next; }
                temp.push_back(mul % 10);
                carry = mul / 10;
            }

            result = result + temp;
            p = p->next;
            pos++;
        }

        result.negative = (negative != other.negative);
        result.remove_leading_zeros();
        return result;
    }

    super_int operator/(const super_int& other) const 
    {
        assert(!other.is_zero());
        super_int result(0);
        super_int current(0);
        result.clear();

        int len = size;
        int* temp_digits = new int[len];
        Node* p = head;
        for (int i = 0; i < len; i++) 
        {
            temp_digits[i] = p->digit;
            p = p->next;
        }

        for (int i = len - 1; i >= 0; i--) 
        {
            current.shift_left();
            current.head->digit = temp_digits[i];
            current.remove_leading_zeros();
            int l = 0, r = 9, x = 0;
            while (l <= r) {
                int m = (l + r) / 2;
                super_int trial = other.abs() * super_int(m);
                if (trial <= current.abs()) {
                    x = m;
                    l = m + 1;
                }
                else {
                    r = m - 1;
                }
            }
            result.push_front(x);
            current = current - other.abs() * super_int(x);
        }

        delete[] temp_digits;
        result.negative = (negative != other.negative);
        result.remove_leading_zeros();
        return result;
    }

    void print_decimal(const super_int& divisor, int decimals = 20) const 
    {
        super_int dividend = *this;
        super_int div = divisor;

        if (dividend.negative != div.negative) cout << '-';

        dividend.negative = false;
        div.negative = false;

        super_int int_part = dividend / div;
        int_part.print();
        cout << '.';

        super_int rem = dividend - (div * int_part);
        for (int i = 0; i < decimals; i++) 
        {
            rem = rem * super_int(10);
            super_int digit = rem / div;
            digit.print();
            rem = rem - div * digit;
        }
    }

    super_int factorial() const 
    {
        super_int result(1);
        super_int one(1);
        super_int counter = *this;
        while (!counter.is_zero()) 
        {
            result = result * counter;
            counter = counter - one;
        }
        return result;
    }

    bool operator<(const super_int& other) const 
    {
        if (size != other.size) return size < other.size;
        return compare_reverse(head, other.head) < 0;
    }

    bool operator==(const super_int& other) const 
    {
        if (size != other.size || negative != other.negative) return false;
        Node* p = head;
        Node* q = other.head;
        while (p && q) {
            if (p->digit != q->digit) return false;
            p = p->next;
            q = q->next;
        }
        return true;
    }

    bool operator<=(const super_int& other) const 
    {
        return (*this < other || *this == other);
    }

private:
    void push_back(int digit) 
    {
        if (!head) {
            head = new Node(digit);
            size = 1;
        }
        else {
            Node* curr = head;
            while (curr->next) curr = curr->next;
            curr->next = new Node(digit);
            size++;
        }
    }

    void push_front(int digit) 
    {
        head = new Node(digit, head);
        size++;
    }

    void clear() 
    {
        while (head) 
        {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        head = nullptr;
        size = 0;
    }

    void copy_from(const super_int& other) 
    {
        negative = other.negative;
        size = other.size;
        head = nullptr;
        if (!other.head) return;

        Node* tail = nullptr;
        for (Node* p = other.head; p; p = p->next) 
        {
            Node* new_node = new Node(p->digit);
            if (!head) head = tail = new_node;
            else {
                tail->next = new_node;
                tail = new_node;
            }
        }
    }

    void print_reverse(Node* node) const 
    {
        if (!node) return;
        print_reverse(node->next);
        cout << node->digit;
    }

    void remove_leading_zeros() 
    {
        if (!head) return;
        Node* rev = reverse_list(head);
        while (rev && rev->digit == 0 && rev->next) 
        {
            Node* tmp = rev;
            rev = rev->next;
            delete tmp;
            size--;
        }
        head = reverse_list(rev);
    }

    Node* reverse_list(Node* node) const 
    {
        Node* prev = nullptr;
        while (node) {
            Node* next = node->next;
            node->next = prev;
            prev = node;
            node = next;
        }
        return prev;
    }

    bool is_zero() const 
    {
        return size == 1 && head && head->digit == 0;
    }

    void shift_left() 
    {
        push_front(0);
    }

    super_int abs() const 
    {
        super_int temp = *this;
        temp.negative = false;
        return temp;
    }

    int compare_reverse(Node* a, Node* b) const 
    {
        if (!a && !b) return 0;
        if (!a) return -1;
        if (!b) return 1;

        int cmp = compare_reverse(a->next, b->next);
        if (cmp != 0) return cmp;
        if (a->digit < b->digit) return -1;
        if (a->digit > b->digit) return 1;
        return 0;
    }
};

int main() 
{
    super_int a(349158);
    super_int b(412578);

    cout << "a = "; a.print(); cout << '\n';
    cout << "b = "; b.print(); cout << '\n';
    cout << "a + b = "; (a + b).print(); cout << '\n';
    cout << "a - b = "; (a - b).print(); cout << '\n';
    cout << "a * b = "; (a * b).print(); cout << '\n';

    super_int c(100);
    super_int d(3);
    cout << "100 / 3 = "; (c / d).print(); cout << '\n';
    cout << "100 / 3 (decimal) = "; c.print_decimal(d); cout << '\n';

    super_int e(20);
    cout << "20! = "; e.factorial().print(); cout << '\n';

    return 0;
}
