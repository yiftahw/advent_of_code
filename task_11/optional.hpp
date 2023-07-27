#include <stdexcept>

template <typename T>
class Optional {
private:
    T value_;
    bool hasValue_;

public:
    // Default constructor creates an empty Optional
    Optional() : hasValue_(false) {}

    // Constructor for a value
    Optional(const T& value) : value_(value), hasValue_(true) {}

    // Destructor
    ~Optional() {
        if (hasValue_) {
            value_.~T();
        }
    }

    // Operator overloading for bool - Checks if the Optional object has a value.
    explicit operator bool() const noexcept {
        return hasValue_;
    }

    // Explicit checking for value - Checks if the Optional object has a value.
    bool hasValue() const noexcept {
        return hasValue_;
    }

    // Get the value or throw if no value - Retrieves the value if it exists, throws std::runtime_error otherwise.
    T getValue() const {
        if (!hasValue_) {
            throw std::runtime_error("No value in Optional");
        }
        return value_;
    }

    // Assignment operator for T type - Assigns a value directly to the Optional object.
    Optional& operator=(const T& value) {
        value_ = value;
        hasValue_ = true;
        return *this;
    }
};
