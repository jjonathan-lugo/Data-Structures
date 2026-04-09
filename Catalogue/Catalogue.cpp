#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void replace_characters(string& str, char from, char to) 
{
    for (int i = 0; i < (int)str.length(); ++i) 
    {
        if (str[i] == from) 
        {
            str[i] = to;
        }
    }
}

struct Restaurant 
{
    string name;
    string owner;
    string address;

    Restaurant(string n, string o, string a)
        : name(n), owner(o), address(a) {}
};

struct TreeNode 
{
    Restaurant data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Restaurant r) : data(r), left(nullptr), right(nullptr) {}
};

class RestaurantTree 
{
private:
    TreeNode* root;

    TreeNode* insert(TreeNode* node, Restaurant r) 
    {
        if (!node)
            return new TreeNode(r);

        if (r.name < node->data.name) 
        {
            node->left = insert(node->left, r);
        }
        else if (r.name > node->data.name) 
        {
            node->right = insert(node->right, r);
        }
        return node;
    }

    void save_to_file(TreeNode* node, ofstream& fout) 
    {
        if (node == nullptr) 
        {
            return;
        }

        save_to_file(node->left, fout);

        string name = node->data.name;
        string owner = node->data.owner;
        string address = node->data.address;

        replace_characters(name, ' ', '_');
        replace_characters(owner, ' ', '_');
        replace_characters(address, ' ', '_');

        fout << name << " " << owner << " " << address << '\n';

        save_to_file(node->right, fout);
    }

    TreeNode* read_from_file(ifstream& infile) 
    {
        string line;
        TreeNode* newNode = nullptr;

        while (getline(infile, line)) 
        {
            stringstream ss(line);
            string name, owner, address;

            getline(ss, name, ' ');
            getline(ss, owner, ' ');
            getline(ss, address, ' ');

            if (name.empty() || owner.empty() || address.empty()) 
            {
                cerr << "Error: Missing data in file line: " << line << endl;
                continue;
            }

            replace_characters(name, '_', ' ');
            replace_characters(owner, '_', ' ');
            replace_characters(address, '_', ' ');

            cout << "Read from file: " << name << ", " << owner << ", " << address << endl;

            Restaurant r(name, owner, address);
            newNode = insert(newNode, r);
        }

        return newNode;
    }

    TreeNode* search(TreeNode* node, const string& name) 
    {
        if (node == nullptr || node->data.name == name)
            return node;
        if (name < node->data.name)
            return search(node->left, name);
        return search(node->right, name);
    }

public:
    RestaurantTree() : root(nullptr) {}

    void insert(Restaurant r) 
    {
        root = insert(root, r);
    }

    void save_to_file(const string& filename) 
    {
        ofstream fout(filename);
        if (!fout) 
        {
            cerr << "Unable to open file for saving!" << '\n';
            return;
        }
        save_to_file(root, fout); 
        fout.close();
    }

    void read_from_file(const string& filename) 
    {
        ifstream infile(filename);
        if (!infile) 
        {
            cerr << "Unable to open file!" << '\n';
            return;
        }
        root = read_from_file(infile);
        infile.close();
    }

    void search_partial_name(const string& partial) 
    {
        bool found = false;
        search_partial_name(root, partial, found);
        if (!found) 
        {
            cout << "No restaurants found matching \"" << partial << "\"." << endl;
        }
    }

    void search_partial_name(TreeNode* node, const string& partial, bool& found) 
    {
        if (!node) return;
        search_partial_name(node->left, partial, found);
        if (node->data.name.find(partial) != string::npos) 
        {
            cout << node->data.name << " - " << node->data.owner << ", " << node->data.address << '\n';
            found = true;
        }
        search_partial_name(node->right, partial, found);
    }

    TreeNode* search(const string& name) 
    {
        return search(root, name);
    }

    void print_in_order() 
    {
        print_in_order(root);
    }

    void add_new_restaurant() 
    {
        string name, owner, address;

        cout << "Enter the name of the restaurant: ";
        getline(cin, name);
        cout << "Enter the owner of the restaurant: ";
        getline(cin, owner);
        cout << "Enter the address of the restaurant: ";
        getline(cin, address);

        Restaurant r(name, owner, address);

        insert(r);

        cout << "New restaurant added successfully!" << '\n';
    }

private:
    void print_in_order(TreeNode* node) 
    {
        if (node == nullptr) 
        {
            return;
        }
        print_in_order(node->left);
        cout << node->data.name << " - " << node->data.owner << ", "
            << node->data.address << '\n';
        print_in_order(node->right);
    }
};

int main() 
{
    RestaurantTree tree;

    cout << "Reading from file and building the tree:" << '\n';
    tree.read_from_file("restaurants.txt");

    cout << "\nRestaurant Collection in Alphabetical Order:" << '\n';
    tree.print_in_order();

    char addOption;
    cout << "\nDo you want to add a new restaurant? (y/n): ";
    cin >> addOption;
    cin.ignore();
    if (addOption == 'y' || addOption == 'Y') 
    {
        tree.add_new_restaurant();
    }

    char searchOption;
    cout << "\nDo you want to search for a restaurant by name keyword? (y/n): ";
    cin >> searchOption;
    cin.ignore();
    if (searchOption == 'y' || searchOption == 'Y') 
    {
        string keyword;
        cout << "Enter part of the name to search for: ";
        getline(cin, keyword);
        tree.search_partial_name(keyword);
    }

    tree.save_to_file("restaurants.txt");

    return 0;
}
