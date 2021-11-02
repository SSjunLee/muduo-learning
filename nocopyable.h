#pragma once
class nocopyable {
protected:
  nocopyable(){};
  ~nocopyable(){};
private:
  nocopyable(const nocopyable& that);
  nocopyable& operator=(const nocopyable& that);
};
