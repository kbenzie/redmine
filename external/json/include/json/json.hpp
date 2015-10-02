// Copyright (C) 2015 Kenenth Benzie
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef JSON_HPP
#define JSON_HPP

#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace json {
// Enumerations
enum type {
  TYPE_OBJECT,
  TYPE_ARRAY,
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_BOOL,
  TYPE_NULL
};

// Forward declarations
class value;
typedef std::pair<const std::string, json::value> pair;

// API
json::value read(const std::string &string, bool diag_on = true);
std::string write(const json::value &value, const char *tab = "\t");

// Objects
class object {
 public:
  // Types
  typedef std::map<std::string, json::value>::iterator iterator;
  typedef std::map<std::string, json::value>::const_iterator const_iterator;

  // Constructors
  object();
  object(std::string key, json::value value);
  object(json::pair pair);
  template <typename Type>
  object(std::string key, Type value);
  object(std::initializer_list<json::pair> values);

  // Accessors
  void add(std::string key, json::value value);
  template <typename Type>
  void add(std::string key, Type value);
  void add(json::pair pair);

  json::value *get(std::string key);
  const json::value *get(std::string key) const;

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

 private:
  std::map<std::string, json::value> mEntries;
};

class array {
 public:
  // Types
  typedef std::vector<json::value>::iterator iterator;
  typedef std::vector<json::value>::const_iterator const_iterator;

  // Constructors
  array(std::initializer_list<json::value> values);
  template <typename... Args>
  array(Args... args);

  // Accessors
  void append(json::value value);
  template <typename Type>
  void append(Type value);
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  json::value &operator[](const size_t index);
  const json::value &operator[](const size_t index) const;
  json::value &at(const size_t index);
  const json::value &at(const size_t index) const;

  size_t size();
  size_t size() const;

 private:
  std::vector<json::value> mEntries;
};

class value {
 public:
  // Constructors
  value();  // NOTE: A null value
  value(json::object object);
  value(json::pair pair);
  value(json::array array);
  explicit value(int8_t number);
  explicit value(int16_t number);
  explicit value(int32_t number);
  explicit value(int64_t number);
  explicit value(uint8_t number);
  explicit value(uint16_t number);
  explicit value(uint32_t number);
  explicit value(uint64_t number);
  explicit value(float number);
  explicit value(double number);
  value(const char *string);
  value(std::string string);
  explicit value(bool boolean);

  // Accessors
  json::type &type();
  const json::type &type() const;
  json::object &object();
  const json::object &object() const;
  json::array &array();
  const json::array &array() const;
  double &number();
  const double &number() const;
  template <typename Number>
  Number number();
  template <typename Number>
  Number number() const;
  std::string &string();
  const std::string &string() const;
  bool &boolean();
  const bool &boolean() const;

 private:
  struct store_base {
    virtual ~store_base() {}
  };

  template <typename Type>
  struct store : public store_base {
    store(Type value) : value(value) {}
    Type value;
  };

  json::type mType;
  std::shared_ptr<store_base> mStore;
};

// Implementations
inline object::object() {}
inline object::object(std::string key, json::value value) {
  mEntries[key] = value;
}
inline object::object(json::pair pair) { mEntries.insert(pair); }
template <typename Type>
inline object::object(std::string key, Type value) {
  mEntries[key] = value;
}
inline object::object(std::initializer_list<json::pair> pairs) {
  mEntries.insert(pairs);
}

inline void object::add(std::string key, json::value value) {
  mEntries[key] = value;
}
template <typename Type>
inline void object::add(std::string key, Type value) {
  mEntries[key] = json::value(value);
}
inline void object::add(json::pair pair) { mEntries.insert(pair); }
inline json::value *object::get(std::string key) {
  auto iter = mEntries.find(key);
  if (mEntries.end() == iter) {
    return nullptr;
  }
  return &iter->second;
}
inline const json::value *object::get(std::string key) const {
  auto iter = mEntries.find(key);
  if (mEntries.end() == iter) {
    return nullptr;
  }
  return &iter->second;
}
inline object::iterator object::begin() { return mEntries.begin(); }
inline object::const_iterator object::begin() const { return mEntries.begin(); }
inline object::iterator object::end() { return mEntries.end(); }
inline object::const_iterator object::end() const { return mEntries.end(); }

inline array::array(std::initializer_list<json::value> values)
    : mEntries(values) {}
template <typename... Args>
inline array::array(Args... args)
    : mEntries(args...) {}

inline void array::append(json::value value) { mEntries.push_back(value); }
template <typename Type>
inline void array::append(Type value) {
  mEntries.push_back(json::value(value));
}
inline array::iterator array::begin() { return mEntries.begin(); }
inline array::const_iterator array::begin() const { return mEntries.begin(); }
inline array::iterator array::end() { return mEntries.end(); }
inline array::const_iterator array::end() const { return mEntries.end(); }
inline json::value &array::operator[](const size_t index) {
  return mEntries[index];
}
inline const json::value &array::operator[](const size_t index) const {
  return mEntries[index];
}
inline json::value &array::at(const size_t index) { return mEntries.at(index); }
inline const json::value &array::at(const size_t index) const {
  return mEntries.at(index);
}
inline size_t array::size() { return mEntries.size(); }
inline size_t array::size() const { return mEntries.size(); }

inline value::value() : mType(TYPE_NULL), mStore(nullptr) {}
inline value::value(json::object object)
    : mType(TYPE_OBJECT),
      mStore(std::make_shared<store<json::object>>(object)) {}
inline value::value(json::pair pair)
    : mType(TYPE_OBJECT), mStore(std::make_shared<store<json::object>>(pair)) {}
inline value::value(json::array array)
    : mType(TYPE_ARRAY), mStore(std::make_shared<store<json::array>>(array)) {}
inline value::value(int8_t number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(int16_t number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(int32_t number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(int64_t number)
    : mType(TYPE_NUMBER),
      mStore(std::make_shared<store<double>>(static_cast<double>(number))) {}
inline value::value(uint8_t number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(uint16_t number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(uint32_t number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(uint64_t number)
    : mType(TYPE_NUMBER),
      mStore(std::make_shared<store<double>>(static_cast<double>(number))) {}
inline value::value(float number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(double number)
    : mType(TYPE_NUMBER), mStore(std::make_shared<store<double>>(number)) {}
inline value::value(const char *string)
    : mType(TYPE_STRING),
      mStore(std::make_shared<store<std::string>>(string)) {}
inline value::value(std::string string)
    : mType(TYPE_STRING),
      mStore(std::make_shared<store<std::string>>(string)) {}
inline value::value(bool boolean)
    : mType(TYPE_BOOL), mStore(std::make_shared<store<bool>>(boolean)) {}

#define CAST(Type) static_cast<store<Type> *>(mStore.get())->value
inline json::type &value::type() { return mType; }
inline const json::type &value::type() const { return mType; }
inline json::object &value::object() { return CAST(json::object); }
inline const json::object &value::object() const { return CAST(json::object); }
inline json::array &value::array() { return CAST(json::array); }
inline const json::array &value::array() const { return CAST(json::array); }
inline double &value::number() { return CAST(double); }
inline const double &value::number() const { return CAST(double); }
template <typename Number>
Number value::number() {
  return static_cast<Number>(CAST(double));
}
template <typename Number>
Number value::number() const {
  return static_cast<Number>(CAST(double));
}
inline std::string &value::string() { return CAST(std::string); }
inline const std::string &value::string() const { return CAST(std::string); }
inline bool &value::boolean() { return CAST(bool); }
inline const bool &value::boolean() const { return CAST(bool); }
#undef CAST
}

#endif
