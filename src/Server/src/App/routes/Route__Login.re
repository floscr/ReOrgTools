open Express;
open ExpressUtils;
open Relude.Globals;

let t =
  PromiseMiddleware.from((_next, req, res) => {
    Shared__API__User.(
      Request.bodyJSON(req)
      |> IO.fromOption(() => UserError.InvalidJson)
      |> IO.flatMap(json =>
           json
           |> User.decodeLoginJson
           |> Result.mapError(_ => UserError.InvalidJson)
           |> IO.fromResult
         )
      // Authenticate user
      |> IO.flatMap(user => {
           let {username, password}: Shared__API__User.User.t = user;
           (username === Config.username && password === Config.password)
           |> (
             fun
             | false => Error(UserError.InvalidCredentials)
             | true => Ok(user)
           )
           |> IO.fromResult;
         })
      // User has been authenticated, create JWT token to send in response
      |> IO.flatMap(user => {
           let options =
             Some({
               ...JsonWebToken.emptyOptions,
               algorithm: HS256,
               expiresIn: "3 days",
             });
           let user = user |> Shared__API__User.User.encodeLoginJWTJson;
           let jwt =
             JsonWebToken.sign(
               ~secret=`string(Config.secretToken),
               ~options,
               `json(user),
             );
           Json_encode.object_([("token", Json_encode.string(jwt))])
           |> IO.pure;
         })
      |> Utils.log
      |> resolve(
           res,
           fun
           | Ok(x) => Response.sendJson(x)
           | _ => Response.sendStatus(Response.StatusCode.Unauthorized),
         )
    )
  });
