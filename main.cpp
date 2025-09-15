#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "crow/middlewares/session.h"
#include <iostream>
#include <filesystem>
#include "include/load.hpp"
#include <vector>
#include <string>
#include <any> 

using Session = crow::SessionMiddleware<crow::InMemoryStore>;

int main() {
    crow::mustache::set_base("templates");
    hash_tables tables;

    crow::App<crow::CookieParser, Session> app;

    app.get_middleware<Session>();

    CROW_ROUTE(app, "/")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        if (user_type.empty()) {
            crow::response res(303);
            res.add_header("Location", "/landing_page");
            return res;
        }

        crow::response res(303);
        if (user_type == "Farmer") {
            res.add_header("Location", "/farmer");
        } else {
            res.add_header("Location", "/buyer");
        }
        return res;
    });

    CROW_ROUTE(app, "/landing_page")([]() {
        auto page = crow::mustache::load("landing_page.html");
        return page.render();
    });

    CROW_ROUTE(app, "/farmer")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");

        // Authentication Check
        if (user_type != "Farmer") {
            crow::response res(303);
            res.add_header("Location", "/landing_page");
            return res;
        }

        std::string username = session.get<std::string>("username");
        crow::mustache::context ctx;
        ctx["username"] = username;
        auto page = crow::mustache::load("farmer/farmer_dashboard.html");
        return crow::response(page.render(ctx));
    });

    CROW_ROUTE(app, "/buyer")([&app](const crow::request& req) -> crow::response {
        auto& session = app.get_context<Session>(req);
        std::string user_type = session.get<std::string>("user_type");
        
        // Authentication Check
        if (user_type != "Buyer") {
            crow::response res(303);
            res.add_header("Location", "/landing_page");
            return res;
        }

        auto page = crow::mustache::load("buyer/buyer_home.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/sign_in")([]() {
        auto page = crow::mustache::load("sign_in.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_in_post").methods("POST"_method)([&app, &tables](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string email = req_body.get("email");
        std::string pass = req_body.get("password");
        std::string type = req_body.get("type");
        std::string* user = tables.findUsername(email);

        if (!user) {
            res.redirect("/error");
            res.end();
            return;
        }

        bool login_success = false;
        if (type == "Farmer") {
            farmer_data* data = tables.findFarmer(*user);
            if (data && data->password == pass) {
                login_success = true;
            }
        } else {
            buyer_data* data = tables.findBuyer(*user);
            if (data && data->password == pass) {
                login_success = true;
            }
        }

        if (login_success) {
            auto& session = app.get_context<Session>(req);
            session.set("username", *user);
            session.set("user_type", type);

            res.code = 303;
            if (type == "Farmer") {
                res.add_header("Location", "/farmer");
            } else {
                res.add_header("Location", "/buyer");
            }
        } else {
            res.code = 303;
            res.add_header("Location", "/error");
        }
        res.end();
    });

    CROW_ROUTE(app, "/sign_up")([]() {
        auto page = crow::mustache::load("sign_up.html");
        return page.render();
    });

    CROW_ROUTE(app, "/sign_up_post").methods("POST"_method)([&app, &tables](const crow::request& req, crow::response& res) {
        auto req_body = crow::query_string(("?" + req.body).c_str());
        std::string name = req_body.get("name");
        std::string username = req_body.get("username");
        std::string email = req_body.get("email");
        std::string pass = req_body.get("password");
        std::string state = req_body.get("state");
        std::string type = req_body.get("type");

        auto& session = app.get_context<Session>(req);
        session.set("username", username);
        session.set("user_type", type);

        res.code = 303;
        if (type == "Farmer") {
            std::string farmname = req_body.get("farm_name");
            farmer_data* new_user = new farmer_data(name, username, email, pass, state);
            tables.addFarmer(new_user);
            res.add_header("Location", "/farmer");
        } else {
            buyer_data* new_user = new buyer_data(name, username, email, pass, state);
            tables.addBuyer(new_user);
            res.add_header("Location", "/buyer");
        }
        res.end();
    });

    CROW_ROUTE(app, "/error")([]() {
        auto page = crow::mustache::load("error.html");
        return crow::response(page.render());
    });

    CROW_ROUTE(app, "/logout")([&app](const crow::request& req, crow::response& res) {
        auto& session = app.get_context<Session>(req);

        for (const auto& key : session.keys()) {
            session.remove(key);
        }

        res.add_header("Location", "/landing_page");
        res.code = 303;
        res.end();
    });

    app.bindaddr("0.0.0.0").port(18080).multithreaded().run();
}

