//
// Created by autoboat on 21/12/25.
//

#include "webserver.h"

#include "external/crow_websvr.hpp"

std::future<void> start_webserver(std::shared_ptr<movement_controller> movement_controller_ptr) {
  static crow::SimpleApp app; //define your crow application
  static std::shared_ptr<movement_controller> movement;
  movement = movement_controller_ptr;
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


  CROW_ROUTE(app, "/left/forward/<int>")
  ([](int a)
  {
    movement->forward(0, a);
    return 200;
  });

  CROW_ROUTE(app, "/left/backward/<int>")
  ([](int a)
  {
    movement->backward(0, a);
      /*std::cout << "Left: " << a << std::endl;
      return std::to_string(a);*/
    return 200;
  });

  CROW_ROUTE(app, "/right/forward/<int>")
  ([](int a)
  {
    movement->forward(1, a);
    return 200;
  });


  CROW_ROUTE(app, "/right/backward/<int>")
  ([](int a)
  {
    movement->backward(1, a);
    return 200;
  });


  CROW_ROUTE(app, "/nav/on")
  ([]()
  {
    std::cout << "Navigator On - Let's goooooooo" << std::endl;
    movement->set_auto_control(1);
    return 200;
  });

  CROW_ROUTE(app, "/nav/off")
  ([]()
  {
    std::cout << "Navigator OFF - Probs crashed :(" << std::endl;
    movement->set_auto_control(0);
    return 200;
  });

  //set the port, set the app to run on multiple threads, and run the app
  auto _a =
    app
    .port(18080)
    .multithreaded()
    .loglevel(crow::LogLevel::Warning)
    .run_async();
  return _a;

}


