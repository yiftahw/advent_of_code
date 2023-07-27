#include <stdexcept>

struct ErrorTypeTag {};

template <typename T, typename E>
class Expected {
private:
    T value_;
    E error_;
    bool hasValue_;

public:
    // Constructor for a legal value
    Expected(const T value) : value_(value), hasValue_(true) {}

    // Constructor for an error value
    Expected(ErrorTypeTag, E error) : error_(error), hasValue_(false) {}

    // Destructor
    ~Expected() {}

    Expected(const Expected& other)
    {
        hasValue_ = other.hasValue();
        if (hasValue_)
            value_ = other.getValue();
        else
            error_ = other.getError();
    }

    Expected& operator=(const Expected& other)
    {
        //checker to avoid self assignment
        if (this == &other) {
            return *this;
        }

        if(hasValue_ && other.hasValue())
        {
            value_ = other.getValue();
        }
        else if(hasValue_ && !other.hasValue())
        {
            hasValue_ = false;
            error_ = other.getError();
        }
        else if(!hasValue_ && other.hasValue())
        {
            hasValue_ = true;
            value_ = other.getValue();
        }
        else
        {
            error_ = other.getError();   
        }

        return *this;
    }

    // Operator overloading for bool - Checks if the Expected object has a value.
    explicit operator bool() const noexcept {
        return hasValue_;
    }

    // Explicit checking for value - Checks if the Expected object has a value.
    bool hasValue() const noexcept {
        return hasValue_;
    }

    // Get the value or throw if no value - Retrieves the value if it exists, throws std::runtime_error otherwise.
    T getValue() const {
        if (!hasValue_) {
            throw std::runtime_error("No value in Expected");
        }
        return value_;
    }

    // Get the error value or return the error held in memory (undefined behavior) - Retrieves the error value if it exists.
    E getError() const noexcept {
        return error_;
    }
};
