/*
 * File:   PropertyMacro.h
 * Author: nguyentran
 *
 * Created on March 22, 2013, 2:27 PM
 */

#ifndef PROPERTYMACRO_H
#define PROPERTYMACRO_H

#define PROPERTY(property_type, property_name)                     \
private:                                                           \
  property_type property_name##_;                                  \
                                                                   \
public:                                                            \
  property_type property_name() const { return property_name##_; } \
  void set_##property_name(const property_type &value) {           \
    property_name##_ = value;                                      \
  }

#define PROPERTY_REF(property_type, property_name)            \
private:                                                      \
  property_type property_name##_;                             \
                                                              \
public:                                                       \
  property_type &property_name() { return property_name##_; } \
  void set_##property_name(const property_type &value) {      \
    property_name##_ = value;                                 \
  }

#define POINTER_PROPERTY(property_type, property_name)              \
private:                                                            \
  property_type* property_name##_;                                  \
                                                                    \
public:                                                             \
  property_type* property_name() const { return property_name##_; } \
  void set_##property_name(property_type* value) { property_name##_ = value; }

#define READ_ONLY_PROPERTY(property_type, property_name) \
private:                                                 \
  property_type property_name##_;                        \
                                                         \
public:                                                  \
  property_type property_name() const { return property_name##_; }

#define READ_ONLY_PROPERTY_REF(property_type, property_name) \
private:                                                     \
  property_type property_name##_;                            \
                                                             \
public:                                                      \
  property_type &property_name() { return property_name##_; }

#define WRITE_ONLY_PROPERTY(property_type, property_name) \
private:                                                  \
  property_type property_name##_;                         \
                                                          \
public:                                                   \
  void set_##property_name(const property_type &value) {  \
    property_name##_ = value;                             \
  }

#define READ_ONLY_POINTER_PROPERTY(property_type, property_name) \
private:                                                         \
  property_type* property_name##_;                               \
                                                                 \
public:                                                          \
  property_type* property_name() const { return property_name##_; }

#define DELETE_COPY_AND_MOVE(type)       \
  type(type const &) = delete;           \
  void operator=(type const &) = delete; \
  type(type &&) = delete;                \
  type &operator=(type &&) = delete;

#define PROPERTY_HEADER(property_type, property_name) \
private:                                              \
  property_type property_name##_;                     \
                                                      \
public:                                               \
  property_type property_name() const;                \
  void set_##property_name(const property_type &value);

#define POINTER_PROPERTY_HEADER(property_type, property_name) \
private:                                                      \
  property_type* property_name##_;                            \
                                                              \
public:                                                       \
  property_type* property_name() const;                       \
  void set_##property_name(property_type* value);

#define VIRTUAL_PROPERTY(property_type, property_name)                     \
private:                                                                   \
  property_type property_name##_;                                          \
                                                                           \
public:                                                                    \
  virtual property_type property_name() const { return property_name##_; } \
  virtual void set_##property_name(const property_type &value) {           \
    property_name##_ = value;                                              \
  }

#define VIRTUAL_PROPERTY_REF(property_type, property_name)            \
private:                                                              \
  property_type property_name##_;                                     \
                                                                      \
public:                                                               \
  virtual property_type &property_name() { return property_name##_; } \
  virtual void set_##property_name(const property_type &value) {      \
    property_name##_ = value;                                         \
  }

#endif /* PROPERTYMACRO_H */
