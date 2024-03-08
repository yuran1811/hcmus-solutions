#include <iostream>
#include <cstring>
#include <stdlib.h>

using std::cin;
using std::cout;
using std::endl;

const int MAX_CHAR_LEN = 200;

struct Ticket {
  int IDNumber;
  char movieName[MAX_CHAR_LEN];
  int seatNumber;
  float price;
};

void addNewTicket(Ticket *ticket, FILE *file) {
  if (!file) return;

  fseek(file, 0, SEEK_END);
  fwrite(ticket, sizeof(Ticket), 1, file);
}

void printTickets(Ticket *ticket, FILE *file) {
  if (!file) return;

  fseek(file, 0, SEEK_SET);

  int idx = 1;
  while (fread(ticket, sizeof(Ticket), 1, file) > 0) {
    cout << ">>> Ticket " << idx++ << endl;
    cout << "|   ID: " << ticket->IDNumber << endl;
    cout << "|   Movie name: " << ticket->movieName << endl;
    cout << "|   Seat number: " << ticket->seatNumber << endl;
    cout << "|   Price: " << ticket->price << endl;
  }
}

void searchTicket(Ticket *ticket, FILE *file, char *movieName, int seatNumber) {
  if (!file) return;

  fseek(file, 0, SEEK_SET);

  while (fread(ticket, sizeof(Ticket), 1, file) > 0) {
    if (strcmp(ticket->movieName, movieName) == 0 &&
        ticket->seatNumber == seatNumber) {
      cout << ">>> Ticket found\n";
      cout << "|   ID: " << ticket->IDNumber << endl;
      cout << "|   Movie name: " << ticket->movieName << endl;
      cout << "|   Seat number: " << ticket->seatNumber << endl;
      cout << "|   Price: " << ticket->price << endl;
      return;
    }
  }

  cout << ">>> Ticket not found\n";
}

void adjustPrice(Ticket *ticket, FILE *file, char *movieName, float price) {
  if (!file) return;

  fseek(file, 0, SEEK_SET);

  while (fread(ticket, sizeof(Ticket), 1, file) > 0) {
    if (strcmp(ticket->movieName, movieName) == 0) {
      ticket->price = price;
      fseek(file, (int)-sizeof(Ticket), SEEK_CUR);
      fwrite(ticket, sizeof(Ticket), 1, file);
      fseek(file, sizeof(Ticket), SEEK_CUR);
    }
  }
}

void showMenu() {
  cout << "1. Input and add new ticket to \"data.bin\"\n";
  cout << "2. Read and display the details of all tickets from \"data.bin\"\n";
  cout << "3. Search for ticket by movie name and seat number and display that "
          "ticket's details\n";
  cout << "4. Adjust ticket's price by name\n";
  cout << "0. Exit\n";
}

int main() {
  bool isStop = false;
  do {
    showMenu();

    cout << "Enter your choice: ";
    int choice = -1;
    cin >> choice;

    FILE *file = fopen("./data.bin", "rb+");
    if (!file) {
      file = fopen("./data.bin", "wb+");
    }

    Ticket *ticket = new Ticket;
    char *movieName = new char[MAX_CHAR_LEN];

    switch (choice) {
      case 1:
        cout << "Enter ID: ";
        cin >> ticket->IDNumber;

        cout << "Enter movie name: ";
        cin.ignore();
        cin.getline(ticket->movieName, MAX_CHAR_LEN);

        cout << "Enter seat number: ";
        cin >> ticket->seatNumber;

        cout << "Enter price: ";
        cin >> ticket->price;

        addNewTicket(ticket, file);
        break;
      case 2:
        printTickets(ticket, file);
        break;
      case 3:
        cout << "Enter movie name: ";
        cin.ignore();
        cin.getline(movieName, MAX_CHAR_LEN);

        cout << "Enter seat number: ";
        int seatNumber;
        cin >> seatNumber;

        searchTicket(ticket, file, movieName, seatNumber);
        break;
      case 4:
        cout << "Enter movie name: ";
        cin.ignore();
        cin.getline(movieName, MAX_CHAR_LEN);

        cout << "Enter new price: ";
        float price;
        cin >> price;

        adjustPrice(ticket, file, movieName, price);
        break;
      case 0:
        isStop = true;
        break;

      default:
        cout << "Invalid choice\n";
        break;
    }

    fclose(file);

    delete ticket;
    delete[] movieName;
  } while (!isStop);

  return 0;
}