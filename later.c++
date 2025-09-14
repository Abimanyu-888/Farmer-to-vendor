struct AuthMiddleware{

    inline static const std::vector<std::string> protected_paths = {
        "/farmer",
        "/buyer"
    };
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        for (const auto& path : protected_paths) {
            
            if (req.url.rfind(path, 0) == 0) { 
                
                std::string session_id = app.get_context<crow::CookieParser>(req).get_cookie("session_id");

                if (session_id.empty() || active_sessions.find(session_id) == active_sessions.end()) {
                    res.redirect("/langing_page.html");
                    res.end();
                }
                return;
            }
        }
    }
    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        
    }
};