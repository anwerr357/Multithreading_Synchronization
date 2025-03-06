#include <iostream>
#include <thread>

using namespace std;

int balance = 100; // Compte bancaire avec 100 unités

void withdraw(int amount) {
    if (balance >= amount) { // Vérifie si le solde est suffisant
        this_thread::sleep_for(chrono::milliseconds(100)); // Simule un délai
        balance -= amount; // Retrait d'argent
        cout << "Retrait de " << amount << ". Nouveau solde : " << balance << endl;
    } else {
        cout << "Fonds insuffisants pour retirer " << amount << endl;
    }
}
void deposit(int amount) {
    balance += amount;
    cout << "Dépôt de " << amount << ". Nouveau solde : " << balance << endl;
}
int main() {
    thread t1(withdraw, 70);
    thread t2(withdraw, 70);
    //thread t3(deposit ,50);
    t1.join();
    t2.join();
    //t3.join();

    cout << "Solde final : " << balance << endl;
    return 0;
}
