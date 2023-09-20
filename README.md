# jsonh
My C++ JSON library

# Example code
```cpp
#include <iostream>
#include "jsonh.hpp"

int main()
{
	jsonh::Object obj;
	obj.set("name", new jsonh::String("John"));
	obj.set("age", new jsonh::Number(30));
	obj.set("isStudent", new jsonh::Boolean(false));

    std::ofstream outputFile("output.json");
    if (outputFile.is_open()) {
        outputFile << obj.indent(5); // Indentation
        outputFile.close();
        std::cout << "JSON saved in 'output.json' file'" << std::endl;
    }
    else {
        std::cerr << "Failed to open file for writing." << std::endl;
    }
}
```
