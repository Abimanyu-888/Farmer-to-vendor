#include <crow.h>
#include <iostream>
#include <filesystem> 

int main()
{
    crow::mustache::set_base("templates");
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load_text("home.html");
        return page;
    });
    app.port(18080).multithreaded().run();
}
