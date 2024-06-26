#include <util/string/type.h>

#include <cstring>
#include <string>

bool CheckExceptionMessage(const char* msg, std::string& err) {
    static const char* badMsg[] = {
        // Операция успешно завершена [cp1251]
        "\xce\xef\xe5\xf0\xe0\xf6\xe8\xff\x20\xf3\xf1\xef\xe5\xf8\xed\xee\x20\xe7\xe0\xe2\xe5\xf0\xf8\xe5\xed\xe0",
        "The operation completed successfully",
        "No error"};

    err.clear();

    if (msg == nullptr) {
        err = "Error message is null";
        return false;
    }

    if (IsSpace(msg)) {
        err = "Error message is empty";
        return false;
    }

    for (auto& i : badMsg) {
        if (std::strstr(msg, i) != nullptr) {
            err = "Invalid error message: " + std::string(msg);
            return false;
        }
    }

    return true;
}
