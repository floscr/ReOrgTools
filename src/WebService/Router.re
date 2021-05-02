open Relude.Globals;

[@react.component]
let make = () => {
  open State;
  open State__User;

  let dispatch = State.Store.useDispatch();
  let user = Store.useSelector(Selector.User.user);
  let optionUser = Store.useSelector(Selector.User.loggedInUser);

  React.useEffect1(
    () => {
      switch (optionUser) {
      | None =>
        Localforage.Localforage_IO.get(Types__LocalStorage.user)
        |> IO.flatMap(Js.Nullable.toOption >> IO.fromOption(() => "No User"))
        |> IO.flatMap(
             State__User.User.decode
             >> IO.fromResult
             >> IO.mapError(_ => "Could not Decode"),
           )
        |> IO.unsafeRunAsync(
             fun
             | Ok(user) =>
               State.UserAction(State__User.Login(user)) |> dispatch
             | _ => ignore(),
           )
      | _ => ignore()
      };
      None;
    },
    [|optionUser|],
  );

  switch (user) {
  | LoggedInUser(_) => <Router__LoggedIn />
  | _ => <Router__Unauthorized />
  };
};
