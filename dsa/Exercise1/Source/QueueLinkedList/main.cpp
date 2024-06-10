#include <iostream>
#include "queue.h"
#include "recursive_queue.h"
#include "funcs.h"

using std::cin;
using std::cout;

typedef int QueueDataType;
typedef Queue<QueueDataType> QueueType;
typedef RecursiveQueue<QueueDataType> RecursiveQueueType;

int main() {
  // QueueType* q = nullptr;
  RecursiveQueueType* q = nullptr;
  unsigned int queue_size = 0;

  for (int choice = 0, isExit = 0; !isExit;) {
    if (q) {
      cout << "-----------\n";
      cout << "| Queue elements: ";
      q->print();
      cout << "\n-----------\n";
    } else
      cout << "-----------\n| Queue is not initialized.\n-----------\n";

    getMenuChoice(choice);
    if (!choice) {
      cout << "Have a nice day!\n";
      break;
    }

    switch (choice) {
      case 1:
        if (!q &&
            !queue_uninitialized_handler<RecursiveQueueType>(queue_size, q)) {
          cout << "!: Please init the queue before enqueueing.\n";
          break;
        }

        try {
          cout << "=: Enter value to be pushed: ";
          QueueDataType value = 0;
          cin >> value;

          q->enqueue(value);
        } catch (const char* e) {
          cout << "!: " << e << '\n';
        }
        break;
      case 2:
        if (!q &&
            !queue_uninitialized_handler<RecursiveQueueType>(queue_size, q)) {
          cout << "!: Please init the queue before dequeueing.\n";
          break;
        }

        try {
          const QueueDataType pop_value = q->dequeue();
          cout << ">>> Front value: " << pop_value << '\n';
        } catch (const char* e) {
          cout << "!: " << e << '\n';
        }
        break;

      default:
        cout << "Invalid choice.\n";
        break;
    }

    cout << "\n?: Continue: Yes(1) - No(0)\n";
    int c = 0;
    cin >> c;
    isExit = c == 0;

    system("cls");
  }

  if (q) q->release();
  delete q;

  return 0;
}
