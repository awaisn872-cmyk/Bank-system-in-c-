#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <conio.h>
using namespace std;

class Account {
private:
    int accNo;
    string name;
    double balance;
    int pin;

public:
    Account() {}

    Account(int a, string n, double b, int p) {
        accNo = a;
        name = n;
        balance = b;
        pin = p;
    }

    int getAccNo() { return accNo; }
    string getName() { return name; }
    double getBalance() { return balance; }

    bool checkPin(int p) { return pin == p; }

    void deposit(double amt) {
        if (amt > 0) balance += amt;
    }

    bool withdraw(double amt) {
        if (amt > 0 && amt <= balance) {
            balance -= amt;
            return true;
        }
        return false;
    }

    void show() {
        cout << "\nAccNo: " << accNo
            << "\nName: " << name
            << "\nBalance: " << balance << endl;
    }

    void save(ofstream& file) {
        file << accNo << " " << name << " " << balance << " " << pin << endl;
    }
};

// FILE
void saveAccounts(vector<Account>& acc) {
    ofstream file("accounts.txt");
    for (auto& a : acc) a.save(file);
}

void loadAccounts(vector<Account>& acc) {
    acc.clear();
    ifstream file("accounts.txt");
    int a, p; string n; double b;

    while (file >> a >> n >> b >> p) {
        acc.push_back(Account(a, n, b, p));
    }
}

void logTransaction(string msg) {
    ofstream file("transactions.txt", ios::app);
    file << msg << endl;
}

// HIDDEN PIN
int inputPin() {
    string pass = "";
    char ch;

    while ((ch = _getch()) != 13) {
        if (ch == 8) {
            if (!pass.empty()) {
                pass.pop_back();
                cout << "\b \b";
            }
        }
        else {
            pass += ch;
            cout << '*';
        }
    }
    cout << endl;
    return stoi(pass);
}

// ADMIN
void adminPanel(vector<Account>& accounts) {
    int choice;

    do {
        cout << "\n--- ADMIN PANEL ---\n";
        cout << "1. View All Accounts\n";
        cout << "2. Delete Account\n";
        cout << "3. View Transactions\n";
        cout << "4. Exit\n";
        cout << "Enter: ";
        cin >> choice;

        if (choice == 1) {
            for (auto& a : accounts) {
                a.show();
                cout << "-----------------\n";
            }
        }
        else if (choice == 2) {
            int accNo;
            cout << "Enter account to delete: ";
            cin >> accNo;

            bool found = false;
            for (int i = 0; i < accounts.size(); i++) {
                if (accounts[i].getAccNo() == accNo) {
                    accounts.erase(accounts.begin() + i);
                    saveAccounts(accounts);
                    cout << "Deleted!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Account not found\n";
        }
        else if (choice == 3) {
            ifstream file("transactions.txt");
            string line;
            cout << "\n--- TRANSACTIONS ---\n";
            while (getline(file, line)) cout << line << endl;
        }

    } while (choice != 4);
}

int main() {
    system("color F0");

    vector<Account> accounts;
    loadAccounts(accounts);

    int choice;
    cout << "============================\n";
    cout << "||    Bank System         ||\n";
    cout<< " ============================\n";
 
    while (true) {
        cout << "\n====== Menu ======\n";
        cout << "1. Create Account\n";
        cout << "2. User Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit\n";
        cout << "Enter: ";
        cin >> choice;

        if (choice == 1) {
            int accNo, pin;
            string name;
            double bal;

            cout << "AccNo: "; cin >> accNo;
            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Balance: "; cin >> bal;

            if (bal < 0) {
                cout << "Invalid balance\n";
                continue;
            }

            cout << "PIN: "; pin = inputPin();

            accounts.push_back(Account(accNo, name, bal, pin));
            saveAccounts(accounts);

            cout << "Account Created!\n";
        }

        else if (choice == 2) {
            int accNo;
            cout << "AccNo: "; cin >> accNo;

            bool found = false;

            for (int i = 0; i < accounts.size(); i++) {
                if (accounts[i].getAccNo() == accNo) {

                    cout << "PIN: ";
                    int pin = inputPin();

                    if (!accounts[i].checkPin(pin)) {
                        cout << "Wrong PIN\n";
                        break;
                    }

                    int ch;
                    do {
                        cout << "\n--- USER MENU ---\n";
                        cout << "1. Deposit\n";
                        cout << "2. Withdraw\n";
                        cout << "3. Transfer\n";
                        cout << "4. Show\n";
                        cout << "5. Logout\n";
                        cout << "Enter: ";
                        cin >> ch;

                        if (ch == 1) {
                            double amt;
                            cout << "Enter amount: "; cin >> amt;

                            if (amt <= 0) {
                                cout << "Invalid amount\n";
                                continue;
                            }

                            accounts[i].deposit(amt);
                            accounts[i].show();

                            logTransaction("AccNo: " + to_string(accNo) + " Deposit: " + to_string(amt));
                            saveAccounts(accounts);
                        }

                        else if (ch == 2) {
                            double amt;
                            cout << "Enter amount: "; cin >> amt;

                            if (accounts[i].withdraw(amt)) {
                                accounts[i].show();
                                logTransaction("AccNo: " + to_string(accNo) + " Withdraw: " + to_string(amt));
                                saveAccounts(accounts);
                            }
                            else cout << "Low balance\n";
                        }

                        else if (ch == 3) {
                            int toAcc;
                            double amt;

                            cout << "Receiver AccNo: "; cin >> toAcc;
                            cout << "Amount: "; cin >> amt;

                            bool found2 = false;

                            for (int j = 0; j < accounts.size(); j++) {
                                if (accounts[j].getAccNo() == toAcc) {

                                    if (accounts[i].withdraw(amt)) {
                                        accounts[j].deposit(amt);

                                        logTransaction("Transfer from " + to_string(accNo) +
                                            " to " + to_string(toAcc) +
                                            " Amount: " + to_string(amt));

                                        saveAccounts(accounts);
                                        cout << "Transfer successful\n";
                                    }
                                    else {
                                        cout << "Insufficient balance\n";
                                    }

                                    found2 = true;
                                    break;
                                }
                            }

                            if (!found2) cout << "Receiver not found\n";
                        }

                        else if (ch == 4) {
                            accounts[i].show();
                        }

                    } while (ch != 5);

                    found = true;
                    break;
                }
            }

            if (!found) cout << "Account not found\n";
        }

        else if (choice == 3) {
            string user, pass;

            cout << "Admin username: "; cin >> user;
            cout << "Password: "; cin >> pass;

            if (user == "admin" && pass == "1234") {
                adminPanel(accounts);
            }
            else {
                cout << "Wrong admin credentials!\n";
            }
        }

        else if (choice == 4) {
            cout << "Goodbye!\n";
            break;
        }
    }
}