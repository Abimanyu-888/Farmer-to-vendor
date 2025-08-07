#include "crow.h"
#include <iostream>

int main()
{
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        return "Hello world!";
    });
    app.port(18080).multithreaded().run();
}
