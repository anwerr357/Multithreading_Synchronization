#include <iostream>
#include <thread>
#include <semaphore.h>

using namespace std;

int balance = 100;
sem_t sem; // Déclaration du sémaphore

void withdraw(int amount) {
    sem_wait(&sem); // ↓↓↓ Bloque l'accès aux autres threads
    if (balance >= amount) {
        this_thread::sleep_for(chrono::milliseconds(100));
        balance -= amount;
        cout << "Retrait de " << amount << ". Nouveau solde : " << balance << endl;
    } else {
        cout << "Fonds insuffisants pour retirer " << amount << endl;
    }
    sem_post(&sem); // ↑↑↑ Libère l'accès aux autres threads
}

int main() {
    sem_init(&sem, 0, 1); // Initialise le sémaphore à 1 (exclusion mutuelle)

    thread t1(withdraw, 70);
    thread t2(withdraw, 70);

    t1.join();
    t2.join();

    sem_destroy(&sem); // Détruit le sémaphore
    cout << "Solde final : " << balance << endl;
    return 0;
}
