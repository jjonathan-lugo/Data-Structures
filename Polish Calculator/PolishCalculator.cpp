#include <iostream>
#include <vector>
#include <cmath>
#include <cctype> 

using namespace std;

struct PolishCalc
{
	vector <double> stack;
	vector <char> var_name;
	vector <double> var_values;
	bool undefined = false;

	double insert(double num)
	{
		stack.push_back(num);
		return 0;
	}

	double take_out()
	{
		if (stack.empty())
		{
			cout << "Error\n";
			return 0;
		}
		double value = stack.back();
		stack.pop_back();
		return value;
	}

	void input_variables(const string& input)
	{
		for (int i = 0; i < input.length(); i++)
		{
			if (isalpha(input[i]) && !var_existance(input[i]))
			{
				cout << "Enter value for " << input[i] << ": ";
				double val;
				cin >> val;
				var_name.push_back(input[i]);
				var_values.push_back(val);
			}
		}
	}

	bool var_existance(char var)
	{
		for (int i = 0; i < var_name.size(); i++)
			if (var_name[i] == var)
				return true;
		return false;
	}

	double var_to_num(char var)
	{
		for (int i = 0; i < var_name.size(); i++)
		{
			if (var_name[i] == var)
				return var_values[i];
		}
		return 0;
	}

	double stack_error() {
		cout << "Error: Not enough operands for operation\n";
		undefined = true;
		return 0;
	}

	double read_polishcalc()
	{
		char input[100];
		cout << "Enter Math Problem: ";
		cin.getline(input, 100);
		input_variables(input);

		char buffer[20];
		int buffer_pos = 0;

		int i = 0;
		while (input[i] != '\0')
		{
			if (input[i] >= '0' && input[i] <= '9' || input[i] == '.')
			{
				buffer[buffer_pos++] = input[i];
			}
			else if (input[i] == ' ' && buffer_pos > 0)
			{
				buffer[buffer_pos] = '\0';
				insert(atof(buffer));
				buffer_pos = 0;
			}

			else if (isalpha(input[i]))
			{
				if (var_existance(input[i]))
					insert(var_to_num(input[i]));
				else
				{
					cout << "Error" << input[i] << "variable not defined." << '\n';
					undefined = true;
					return 0;
				}
			}

			else if (input[i] == '+')
			{
				if (stack.size() < 2) 
					return stack_error();
				insert(take_out() + take_out());
			}
			else if (input[i] == '-')
			{
				if (stack.size() < 2)
					return stack_error();
				double a = take_out();
				double b = take_out();
				insert(b - a);
			}
			else if (input[i] == '*')
			{
				if (stack.size() < 2)
					return stack_error();
				insert(take_out() * take_out());
			}
			else if (input[i] == '/')
			{
				if (stack.size() < 2)
					return stack_error();
				double top = take_out();
				double bot = take_out();
				if (top == 0)
				{
					undefined = true;
					return 0;
				}
				insert(bot / top);
			}
			else if (input[i] == '^')
			{
				if (stack.size() < 2)
					return stack_error();
				double num = take_out();
				double raised = take_out();
				insert(pow(raised, num));
			}
			i++;
		}

		if (buffer_pos > 0)
		{
			buffer[buffer_pos] = '\0';
			insert(atof(buffer));
		}

		if (stack.size() != 1) 
		{
			cout << "Error: Invalid expression (too many operands or not enough operations)\n";
			undefined = true;
			return 0;
		}

		return take_out();
	}
};

int main()
{
	PolishCalc x;
	double result = x.read_polishcalc();
	if (x.undefined)
		cout << "Answer: undefined";
	else if (result == (int)result)
		cout << "Answer: " << (int)result << '\n';
	else
		cout << "Answer: " << result << '\n';
	return 0;
}
