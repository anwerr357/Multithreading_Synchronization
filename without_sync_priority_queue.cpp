#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std;

int balance = 100;

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
}

void transactionProcessor() {
    while (true) {
        if (!transactionQueue.empty()) {
            Transaction transaction = transactionQueue.top();
            transactionQueue.pop();
            processTransaction(transaction);
        } else if (doneAddingTransactions.load()) {
            cout << "All transactions have been processed" << endl;
            break;
        } else {
            this_thread::sleep_for(chrono::milliseconds(10)); // Wait a bit before checking again
        }
    }
}

void addTransaction(int amount, int priority, TransactionType type) {
    transactionQueue.push({amount, priority, type});
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
    // Create a thread to add transactions
    thread adder(transactionAdder);

    // Create a thread to process transactions
    thread processor(transactionProcessor);

    adder.join();
    processor.join();

    cout << "Solde final : " << balance << endl;
    return 0;
}