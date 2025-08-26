#include <crow.h>
#include <iostream>
#include <filesystem> 

int main()
{
    crow::mustache::set_base("templates");

    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){
        crow::mustache::context ctx;
        ctx["page_title"] = "sign_in";
        auto page = crow::mustache::load("sign_in.html").render(ctx);
        return page;
    });

    app.port(18080).multithreaded().run();
}
