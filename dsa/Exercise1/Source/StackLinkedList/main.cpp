#include <iostream>
#include "stack.h"
#include "recursive_stack.h"
#include "funcs.h"

using std::cin;
using std::cout;

typedef int StackDataType;
typedef Stack<StackDataType> StackType;
typedef RecursiveStack<StackDataType> RecursiveStackType;

int main() {
  // StackType* s = nullptr;
  RecursiveStackType* s = nullptr;
  unsigned int stack_size = 0;

  for (int choice = 0, isExit = 0; !isExit;) {
    if (s) {
      cout << "-----------\n";
      cout << "| Stack elements: ";
      s->print();
      cout << "\n-----------\n";
    } else
      cout << "-----------\n| Stack is not initialized.\n-----------\n";

    getMenuChoice(choice);
    if (!choice) {
      cout << "Have a nice day!\n";
      break;
    }

    switch (choice) {
      case 1:
        if (!s && !stack_uninitialized_handler<RecursiveStackType>(stack_size, s)) {
          cout << "!: Please init the stack before pushing.\n";
          break;
        }

        try {
          cout << "=: Enter value to be pushed: ";
          StackDataType value = 0;
          cin >> value;

          s->push(value);
        } catch (const char* e) {
          cout << "!: " << e << '\n';
        }
        break;
      case 2:
        if (!s && !stack_uninitialized_handler<RecursiveStackType>(stack_size, s)) {
          cout << "!: Please init the stack before poping.\n";
          break;
        }

        try {
          const StackDataType pop_value = s->pop();
          cout << ">>> Popped value: " << pop_value << '\n';
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

  if (s) s->release();
  delete s;

  return 0;
}
