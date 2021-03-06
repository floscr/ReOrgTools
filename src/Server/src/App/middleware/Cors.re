/* CORS: allow access from page served by parcel server */
let authorizeOrigin:
  ((Express.Request.t, Express.Response.t)) =>
  (option(string), Express.Response.t) =
  ((req, res)) =>
    switch (Express.Request.get("Origin", req)) {
    | Some(origin) as result =>
      Js.String.startsWith("http://localhost:8080", origin)
        ? (result, res) : (None, res)
    | None => (None, res)
    };

let attachCorsHeaders:
  ((option(string), Express.Response.t)) => Express.Response.t =
  ((maybeOrigin, res)) =>
    switch (maybeOrigin) {
    | Some(origin) =>
      Express.Response.setHeader("Access-Control-Allow-Origin", origin, res)
      ->Express.Response.setHeader(
          "Access-Control-Allow-Headers",
          "Origin, X-Requested-With, Content-Type, Accept",
          _,
        )
    | None => res
    };

let corsHandler:
  (Express.Middleware.next, Express.Request.t, Express.Response.t) =>
  Express.complete =
  (next, req, res) =>
    authorizeOrigin((req, res))
    ->attachCorsHeaders
    ->next(Express.Next.middleware, _);

let t = Express.Middleware.from(corsHandler);
