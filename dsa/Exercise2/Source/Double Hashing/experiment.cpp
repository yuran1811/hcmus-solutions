#include "hash.h"

#include <functional>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

time_point<system_clock> start, stop;

typedef void (*FuncPointer)();

template <class T>
using FuncType = function<T()>;

template <class T>
using TPointer = T*;

struct Book {
  string title;
  string author;
};

Book extractToBook(string line) {
  int pos = line.find("|");
  return Book{line.substr(0, pos), line.substr(pos + 1, line.size() - pos - 1)};
}

void readInput(vector<Book>& books) {
  fstream fin("books.txt");

  string line = "";
  getline(fin, line);

  while (getline(fin, line)) {
    books.push_back(extractToBook(line));
  }
}

template <class T>
void measureTime(FuncType<T> func) {
  start = high_resolution_clock::now();
  func();
  stop = high_resolution_clock::now();

  auto elapsed_time = duration_cast<duration<long long, micro>>(stop - start);
  cout << "Elapsed Time: " << elapsed_time.count() << " microseconds\n\n";
}

int main() {
  vector<Book> books;
  readInput(books);

  HashTable<string, string> table;
  table.init(int(3 * 1e5));

  for (Book book : books) {
    table.add(book.title, book.author);
  }

  cout << countCollision << endl;

  // const int idx = -1;
  // const int idx = 0;
  // const int idx = books.size() >> 1;
  const int idx = books.size() - 1;
  // Book bookToFind = Book{"Tim ve binh yen", "Ngo Nguyen The Khoa"};
  Book bookToFind = books[idx];
  cout << "Searching for book: " << idx << "th\n\n";

  function<void()> searchKey = [&]() -> void {
    cout << ">>> Using Linear Search: ";
    for (Book book : books)
      if (book.title == bookToFind.title) return void(cout << "Found\n");

    cout << "Not found\n";
  };

  function<void()> searchKeyWithHash = [&]() -> void {
    cout << ">>> Using Hash Table: ";

    string* v = table.searchValue(bookToFind.title);
    if (v)
      cout << "Found\n";
    else
      cout << "Not found\n";
  };

  measureTime(searchKey);
  measureTime(searchKeyWithHash);

  table.release();

  return 0;
}