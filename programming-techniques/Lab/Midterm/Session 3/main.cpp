#include <iostream>
#include <cstring>
#include <stdlib.h>

using std::cin;
using std::cout;
using std::endl;

const int CHAR_ARR_LEN = 200;

struct Employee {
  int IDNumber;
  char name[CHAR_ARR_LEN];
  int age;
  float salary;
};

void addNewEmployee(Employee *employee, FILE *file) {
  FILE *tmpFile = fopen("./tmp.bin", "wb+");
  if (!file || !tmpFile) return;

  fseek(file, 0, SEEK_END);
  int fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  fwrite(employee, sizeof(Employee), 1, tmpFile);
  while (fread(employee, sizeof(Employee), 1, file) > 0) {
    fwrite(employee, sizeof(Employee), 1, tmpFile);
  }

  fclose(file);
  fclose(tmpFile);

  remove("./data.bin");
  rename("./tmp.bin", "./data.bin");
}

void printEmployee(Employee *employee) {
  if (!employee) return;

  cout << "|   ID Number: " << employee->IDNumber << endl;
  cout << "|   Name: " << employee->name << endl;
  cout << "|   Age: " << employee->age << endl;
  cout << "|   Salary: " << employee->salary << endl;
}

void printAllEmployees(Employee *employee, FILE *file) {
  if (!file) return;

  fseek(file, 0, SEEK_SET);
  while (fread(employee, sizeof(Employee), 1, file) > 0) {
    printEmployee(employee);
  }
}

void searchEmployeeByID(Employee *&employee, int IDNumber, FILE *file) {
  if (!file) {
    cout << ">>> Employee not found\n";
    return;
  }

  fseek(file, 0, SEEK_SET);

  while (fread(employee, sizeof(Employee), 1, file) > 0) {
    if (employee->IDNumber == IDNumber) {
      cout << ">>> Employee found\n";
      printEmployee(employee);
      return;
    }
  }

  cout << ">>> Employee not found\n";
}

void showTop3EmployeesWithHighestSalary(Employee *employee, FILE *file) {
  if (!file) return;

  int salaryNo1 = -1;
  int salaryNo2 = -1;
  int salaryNo3 = -1;

  fseek(file, 0, SEEK_SET);
  while (fread(employee, sizeof(Employee), 1, file) > 0) {
    if (salaryNo1 == -1 || employee->salary > salaryNo1) {
      salaryNo3 = salaryNo2;
      salaryNo2 = salaryNo1;
      salaryNo1 = employee->salary;
    } else if (salaryNo2 == -1 || employee->salary > salaryNo2) {
      salaryNo3 = salaryNo2;
      salaryNo2 = employee->salary;
    } else if (salaryNo3 == -1 || employee->salary > salaryNo3) {
      salaryNo3 = employee->salary;
    }
  }

  int count = (salaryNo1 > 0) + (salaryNo2 > 0) + (salaryNo3 > 0);

  while (count >= 1) {
    fseek(file, 0, SEEK_SET);
    while (fread(employee, sizeof(Employee), 1, file) > 0) {
      if ((employee->salary == salaryNo1 && count == 3) ||
          (employee->salary == salaryNo2 && count == 2) ||
          (employee->salary == salaryNo3 && count == 1)) {
        cout << ">>> Employee\n";
        printEmployee(employee);
        break;
      }
    }

    count--;
  }
}

void deleteEmployeeByID(Employee *employee, int IDNumber, FILE *file) {
  if (!file) return;

  FILE *tempFile = fopen("./temp.bin", "wb+");
  if (!tempFile) return;

  fseek(file, 0, SEEK_SET);

  while (fread(employee, sizeof(Employee), 1, file) > 0) {
    if (employee->IDNumber != IDNumber) {
      fwrite(employee, sizeof(Employee), 1, tempFile);
    }
  }

  fclose(file);
  fclose(tempFile);

  remove("./data.bin");
  rename("./temp.bin", "./data.bin");
}

void showMenu() {
  cout << "1. Input and add new Employee to the beginning of \"data.bin\"\n";
  cout
      << "2. Search for Employee by ID Number and display employee's details\n";
  cout << "3. Find and display top 3 employees which have the highest salary\n";
  cout << "4. Delete the employee's record which has a given ID number.\n";
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

    Employee *employee = new Employee;
    Employee *employeeFound = new Employee;
    int IDNumber = 0;

    switch (choice) {
      case 1:
        cout << "Enter Employee's ID Number: ";
        cin >> employee->IDNumber;

        cout << "Enter Employee's name: ";
        cin.ignore();
        cin.getline(employee->name, CHAR_ARR_LEN);

        cout << "Enter Employee's age: ";
        cin >> employee->age;

        cout << "Enter salary: ";
        cin >> employee->salary;

        addNewEmployee(employee, file);
        break;
      case 2:
        cout << "Enter Employee's ID Number: ";
        cin >> IDNumber;

        searchEmployeeByID(employee, IDNumber, file);
        break;
      case 3:
        showTop3EmployeesWithHighestSalary(employee, file);
        break;
      case 4:
        cout << "Enter Employee's ID Number: ";
        cin >> IDNumber;

        deleteEmployeeByID(employee, IDNumber, file);
        break;

      case 0:
        isStop = true;
        break;

      default:
        cout << "Invalid choice\n";
        break;
    }

    fclose(file);

    delete employee;
    delete employeeFound;
  } while (!isStop);

  return 0;
}