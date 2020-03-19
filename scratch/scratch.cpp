#include <iostream>

class Animal {
public:
  virtual void hello() {}
};

class Dog : public Animal {
public:
  void hello() { std::cout << "woof!" << std::endl; }
};

class Cat : public Animal {
public:
  void hello() { std::cout << "meow" << std::endl; }
};

int main() {
  Dog d;
  d.hello(); // prints "woof!"

  Cat c;
  c.hello(); // prints "meow"

  // how are we allowed to create an array of abstract class?
  // doesn't c++ need to know how to allocate memory for any abstract
  // class in order to do this?
  Animal creatures[5];
  creatures[0] = d;
  creatures[1] = c;
  creatures[4] = d;

  // prints "6Animal"
  std::cout << typeid(creatures[0]).name() << std::endl;

  // this appears to call the Animal hello(), which does nothing
  creatures[1].hello();
}
