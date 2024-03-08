#include <iostream>
#include <cstring>
#include <stdlib.h>

using std::cin;
using std::cout;
using std::endl;

const int CHAR_ARR_LEN = 200;

struct Car {
  char brand[CHAR_ARR_LEN];
  char model[CHAR_ARR_LEN];
  int year;
  float price;
};

void addNewCar(Car *car, FILE *file) {
  if (!file) return;

  fseek(file, 0, SEEK_END);
  fwrite(car, sizeof(Car), 1, file);
}

void printCars(Car *car, FILE *file) {
  if (!file) return;

  fseek(file, 0, SEEK_SET);

  int idx = 1;
  while (fread(car, sizeof(Car), 1, file) > 0) {
    cout << ">>> Car " << idx++ << endl;
    cout << "|   Brand: " << car->brand << endl;
    cout << "|   Model: " << car->model << endl;
    cout << "|   Year: " << car->year << endl;
    cout << "|   Price: " << car->price << endl;
  }
}

void adjustPrice(Car *car, FILE *file, char *brand, char *model, float price) {
  if (!file) return;

  fseek(file, 0, SEEK_SET);

  while (fread(car, sizeof(Car), 1, file) > 0) {
    if (strcmp(car->brand, brand) == 0 && strcmp(car->model, model) == 0) {
      car->price = price;
      fseek(file, (int)-sizeof(Car), SEEK_CUR);
      fwrite(car, sizeof(Car), 1, file);
      fseek(file, sizeof(Car), SEEK_CUR);
    }
  }
}

void deleteOldCars(Car *car, FILE *file) {
  if (!file) return;

  FILE *tempFile = fopen("./temp.bin", "wb+");
  if (!tempFile) return;

  fseek(file, 0, SEEK_SET);

  while (fread(car, sizeof(Car), 1, file) > 0) {
    if (car->year >= 1980) {
      fwrite(car, sizeof(Car), 1, tempFile);
    }
  }

  fclose(file);
  fclose(tempFile);

  remove("./data.bin");
  rename("./temp.bin", "./data.bin");
}

void showMenu() {
  cout << "1. Input and add new car to \"data.bin\"\n";
  cout << "2. Read and display the details of all cars from \"data.bin\"\n";
  cout << "3. Adjust car's price by brand and model\n";
  cout << "4. Delete all cars with the year is before 1980\n";
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

    Car *car = new Car;
    char *brand = new char[CHAR_ARR_LEN];
    char *model = new char[CHAR_ARR_LEN];

    switch (choice) {
      case 1:
        cout << "Enter car's brand: ";
        cin.ignore();
        cin.getline(car->brand, CHAR_ARR_LEN);

        cout << "Enter car's model: ";
        cin.getline(car->model, CHAR_ARR_LEN);

        cout << "Enter car's year: ";
        cin >> car->year;

        cout << "Enter price: ";
        cin >> car->price;

        addNewCar(car, file);
        break;
      case 2:
        printCars(car, file);
        break;
      case 3:
        cout << "Enter car's brand: ";
        cin.ignore();
        cin.getline(brand, CHAR_ARR_LEN);

        cout << "Enter car's model: ";
        cin.getline(model, CHAR_ARR_LEN);

        cout << "Enter new price: ";
        float price;
        cin >> price;

        adjustPrice(car, file, brand, model, price);
        break;
      case 4:
        deleteOldCars(car, file);
        break;

      case 0:
        isStop = true;
        break;

      default:
        cout << "Invalid choice\n";
        break;
    }

    fclose(file);

    delete car;
    delete[] brand;
    delete[] model;
  } while (!isStop);

  return 0;
}