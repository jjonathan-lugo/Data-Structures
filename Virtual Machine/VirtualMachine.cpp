#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

struct instr
{
    int op;
    int reg;
    int n;
};

const int max_memP = 1000;
const int max_memD = 1000;
const int max_regs = 16;
const int max_memS = 100;


instr memP[max_memP]; // Program memory
int memD[max_memD];   // Data memory
int regs[max_regs];   // Registers
string memS[max_memS]; // String memory

int pos_memP = 0, pos_memD = 0, pos_memS = 0;

struct ValPair
{
    string name;
    int val;
};

vector<ValPair> labels, strings, varnames;

bool debugPhase1 = false;
bool debugPhase2 = false;

void firstPass(ifstream& fi)
{
    string line;
    while (getline(fi, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        bool its_an_insturction = line[0] == ' ';

        istringstream si(line);
        string s;
        si >> s;

        if (debugPhase1)
            cout << "Line Read: " << line << endl;

        if (s == "label")
        {
            string labelName;
            si >> labelName;
            labels.push_back({ labelName, pos_memP });

            if (debugPhase1)
                cout << "  Label: " << labelName
                << " at pos_memP = " << pos_memP << endl;
        }
        else if (s == "data")
        {
            string varName;
            int value;
            si >> varName >> value;
            varnames.push_back({ varName, pos_memD });
            memD[pos_memD++] = value;

            if (debugPhase1)
                cout << "  Data: " << varName << " = " << value
                << " at memD[" << pos_memD - 1 << "]" << endl;
        }
        else if (s == "string")
        {
            string strName, strValue;
            si >> strName;
            getline(si, strValue);
            strings.push_back({ strName, pos_memS });
            memS[pos_memS++] = strValue.substr(1);

            if (debugPhase1)
                cout << "  String: " << strName << " = " << memS[pos_memS - 1]
                << " at memS[" << pos_memS - 1 << "]" << endl;
        }
        else
            pos_memP++;

        if (debugPhase1)
            cout << "  Instruction: Incrementing pos_memP to " << pos_memP << endl;
    }
}

void secondPass(ifstream& fi)
{
    string line;
    fi.clear();
    fi.seekg(0);

    pair<string, int> opcodeMap[] = {
     {"ldc", 1}, {"ldm", 2}, {"ldi", 3}, {"stm", 4}, {"sti", 5},
     {"add", 6}, {"sub", 7}, {"mul", 8}, {"div", 9}, {"mod", 10},
     {"skeq", 11}, {"skne", 12}, {"sklt", 13}, {"skle", 14},
     {"skgt", 15}, {"skge", 16}, {"jump", 17}, {"inn", 18},
     {"outn", 19}, {"outs", 20}, {"line", 21}, {"halt", 100}
    };
    const int opcodeCount = sizeof(opcodeMap) / sizeof(opcodeMap[0]);

    pos_memP = 0;

    while (getline(fi, line))
    {
        if (line.empty() || line[0] == '#' || line.find("label") == 0 ||
            line.find("data") == 0 || line.find("string") == 0)
            continue;

        istringstream si(line);
        string opcode;
        int reg = 0, n = 0;

        si >> opcode;

        int opcodeVal = -1;
        for (int i = 0; i < opcodeCount; ++i)
        {
            if (opcodeMap[i].first == opcode)
            {
                opcodeVal = opcodeMap[i].second;
                break;
            }
        }

        if (opcodeVal != -1)
        {
            instr instruction;
            instruction.op = opcodeVal;

            if (opcode == "inn" || opcode == "outn" || opcode == "line")
                si >> instruction.reg;
            else
                si >> instruction.reg >> instruction.n;

            if (opcode == "jump")
            {
                string labelName;
                istringstream jump(line);

                string label;
                int label_val;

                jump >> label >> label_val >> labelName;

                for (size_t i = 0; i < labels.size(); i++)
                {
                    if (labels[i].name == labelName)
                    {
                        instruction.n = labels[i].val;
                        break;
                    }
                }
            }

            memP[pos_memP++] = instruction;

            if (debugPhase2)
                cout << "Instruction: " << opcode << " reg: " << instruction.reg << " n: "
                << instruction.n << " at memP[" << pos_memP - 1 << "]" << endl;
        }
        else
        {
            cerr << "Error: Unknown opcode" << opcode << endl;
            exit(1);
        }
    }
}

void debug_prompt()
{
    char choice1, choice2;

    cout << "Enable debug option 1? (y/n): ";
    cin >> choice1;
    if (choice1 == 'y' || choice1 == 'Y')
        debugPhase1 = true;

    cout << "Enable debug option 2? (y/n): ";
    cin >> choice2;
    if (choice2 == 'y' || choice2 == 'Y')
        debugPhase2 = true;
}


int main()
{
    ifstream fi("alphabetical.txt");
    if (!fi)
    {
        cerr << "Error: Cannot open file." << '\n';
        return 1;
    }

    debug_prompt();

    firstPass(fi);

    cout << "\nFirst Pass\n";
    cout << "Labels: " << '\n' << '\n';
    for (size_t i = 0; i < labels.size(); i++)
        cout << "(" << labels[i].name << ", "
        << labels[i].val << ") " << '\n' << '\n';

    cout << "Variables: " << '\n' << '\n';
    for (size_t i = 0; i < varnames.size(); i++)
        cout << "(" << varnames[i].name << ", "
        << varnames[i].val << ") " << '\n' << '\n';

    cout << "Strings: " << '\n' << '\n';
    for (size_t i = 0; i < strings.size(); i++)
        cout << "(" << strings[i].name << ", "
        << strings[i].val << ") " << '\n' << '\n';

    secondPass(fi);

    if (fi.fail())
    {
        cerr << "Error: File reading failed during second pass.\n";
        return 1;
    }

    cout << "\nSecond Pass: Memory Program (memP) Translated Instructions:\n ";
    for (int i = 0; i < pos_memP; i++)
    {
        cout << "memP[" << i << "] = { Op: " << memP[i].op
            << ", Reg: " << memP[i].reg
            << ", N: " << memP[i].n << " }\n";
    }

    cout << "\nAlphabetical Table:\n";
    for (char i = 'A'; i <= 'Z'; i++)
    {
        cout << i << (char)(i + 32) << "  ";
        if (i == 'L' || i == 'X')
            cout << '\n';
    }

    fi.close();
    return 0;
}
