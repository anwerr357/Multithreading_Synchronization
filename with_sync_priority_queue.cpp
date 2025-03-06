#include <iostream>
#include <thread>
#include <semaphore.h>
#include <queue>
#include <vector>
#include <atomic>

using namespace std;

int balance = 100;
sem_t sem; // semaphore for balance
sem_t queueSem; // semaphore for priority queue

enum TransactionType { WITHDRAW, DEPOSIT };

struct Transaction {
    int amount;
    int priority;
    TransactionType type;

    // Custom comparator for priority queue
    bool operator<(const Transaction& other) const {
        return priority < other.priority;
    }
};

priority_queue<Transaction> transactionQueue;
atomic<bool> doneAddingTransactions(false); // flag to indicate when adding is done
void processTransaction(const Transaction& transaction) {
    sem_wait(&sem);
    if (transaction.type == WITHDRAW) {
        if (balance >= transaction.amount) {
            this_thread::sleep_for(chrono::milliseconds(100));
            balance -= transaction.amount;
            cout << "Retrait de " << transaction.amount << ". Nouveau solde : " << balance << endl;
        } else {
            cout << "Fonds insuffisants pour retirer " << transaction.amount << endl;
        }
    } else if (transaction.type == DEPOSIT) {
        balance += transaction.amount;
        cout << "Dépôt de " << transaction.amount << ". Nouveau solde : " << balance << endl;
    }
    sem_post(&sem);
}

void transactionProcessor() {
    while (true) {
        sem_wait(&queueSem);
        if (!transactionQueue.empty()) {
            Transaction transaction = transactionQueue.top();
            transactionQueue.pop();

            processTransaction(transaction);
        
            sem_post(&queueSem);
        }
        else if(doneAddingTransactions.load()){
            cout<<"All transaction have been processed"<<endl;
            sem_post(&queueSem);

            break;
        } 
        else {
            sem_post(&queueSem);
            this_thread::sleep_for(chrono::milliseconds(10)); // Wait a bit before checking again
        }
    }
}

void addTransaction(int amount, int priority, TransactionType type) {
    sem_wait(&queueSem);
    transactionQueue.push({amount, priority, type});
    sem_post(&queueSem);
}

void transactionAdder() {
       // Create threads to add transactions
       thread t1(addTransaction, 70, 1, WITHDRAW); // Low priority withdraw
       thread t2(addTransaction, 50, 2, DEPOSIT);  // Medium priority deposit
       thread t3(addTransaction, 70, 3, WITHDRAW); // High priority withdraw
   
       // Wait for all threads to finish
       t1.join();
       t2.join();
       t3.join();
   
       doneAddingTransactions.store(true); // Indicate that adding is done
}

int main() {
    sem_init(&sem, 0, 1); // Initialize the semaphore to 1 (mutual exclusion)
    sem_init(&queueSem, 0, 1); // Initialize the semaphore for the queue to 1 (mutual exclusion)
    
    // Create a thread to add transactions
    thread adder(transactionAdder);

    // Create a thread to process transactions
    thread processor(transactionProcessor);

    adder.join();
    processor.join();
    this_thread::sleep_for(chrono::seconds(1)); // Allow some time for processing
    // processor.detach();
    // adder.detach();

    sem_destroy(&sem); // Destroy the semaphore for balance
    sem_destroy(&queueSem); // Destroy the semaphore for the queue
    cout << "Solde final : " << balance << endl;
    return 0;
}