open ReactUtils;
open Relude.Globals;

module Styles = {
  open Css;
  open FixedTheme;

  let formRoot =
    style([
      display(flexBox),
      flexDirection(column),
      flexGrow(1.),
      flexShrink(1.),
      maxWidth(px(300)),
      padding(Spacing.large),
    ]);
};

type state = {
  username: string,
  password: string,
};

let initialState = {username: "", password: ""};

type action =
  | SetUser(string)
  | SetPassword(string)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | SetUser(username) => Update({...state, username})
  | SetPassword(password) => Update({...state, password})
  | NoOp => NoUpdate
  };

[@react.component]
let make = () => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);
  let dispatch = State.Store.useDispatch();

  let onChangeUser = event => {
    SetUser(event->ReactEvent.Form.target##value) |> send;
  };

  let onChangePassword = event => {
    SetPassword(event->ReactEvent.Form.target##value) |> send;
  };

  let onSubmit = (e: ReactEvent.Form.t): unit => {
    ReactEvent.Form.preventDefault(e);

    Shared__API__User.User.make(state.username, state.password)
    |> State__User.LoginRequest.make
    |> IO.unsafeRunAsync(
         fun
         | Ok(user) => {
             Localforage.Localforage_IO.set(
               Types__LocalStorage.user,
               user |> State__User.User.encode,
             )
             |> ignore;
             State.UserAction(State__User.Login(user)) |> dispatch;
           }
         | Error(data) => Js.log2("Error logging in", data),
       )
    |> ignore;
  };

  <form onSubmit className=Styles.formRoot>
    <label htmlFor="username"> {"Username" |> s} </label>
    <input
      autoFocus=true
      id="username"
      autoComplete="off"
      value={state.username}
      onChange=onChangeUser
      placeholder="User"
      name="username"
    />
    <label htmlFor="password"> {"Password" |> s} </label>
    <input
      id="password"
      autoComplete="off"
      value={state.password}
      onChange=onChangePassword
      placeholder="Password"
      name="password"
      type_="password"
    />
    <input type_="submit" value="Submit" />
  </form>;
};
