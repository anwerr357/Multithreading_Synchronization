#include <iostream>
#include <thread>
#include <semaphore.h>

using namespace std;

int balance = 100;
sem_t sem; // semaphore

void withdraw(int amount) {
    sem_wait(&sem); 
    if (balance >= amount) {
        this_thread::sleep_for(chrono::milliseconds(100));
        balance -= amount;
        cout << "Retrait de " << amount << ". Nouveau solde : " << balance << endl;
    } else {
        cout << "Fonds insuffisants pour retirer " << amount << endl;
    }
    sem_post(&sem);
}
void deposit(int amount) {
    sem_wait(&sem);
    balance += amount;
    cout << "Dépôt de " << amount << ". Nouveau solde : " << balance << endl;
    sem_post(&sem); 
}

int main() {
    sem_init(&sem, 0, 1); // Initialise le sémaphore à 1 (exclusion mutuelle)

    thread t1(withdraw, 70);
    thread t3(deposit, 50);
    thread t2(withdraw, 70);
    thread tt[10]; 
    t1.join();
    t3.join();
    t2.join();

    sem_destroy(&sem); 
    cout << "Solde final : " << balance << endl;
    return 0;
}
