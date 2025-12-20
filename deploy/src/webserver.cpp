//
// Created by autoboat on 21/12/25.
//

#include "webserver.h"
#include "external/crow_websvr.hpp"

void start_webserver() {
  static crow::SimpleApp app; //define your crow application

  //define your endpoint at the root directory (serve static control page)
  CROW_ROUTE(app, "/")([](){
      std::ifstream ifs("web/control.html");
      if (!ifs) {
          return crow::response(404);
      }
      std::ostringstream ss;
      ss << ifs.rdbuf();
      crow::response res(ss.str());
      res.set_header("Content-Type", "text/html");
      return res;
  });


  CROW_ROUTE(app, "/left/<int>")
  ([](int a)
  {
      std::cout << "Left: " << a << std::endl;
      return std::to_string(a);
  });

  CROW_ROUTE(app, "/right/<int>")
  ([](int a)
  {
      std::cout << "Right: " << a << std::endl;
      return std::to_string(a);
  });

  CROW_ROUTE(app, "/nav/on")
  ([]()
  {
      std::cout << "Navigator ON - Let's gooo" << std::endl;
      return "Navigator ON";
  });

  CROW_ROUTE(app, "/nav/off")
  ([]()
  {
      std::cout << "Navigator OFF - Probs crashed :(" << std::endl;
      return "Navigator OFF";
  });

  //set the port, set the app to run on multiple threads, and run the app
  static auto _a = app.port(18080).multithreaded().run_async();

}


