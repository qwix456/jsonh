# jsonh
JSON Library written in C++

# Example code
```cpp
#include "json.h"

using json_example = skyue::jsonh::JSON;

int main()
{
    json_example json;
    json["test"] = "value";
    json.json_to_file("test.json");
}
```
