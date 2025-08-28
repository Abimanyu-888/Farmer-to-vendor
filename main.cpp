#include <crow.h>
#include <iostream>
#include <filesystem> 

int main()
{
    crow::mustache::set_base("templates");

    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        auto page = crow::mustache::load("landing_page.html");
        return page.render();
    });

     CROW_ROUTE(app, "/sign_in")([](){
        auto page = crow::mustache::load("sign_in.html");
        return page.render();
    });
     CROW_ROUTE(app, "/sign_up")([](){
        auto page = crow::mustache::load("sign_up.html");
        return page.render();
    });
    app.port(18080).multithreaded().run();
}
